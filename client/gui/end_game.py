import tkinter as tk


class EndGameView(tk.Frame):
    def __init__(self, master):
        super().__init__(master)
        self.master = master
        self.build_ui()

    def build_ui(self):
        titulo = tk.Label(
            self,
            text="FIN DEL JUEGO",
            font=("Arial", 22, "bold")
        )
        titulo.pack(pady=30)

        self.lbl_resultado = tk.Label(
            self,
            text="Esperando resultado final...",
            font=("Arial", 14),
            fg="blue",
            wraplength=500,
            justify="center"
        )
        self.lbl_resultado.pack(pady=20)

        btn_volver = tk.Button(
            self,
            text="Volver al inicio",
            width=18,
            command=self.volver_inicio
        )
        btn_volver.pack(pady=20)

    def set_result_message(self, message):
        if message == "GAME_OVER BOTH":
            self.lbl_resultado.config(
                text="Resultado final: ambos jugadores acertaron.",
                fg="green"
            )

        elif message == "GAME_OVER J1":
            self.lbl_resultado.config(
                text="Resultado final: solo el jugador 1 acertó.",
                fg="blue"
            )

        elif message == "GAME_OVER J2":
            self.lbl_resultado.config(
                text="Resultado final: solo el jugador 2 acertó.",
                fg="blue"
            )

        elif message == "GAME_OVER NONE":
            self.lbl_resultado.config(
                text="Resultado final: ningún jugador acertó.",
                fg="red"
            )

        else:
            self.lbl_resultado.config(
                text="Resultado final desconocido.",
                fg="red"
            )

    def handle_server_message(self, message):
        if message.startswith("GAME_OVER"):
            self.set_result_message(message)

    def volver_inicio(self):
        self.master.close_connection()
        self.master.show_login()