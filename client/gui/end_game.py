import tkinter as tk
from tkinter import font as tkfont


class EndGameView(tk.Frame):
    def __init__(self, master):
        super().__init__(master, bg="#7EC8E3")
        self.master = master
        self.build_ui()

    def build_ui(self):
        card = tk.Frame(
            self,
            bg="#B8D4E8",
            relief="flat",
            highlightbackground="#5AABCC",
            highlightthickness=3
        )
        card.place(relx=0.5, rely=0.5, anchor="center")

        try:
            pixel_font = tkfont.Font(family="Press Start 2P", size=13, weight="bold")
        except Exception:
            pixel_font = tkfont.Font(family="Courier", size=13, weight="bold")

        result_font = tkfont.Font(family="Segoe UI", size=12, weight="bold")
        button_font = tkfont.Font(family="Segoe UI", size=11)
        status_font = tkfont.Font(family="Segoe UI", size=10)

        titulo = tk.Label(
            card,
            text="FIN DEL JUEGO",
            font=pixel_font,
            bg="#B8D4E8",
            fg="#1a3a50",
            wraplength=320,
            justify="center"
        )
        titulo.pack(padx=50, pady=(28, 20))

        self.lbl_resultado = tk.Label(
            card,
            text="Esperando resultado final...",
            font=result_font,
            bg="#B8D4E8",
            fg="#185FA5",
            wraplength=380,
            justify="center"
        )
        self.lbl_resultado.pack(padx=35, pady=(0, 22))

        btn_frame = tk.Frame(card, bg="#B8D4E8")
        btn_frame.pack(pady=(0, 20))

        btn_volver = tk.Button(
            btn_frame,
            text="Volver al inicio",
            font=button_font,
            width=16,
            command=self.volver_inicio,
            bg="#d6c9b8",
            fg="#3d2200",
            activebackground="#c4b49e",
            activeforeground="#3d2200",
            relief="flat",
            bd=0,
            highlightbackground="#9A7040",
            highlightthickness=2,
            cursor="hand2"
        )
        btn_volver.pack()

    def set_result_message(self, message):
        if message == "GAME_OVER BOTH":
            self.lbl_resultado.config(
                text="Resultado final: ambos jugadores acertaron",
                fg="#1a5c30"
            )

        elif message == "GAME_OVER J1":
            self.lbl_resultado.config(
                text="Resultado final: solo el jugador 1 acertó",
                fg="#185FA5"
            )

        elif message == "GAME_OVER J2":
            self.lbl_resultado.config(
                text="Resultado final: solo el jugador 2 acertó",
                fg="#185FA5"
            )

        elif message == "GAME_OVER NONE":
            self.lbl_resultado.config(
                text="Resultado final: ningún jugador acertó",
                fg="#c0392b"
            )

        else:
            self.lbl_resultado.config(
                text="Resultado final desconocido",
                fg="#c0392b"
            )

    def handle_server_message(self, message):
        if message.startswith("GAME_OVER"):
            self.set_result_message(message)

    def volver_inicio(self):
        self.master.close_connection()
        self.master.show_login()