import torch
import numpy as np
from transformers import AutoModelForSpeechSeq2Seq, AutoProcessor


class SpeechToText:
    #def __init__(self, model_name="openai/whisper-large-v3"):
    def __init__(self, model_name="openai/whisper-large-v3-turbo"):
        self.device = "cuda" if torch.cuda.is_available() else "cpu"
        self.dtype = torch.float16 if torch.cuda.is_available() else torch.float32

        print(f"[STT] Loading Whisper {model_name} on {self.device}")

        self.model = AutoModelForSpeechSeq2Seq.from_pretrained(
            model_name,
            torch_dtype=self.dtype,
            low_cpu_mem_usage=True,
            use_safetensors=True
        ).to(self.device)

        self.processor = AutoProcessor.from_pretrained(model_name)

    def transcribe(self, audio_np: np.ndarray) -> str:
        audio_np = np.asarray(audio_np, dtype=np.float32).squeeze()

        inputs = self.processor(
            audio_np,
            sampling_rate=16000,
            return_tensors="pt"
        )

        # Move to GPU
        input_features = inputs["input_features"].to(self.device, dtype=self.dtype)

        with torch.inference_mode():
            generated_ids = self.model.generate(
                input_features=input_features,
                max_new_tokens=256
            )

        text = self.processor.batch_decode(
            generated_ids,
            skip_special_tokens=True
        )[0]

        return text

