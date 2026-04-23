import tkinter as tk


class WordInputView(tk.Frame):
    def __init__(self, master):
        super().__init__(master)
        self.master = master
        self.build_ui()

    def build_ui(self):
        titulo = tk.Label(self, text="INGRESAR PALABRA", font=("Arial", 20, "bold"))
        titulo.pack(pady=30)

        instrucciones = tk.Label(
            self,
            text="Ingresa la palabra secreta para esta ronda",
            font=("Arial", 13)
        )
        instrucciones.pack(pady=10)

        form_frame = tk.Frame(self)
        form_frame.pack(pady=20)

        lbl_palabra = tk.Label(form_frame, text="Palabra:", font=("Arial", 12))
        lbl_palabra.grid(row=0, column=0, padx=10, pady=10, sticky="e")

        self.entry_palabra = tk.Entry(
            form_frame,
            width=20,
            font=("Arial", 14),
            justify="center"
        )
        self.entry_palabra.grid(row=0, column=1, padx=10, pady=10)

        self.entry_palabra.bind("<Return>", lambda event: self.enviar_palabra())

        self.btn_enviar = tk.Button(
            self,
            text="Enviar palabra",
            width=18,
            command=self.enviar_palabra
        )
        self.btn_enviar.pack(pady=15)

        self.lbl_estado = tk.Label(self, text="", font=("Arial", 11), fg="red")
        self.lbl_estado.pack(pady=10)

    def habilitar_input(self):
        self.entry_palabra.config(state="normal")
        self.btn_enviar.config(state="normal")
        self.entry_palabra.focus_set()

    def deshabilitar_input(self):
        self.entry_palabra.config(state="disabled")
        self.btn_enviar.config(state="disabled")

    def enviar_palabra(self):
        palabra = self.entry_palabra.get().strip()

        if palabra == "":
            self.lbl_estado.config(text="Ingresa una palabra", fg="red")
            return

        if len(palabra) != 5:
            self.lbl_estado.config(text="La palabra debe tener 5 caracteres", fg="red")
            return

        mensaje = f"SETWORD {palabra}"
        self.master.send_server(mensaje)

        self.lbl_estado.config(text="Enviando palabra...", fg="blue")
        self.entry_palabra.delete(0, tk.END)
        self.deshabilitar_input()

    def handle_server_message(self, message):
        if message == "YOUR_TURN_SETWORD":
            self.habilitar_input()
            self.lbl_estado.config(text="Es tu turno de ingresar palabra", fg="green")

        elif message == "WAITING_PLAYER":
            self.deshabilitar_input()
            self.master.show_waiting("Esperando a que el otro jugador adivine...")

        elif message == "ERROR INVALID_WORD":
            self.habilitar_input()
            self.lbl_estado.config(text="Palabra inválida", fg="red")

        elif message == "ERROR INVALID_FORMAT":
            self.habilitar_input()
            self.lbl_estado.config(text="Formato inválido", fg="red")

        elif message == "ERROR NOT_YOUR_TURN":
            self.deshabilitar_input()
            self.lbl_estado.config(text="No es tu turno", fg="red")