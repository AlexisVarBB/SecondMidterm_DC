import tkinter as tk
from tkinter import font as tkfont


class MainWordleView(tk.Frame):
    def __init__(self, master):
        super().__init__(master, bg="#7EC8E3")
        self.master = master
        self.row_actual = 0
        self.ultimo_guess = ""
        self.celdas = []
        self.build_ui()

    def build_ui(self):
        
        card = tk.Frame(self, bg="#B8D4E8", relief="flat",
                        highlightbackground="#5AABCC", highlightthickness=3)
        card.place(relx=0.5, rely=0.5, anchor="center")

        
        try:
            pixel_font = tkfont.Font(family="Press Start 2P", size=18, weight="bold")
        except Exception:
            pixel_font = tkfont.Font(family="Courier", size=18, weight="bold")

        status_font = tkfont.Font(family="Segoe UI", size=11)
        cell_font   = tkfont.Font(family="Courier New", size=16, weight="bold")
        entry_font  = tkfont.Font(family="Courier New", size=14, weight="bold")
        button_font = tkfont.Font(family="Segoe UI", size=11)

        
        tk.Label(card, text="WORDLE", font=pixel_font,
                 bg="#B8D4E8", fg="#1a3a50").pack(padx=50, pady=(24, 8))

        # Estado
        self.lbl_state = tk.Label(card, text="Esperando turno...",
                                  font=status_font, bg="#B8D4E8", fg="#185FA5")
        self.lbl_state.pack(pady=(0, 12))

       
        tablero_frame = tk.Frame(card, bg="#B8D4E8")
        tablero_frame.pack(padx=30, pady=(0, 14))

        for i in range(6):
            fila = []
            for j in range(5):
                celda = tk.Label(
                    tablero_frame, text="", width=3, height=1,
                    font=cell_font, bg="#daeaf5", fg="#1a3a50",
                    relief="flat",
                    highlightbackground="#5AABCC", highlightthickness=2
                )
                celda.grid(row=i, column=j, padx=4, pady=4)
                fila.append(celda)
            self.celdas.append(fila)

        
        input_frame = tk.Frame(card, bg="#B8D4E8")
        input_frame.pack(pady=(0, 14))

        self.entry_guess = tk.Entry(
            input_frame, width=8, font=entry_font, justify="center",
            bg="#daeaf5", fg="#1a3a50", insertbackground="#1a3a50",
            relief="flat", highlightbackground="#5AABCC", highlightthickness=2,
            state="disabled"
        )
        self.entry_guess.grid(row=0, column=0, padx=(0, 10))
        self.entry_guess.bind("<Return>", lambda e: self.enviar_guess())
        self.entry_guess.bind("<KeyRelease>", self._mayusculas)

        self.btn_enviar = tk.Button(
            input_frame, text="Enviar intento",
            font=button_font, width=14,
            bg="#C9A97A", fg="#3d2200",
            activebackground="#B8915F", activeforeground="#3d2200",
            relief="flat", bd=0,
            highlightbackground="#9A7040", highlightthickness=2,
            cursor="hand2", command=self.enviar_guess,
            state="disabled"
        )
        self.btn_enviar.grid(row=0, column=1)

       
        self.lbl_error = tk.Label(card, text="", font=status_font,
                                  bg="#B8D4E8", fg="#c0392b")
        self.lbl_error.pack(pady=(0, 20))

   

    def _mayusculas(self, event=None):
        val = self.entry_guess.get().upper()
        self.entry_guess.delete(0, tk.END)
        self.entry_guess.insert(0, val)

    def _set_status(self, texto, color="#185FA5"):
        self.lbl_state.config(text=texto, fg=color)

    def _set_error(self, texto, color="#c0392b"):
        self.lbl_error.config(text=texto, fg=color)

  

    def habilitar_tablero(self):
        if self.row_actual >= 6:
            return
        self.entry_guess.config(state="normal")
        self.btn_enviar.config(state="normal", bg="#C9A97A", cursor="hand2")
        self.entry_guess.focus_set()
        self._set_status("Es tu turno de adivinar", "#1a5c30")
        self._set_error("")

    def deshabilitar_tablero(self):
        self.entry_guess.config(state="disabled")
        self.btn_enviar.config(state="disabled", bg="#d6c9b8", cursor="arrow")

   

    def enviar_guess(self):
        palabra = self.entry_guess.get().strip()

        if palabra == "":
            self._set_error("Ingresa una palabra")
            return
        if len(palabra) != 5:
            self._set_error("La palabra debe tener 5 caracteres")
            return

        self.ultimo_guess = palabra.upper()
        self.master.send_server(f"GUESS {palabra}")
        self._set_status("Enviando intento...", "#185FA5")
        self._set_error("")
        self.entry_guess.delete(0, tk.END)
        self.deshabilitar_tablero()

    def pintar_resultado(self, palabra, resultados):
        if self.row_actual >= 6:
            return

        colores = {
            2: ("#7dcba0", "#3a8f62", "#0d3320"),   
            1: ("#e8d87a", "#b09a20", "#3d2e00"),   
            0: ("#a8b8c4", "#6a8494", "#1a3a50"),   
        }

        for j in range(min(5, len(palabra))):
            bg, border, fg = colores.get(resultados[j], colores[0])
            self.celdas[self.row_actual][j].config(
                text=palabra[j],
                bg=bg, fg=fg,
                highlightbackground=border
            )

        self.row_actual += 1

    def handle_server_message(self, message):
        if message == "YOUR_TURN_GUESS":
            self.habilitar_tablero()

        elif message == "WAITING_PLAYER":
            self.deshabilitar_tablero()
            self._set_status("Esperando al otro jugador...", "#185FA5")

        elif message == "ERROR INVALID_WORD":
            self.habilitar_tablero()
            self._set_error("Palabra inválida")

        elif message == "ERROR NOT_YOUR_TURN":
            self.deshabilitar_tablero()
            self._set_error("No es tu turno")

        elif message == "ROUND_END":
            self.deshabilitar_tablero()
            self._set_status("La ronda terminó", "#185FA5")

        elif message.startswith("GAME_OVER"):
            self.deshabilitar_tablero()

        elif message.startswith("RESULT"):
            partes = message.split()
            if len(partes) == 6:
                try:
                    resultados = list(map(int, partes[1:6]))
                    self.pintar_resultado(self.ultimo_guess, resultados)
                    self._set_status("Resultado recibido", "#185FA5")
                except ValueError:
                    self._set_error("Resultado inválido recibido")