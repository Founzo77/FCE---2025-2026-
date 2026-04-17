import numpy as np
import json
from vad import VoiceActivityDetector

class AIPipeline:
    def __init__(self, audio, stt, command_model, client):
        self.audio = audio
        self.stt = stt
        self.command_model = command_model
        self.client = client
        self.running = True
        self.vad = VoiceActivityDetector()

    def run(self):
        self.audio.start()
        print("🎤 Listening...")

        vad_buffer = []
        speech_buffer = []
        speaking = False
        silence_frames = 0

        VAD_FRAMES = 8        # 8 × 50ms = 400ms for VAD
        SILENCE_FRAMES = 2  # ~500ms

        while self.running:
            chunk = self.audio.read()
            chunk = np.asarray(chunk)

            if chunk.ndim == 2:
                chunk = np.mean(chunk, axis=1)

            if chunk.dtype == np.int16:
                chunk = chunk.astype(np.float32) / 32768.0

            vad_buffer.append(chunk)

            # Need enough audio for VAD
            if len(vad_buffer) < VAD_FRAMES:
                continue

            vad_audio = np.concatenate(vad_buffer)
            vad_buffer.clear()

            energy = np.mean(np.abs(vad_audio))
            is_speech = self.vad.has_speech(vad_audio)

            print(f"Mic energy: {energy:.5f} | VAD: {is_speech}")

            if is_speech:
                speech_buffer.append(vad_audio)
                speaking = True
                silence_frames = 0
            else:
                if speaking:
                    silence_frames += 1

            # End of utterance
            if speaking and silence_frames > SILENCE_FRAMES:
                audio_np = np.concatenate(speech_buffer)
                speech_buffer.clear()
                speaking = False
                silence_frames = 0

                print("🧠 Processing...")

                text = self.stt.transcribe(audio_np)

                if not text.strip():
                    print("… (empty)")
                    continue

                print("🗣️", text)

                raw = self.command_model.generate(text)
                print("🤖", raw)

                try:
                    data = extract_json(raw)
                    cmds = data.get("commands", [])

                    for cmd in cmds:
                        print("➡ Sending:", cmd)
                        self.client.send_command(cmd)

                except Exception as e:
                    print("⚠ JSON parse error:", e)

def extract_json(text: str):
    # Enlève ```json ... ``` si présent
    text = text.strip()

    if text.startswith("```"):
        text = text.split("```")[1]

    # Trouve le premier { et le dernier }
    start = text.find("{")
    end = text.rfind("}")

    if start == -1 or end == -1:
        raise ValueError("No JSON object found")

    return json.loads(text[start:end+1])

