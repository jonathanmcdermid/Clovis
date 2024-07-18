import subprocess

class UCIEngine:
    def __init__(self, engine_path, hash_size=16):
        self.engine_path = engine_path
        self.process = subprocess.Popen(
            engine_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )
        self.send_command('uci')
        self.receive_output()
        self.send_command(f'setoption name Hash value {hash_size}')
        self.send_command('isready')
        self.receive_output()
        
    def send_command(self, command):
        """Send a command to the UCI engine."""
        self.process.stdin.write(f"{command}\n")
        self.process.stdin.flush()

    def receive_output(self, until='readyok'):
        """Receive output from the UCI engine until a specific message."""
        output = []
        while True:
            line = self.process.stdout.readline().strip()
            if line == until or line == 'uciok':
                break
            if line:
                output.append(line)
        return output

    def close(self):
        """Close the UCI engine process."""
        self.send_command('quit')
        self.process.stdin.flush()
        self.process.wait()
