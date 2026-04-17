import requests
import queue
import threading

class CommandQueue:
    def __init__(self):
        self.q = queue.Queue()

    def push(self, cmd):
        self.q.put(cmd)

    def pop(self):
        return self.q.get()

class EngineClient:
    def __init__(self, host="127.0.0.1", port=8080, scheme="http"):
        self.base_url = f"{scheme}://{host}:{port}"
        self.queue = CommandQueue()
        self.running = True

        self.thread = threading.Thread(target=self.worker, daemon=True)
        self.thread.start()

    def send_command(self, cmd):
        self.queue.push(cmd)

    def stop(self):
        self.running = False
        # débloque le thread si bloqué sur .get()
        self.queue.push(None)

    def worker(self):
        while self.running:
            cmd = self.queue.pop()
            if cmd is None:
                break

            try:
                r = requests.post(
                    f"{self.base_url}/{cmd['command']}",
                    json=cmd,
                    timeout=2
                )
                if r.status_code != 200:
                    print("❌ Engine error:", r.text)
            except Exception as e:
                print("❌ Network error:", e)
