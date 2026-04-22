import tkinter as tk


class WaitingView(tk.Frame):
    def __init__(self, master, mensaje="Esperando al otro jugador..."):
        super().__init__(master)
        self.master = master
        self.mensaje = mensaje
        self.build_ui()

    def build_ui(self):
        titulo = tk.Label(self, text="SALA DE ESPERA", font=("Arial", 20, "bold"))
        titulo.pack(pady=40)

        self.lbl_mensaje = tk.Label(self, text=self.mensaje, font=("Arial", 14))
        self.lbl_mensaje.pack(pady=20)

        self.lbl_estado = tk.Label(self, text="Conexión activa", font=("Arial", 11), fg="green")
        self.lbl_estado.pack(pady=10)

        btn_salir = tk.Button(self, text="Desconectar", width=15, command=self.desconectar)
        btn_salir.pack(pady=25)

    def actualizar_mensaje(self, mensaje):
        self.lbl_mensaje.config(text=mensaje)

    def set_status(self, mensaje):
        self.lbl_estado.config(text=mensaje)

    def handle_server_message(self, message):
        if message == "START_GAME":
            self.set_status("Juego listo")

    def desconectar(self):
        try:
            self.master.send_server("DISCONNECT")
        except Exception:
            pass

        self.master.close_connection()
        self.master.show_login()