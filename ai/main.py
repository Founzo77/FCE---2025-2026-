from audio import AudioStream
from stt import SpeechToText
from command_model import CommandModel
from client import EngineClient
from pipeline import AIPipeline
import signal

audio = AudioStream()
stt = SpeechToText()
cmd_model = CommandModel()
client = EngineClient("127.0.0.1", 8080)

pipeline = AIPipeline(audio, stt, cmd_model, client)

def shutdown(sig, frame):
    print("\n🛑 Stopping...")
    pipeline.running = False
    #pipeline.audio.stop()
    
def main():
    pipeline.run()

if __name__ == "__main__":
    signal.signal(signal.SIGINT, shutdown)
    main()
