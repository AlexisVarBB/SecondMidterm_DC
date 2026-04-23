import tkinter as tk
from network import TCPClient
from gui.login_view import LoginView
from gui.register_view import RegisterView
from gui.waiting_view import WaitingView
from gui.word_input_view import WordInputView
from gui.main_wordle_view import MainWordleView
from gui.end_game import EndGameView
from listener import ServerListener


class App(tk.Tk):
    def __init__(self):
        super().__init__()

        self.title("Wordle Client")
        self.geometry("700x300")
        self.resizable(True, True)

        self.client = TCPClient()
        self.listener = ServerListener(self)
        self.connected = False
        self.current_view = None
        self.game_finished = False

        self.show_login()

    def clear_view(self):
        if self.current_view is not None:
            self.current_view.destroy()
            self.current_view = None

    def show_login(self):
        self.clear_view()
        self.game_finished = False
        self.current_view = LoginView(self)
        self.current_view.pack(fill="both", expand=True)

    def show_register(self):
        self.clear_view()
        self.current_view = RegisterView(self)
        self.current_view.pack(fill="both", expand=True)

    def show_waiting(self, mensaje="Esperando al otro jugador..."):
        self.clear_view()
        self.current_view = WaitingView(self, mensaje)
        self.current_view.pack(fill="both", expand=True)

    def ensure_connection(self):
        if self.connected:
            return True

        if self.client.connect():
            self.connected = True
            self.listener.start()
            return True

        return False

    def send_server(self, message):
        self.client.send_message(message)

    def recv_server(self):
        return self.client.receive_message()

    def close_connection(self):
        if self.connected:
            self.listener.stop()
            self.client.close()
            self.connected = False

    def safe_handle_server_message(self, message):
        self.after(0, lambda: self.handle_server_message(message))

    def handle_server_message(self, message):
        if message == "WAITING_PLAYER":
            self.show_waiting("Esperando al otro jugador...")

        elif message == "START_GAME":
            if self.current_view is not None and hasattr(self.current_view, "set_status"):
                self.current_view.set_status("Juego listo")

        elif message == "OPPONENT_DISCONNECTED":
            if not self.game_finished:
                self.close_connection()
                self.show_login()

        elif message == "YOUR_TURN_SETWORD":
            if not isinstance(self.current_view, WordInputView):
                self.show_word_input()

            if self.current_view is not None and hasattr(self.current_view, "handle_server_message"):
                self.current_view.handle_server_message(message)

        elif message == "YOUR_TURN_GUESS":
            if not isinstance(self.current_view, MainWordleView):
                self.show_main_wordle()

            if self.current_view is not None and hasattr(self.current_view, "handle_server_message"):
                self.current_view.handle_server_message(message)
                
        elif message.startswith("GAME_OVER"):
            self.game_finished = True
            self.show_end_game()

            if self.current_view is not None and hasattr(self.current_view, "handle_server_message"):
                self.current_view.handle_server_message(message)

        elif self.current_view is not None and hasattr(self.current_view, "handle_server_message"):
            self.current_view.handle_server_message(message)

    def show_word_input(self):
        self.clear_view()
        self.current_view = WordInputView(self)
        self.current_view.pack(fill="both", expand=True)

    def show_main_wordle(self):
        self.clear_view()
        self.current_view = MainWordleView(self)
        self.current_view.pack(fill="both", expand=True)
        
    def show_end_game(self):
        self.clear_view()
        self.current_view = EndGameView(self)
        self.current_view.pack(fill="both", expand=True)