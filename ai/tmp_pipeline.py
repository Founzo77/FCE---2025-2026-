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

        buffer = []

        while self.running:
            chunk = self.audio.read()
            buffer.append(chunk)

            # accumulate ~0.5s of audio (20 * 25ms)
            if len(buffer) < 20:
                continue

            audio_np = np.concatenate(buffer, axis=0)
            buffer.clear()

            # Convert stereo → mono
            if audio_np.ndim == 2:
                audio_np = np.mean(audio_np, axis=1)

            if not self.vad.has_speech(audio_np):
                continue

            text = self.stt.transcribe(audio_np)

            if not text.strip():
                continue

            print("🗣️", text)

            raw = self.command_model.generate(text)
            print("🤖", raw)

            # try:
            #     cmd = json.loads(raw)
            #     self.client.send_command(cmd)
            # except Exception as e:
            #     print("⚠ Invalid command:", e, raw)
