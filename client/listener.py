import threading


class ServerListener:
    def __init__(self, app):
        self.app = app
        self.running = False
        self.thread = None

    def start(self):
        if self.running:
            return

        self.running = True
        self.thread = threading.Thread(target=self.listen_loop, daemon=True)
        self.thread.start()

    def stop(self):
        self.running = False

    def listen_loop(self):
        while self.running:
            try:
                respuesta = self.app.recv_server()

                if respuesta is None:
                    self.app.safe_handle_server_message("OPPONENT_DISCONNECTED")
                    break

                self.app.safe_handle_server_message(respuesta)

            except Exception:
                self.app.safe_handle_server_message("OPPONENT_DISCONNECTED")
                break