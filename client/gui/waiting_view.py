import tkinter as tk
from tkinter import font as tkfont


class WaitingView(tk.Frame):
    def __init__(self, master, mensaje="Esperando al otro jugador..."):
        super().__init__(master, bg="#7EC8E3")
        self.master = master
        self.mensaje = mensaje
        self._dot_count = 0
        self.build_ui()

    def build_ui(self):
        
        card = tk.Frame(self, bg="#B8D4E8", relief="flat", bd=0,
                        highlightbackground="#5AABCC", highlightthickness=3)
        card.place(relx=0.5, rely=0.5, anchor="center")

        
        try:
            pixel_font = tkfont.Font(family="Press Start 2P", size=14, weight="bold")
        except Exception:
            pixel_font = tkfont.Font(family="Courier", size=14, weight="bold")

        normal_font  = tkfont.Font(family="Segoe UI", size=11)
        status_font  = tkfont.Font(family="Segoe UI", size=10)
        button_font  = tkfont.Font(family="Segoe UI", size=11)

      
        self.lbl_icon = tk.Label(card, text="[>]", font=("Courier", 26, "bold"),
                                 bg="#B8D4E8", fg="#1a3a50")
        self.lbl_icon.pack(pady=(30, 5))
        self._blink_icon()

        
        titulo = tk.Label(card, text="SALA DE ESPERA",
                          font=pixel_font, bg="#B8D4E8", fg="#1a3a50",
                          wraplength=280, justify="center")
        titulo.pack(padx=40, pady=(10, 20))

        
        self.lbl_mensaje = tk.Label(card, text=self.mensaje,
                                    font=normal_font, bg="#B8D4E8", fg="#1a3a50")
        self.lbl_mensaje.pack(pady=(0, 6))

        
        self.lbl_estado = tk.Label(card, text="● Conexión activa",
                                   font=status_font, bg="#c4ecda", fg="#1a5c30",
                                   padx=12, pady=4, relief="flat",
                                   highlightbackground="#5abf8a", highlightthickness=1)
        self.lbl_estado.pack(pady=8)

       
        btn_salir = tk.Button(
            card, text="Desconectar",
            font=button_font, width=14,
            bg="#C9A97A", fg="#3d2200",
            activebackground="#B8915F", activeforeground="#3d2200",
            relief="flat", bd=0,
            highlightbackground="#9A7040", highlightthickness=2,
            cursor="hand2", command=self.desconectar
        )
        btn_salir.pack(pady=(10, 30))

  

    def _blink_icon(self):
        icons = ["[>]", "[ ]"]
        current = self.lbl_icon.cget("text")
        self.lbl_icon.config(text=icons[1] if current == icons[0] else icons[0])
        self.after(700, self._blink_icon)

    

    def actualizar_mensaje(self, mensaje):
        self.lbl_mensaje.config(text=mensaje)

    def set_status(self, mensaje):
        self.lbl_estado.config(text=mensaje)

    def handle_server_message(self, message):
        if message == "START_GAME":
            self.set_status("✔ Juego listo")
            self.lbl_estado.config(bg="#c4ecda", fg="#1a5c30")

    def desconectar(self):
        try:
            self.master.send_server("DISCONNECT")
        except Exception:
            pass
        self.master.close_connection()
        self.master.show_login()