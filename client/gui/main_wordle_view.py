import tkinter as tk


class MainWordleView(tk.Frame):
    def __init__(self, master):
        super().__init__(master)
        self.master = master
        self.row_actual = 0
        self.ultimo_guess = ""
        self.celdas = []
        self.build_ui()

    def build_ui(self):
        titulo = tk.Label(self, text="WORDLE", font=("Arial", 20, "bold"))
        titulo.pack(pady=20)

        self.lbl_state = tk.Label(self, text="Esperando turno...", font=("Arial", 12), fg="blue")
        self.lbl_state.pack(pady=10)

        tablero_frame = tk.Frame(self)
        tablero_frame.pack(pady=15)

        for i in range(5):
            fila = []
            for j in range(5):
                celda = tk.Label(
                    tablero_frame,
                    text="",
                    width=4,
                    height=2,
                    font=("Arial", 18, "bold"),
                    relief="solid",
                    borderwidth=1,
                    bg="white"
                )
                celda.grid(row=i, column=j, padx=4, pady=4)
                fila.append(celda)
            self.celdas.append(fila)

        input_frame = tk.Frame(self)
        input_frame.pack(pady=20)

        self.entry_guess = tk.Entry(input_frame, width=15, font=("Arial", 16), justify="center", state="disabled")
        self.entry_guess.grid(row=0, column=0, padx=10)

        self.btn_enviar = tk.Button(input_frame, text="Enviar intento", width=15, command=self.enviar_guess, state="disabled")
        self.btn_enviar.grid(row=0, column=1, padx=10)

    def habilitar_tablero(self):
        self.entry_guess.config(state="normal")
        self.btn_enviar.config(state="normal")
        self.lbl_state.config(text="Es tu turno de adivinar", fg="green")

    def deshabilitar_tablero(self):
        self.entry_guess.config(state="disabled")
        self.btn_enviar.config(state="disabled")

    def enviar_guess(self):
        palabra = self.entry_guess.get().strip()

        if palabra == "":
            self.lbl_state.config(text="Ingresa una palabra", fg="red")
            return

        self.ultimo_guess = palabra.upper()
        mensaje = f"GUESS {palabra}"
        self.master.send_server(mensaje)
        self.lbl_state.config(text="Enviando intento...", fg="blue")
        self.entry_guess.delete(0, tk.END)

    def pintar_resultado(self, palabra, resultados):
        if self.row_actual >= 5:
            return

        for j in range(min(5, len(palabra))):
            color = "lightgray"

            if resultados[j] == 2:
                color = "lightgreen"
            elif resultados[j] == 1:
                color = "khaki"

            self.celdas[self.row_actual][j].config(text=palabra[j], bg=color)

        self.row_actual += 1

    def handle_server_message(self, message):
        if message == "YOUR_TURN_GUESS":
            self.habilitar_tablero()

        elif message == "WAITING_PLAYER":
            self.deshabilitar_tablero()
            self.lbl_state.config(text="Esperando al otro jugador...", fg="blue")

        elif message == "ERROR INVALID_WORD":
            self.lbl_state.config(text="Palabra inválida", fg="red")

        elif message == "ERROR NOT_YOUR_TURN":
            self.lbl_state.config(text="No es tu turno", fg="red")

        elif message.startswith("RESULT"):
            partes = message.split()

            if len(partes) == 6:
                try:
                    resultados = list(map(int, partes[1:6]))
                    palabra = self.ultimo_guess
                    self.pintar_resultado(palabra, resultados)
                    self.habilitar_tablero()
                except ValueError:
                    self.lbl_state.config(text="Resultado inválido recibido", fg="red")