import torch
import json

from transformers import (
    Mistral3ForConditionalGeneration,
    MistralCommonBackend,
    FineGrainedFP8Config
)

from engine_schema import ENGINE_TOOLS
from scene_provider import get_scene_description


class CommandModel:
    def __init__(self, model_name="mistralai/Ministral-3-8B-Instruct-2512"):
        print(f"[LLM] Loading {model_name}")

        self.device = "cuda" if torch.cuda.is_available() else "cpu"

        # Load model in FP8 → dequantized to BF16 internally (stable)
        self.model = Mistral3ForConditionalGeneration.from_pretrained(
            model_name,
            device_map="auto",
            quantization_config=FineGrainedFP8Config(dequantize=True)
        )

        self.tokenizer = MistralCommonBackend.from_pretrained(model_name)
        self.model.generation_config.max_length = 1000000

    # ----------------------------------------
    # Prompt construction (Mistral Chat format)
    # ----------------------------------------
    def build_messages(self, user_text: str):
        scene = get_scene_description()
        tools = json.dumps(ENGINE_TOOLS, indent=2)

        system_prompt = (
            "You are a real-time AI controlling a 3D rendering engine.\n\n"
            "You must output a JSON object with this format:\n"
            '{"commands":[{"command":"...","args":{...}}]}\n\n'
            "You may output zero, one, or multiple commands.\n"
            "If the user did not ask to modify the scene, output an empty list.\n\n"
            "Rules:\n"
            "- Output JSON only\n"
            "- No markdown\n"
            "- No explanation\n\n"
            "Scene:\n"
            f"{scene}\n\n"
            "Available commands:\n"
            f"{tools}\n"
        )

        return [
            {
                "role": "system",
                "content": [
                    {"type": "text", "text": system_prompt}
                ],
            },
            {
                "role": "user",
                "content": [
                    {"type": "text", "text": user_text}
                ],
            },
        ]

    # ----------------------------------------
    # Generation
    # ----------------------------------------
    def generate(self, user_text: str) -> str:
        messages = self.build_messages(user_text)

        tokenized = self.tokenizer.apply_chat_template(
            messages,
            return_tensors="pt",
            return_dict=True
        )

        # Move everything to the model device
        for k in tokenized:
            tokenized[k] = tokenized[k].to(self.model.device)

        # Build explicit attention mask (fixes pad==eos issue)
        attention_mask = (tokenized["input_ids"] != self.tokenizer.eos_token_id).long()

        with torch.inference_mode():
            output = self.model.generate(
                input_ids=tokenized["input_ids"],
                attention_mask=attention_mask,
                max_new_tokens=128,
                do_sample=False,
                use_cache=True
            )[0]

        prompt_len = tokenized["input_ids"].shape[1]
        decoded = self.tokenizer.decode(
            output[prompt_len:],
            skip_special_tokens=True
        )

        return decoded.strip()
