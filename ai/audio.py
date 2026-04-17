import sounddevice as sd
import queue
import numpy as np

class AudioStream:
    def __init__(self, samplerate=16000):
        self.q = queue.Queue()
        self.samplerate = samplerate

    def _callback(self, indata, frames, time, status):
        self.q.put(indata.copy())

    def start(self):
        self.stream = sd.InputStream(
            samplerate=self.samplerate,
            channels=1,
            callback=self._callback
        )
        self.stream.start()

    def read(self):
        return self.q.get()
