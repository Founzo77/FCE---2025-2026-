import torch
import numpy as np
import silero_vad

class VoiceActivityDetector:
    def __init__(self):
        self.model = silero_vad.load_silero_vad()

    def has_speech(self, audio_np: np.ndarray, sample_rate=16000) -> bool:
        audio = torch.from_numpy(audio_np).float()

        speech_timestamps = silero_vad.get_speech_timestamps(
            audio,
            self.model,
            sampling_rate=sample_rate,
            threshold=0.05,                # ← was 0.2 (too high!)
            min_speech_duration_ms=80,     # detect short words
            min_silence_duration_ms=150,   # allow fast pauses
            window_size_samples=512
        )

        return len(speech_timestamps) > 0
