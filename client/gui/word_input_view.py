import tkinter as tk
from tkinter import font as tkfont


class WordInputView(tk.Frame):
    def __init__(self, master):
        super().__init__(master, bg="#7EC8E3")
        self.master = master
        self.build_ui()

    def build_ui(self):
        # --- Tarjeta central ---
        card = tk.Frame(self, bg="#B8D4E8", relief="flat",
                        highlightbackground="#5AABCC", highlightthickness=3)
        card.place(relx=0.5, rely=0.5, anchor="center")

        
        try:
            pixel_font = tkfont.Font(family="Press Start 2P", size=12, weight="bold")
        except Exception:
            pixel_font = tkfont.Font(family="Courier", size=12, weight="bold")

        label_font  = tkfont.Font(family="Segoe UI", size=11, weight="bold")
        instr_font  = tkfont.Font(family="Segoe UI", size=11)
        entry_font  = tkfont.Font(family="Courier New", size=15, weight="bold")
        button_font = tkfont.Font(family="Segoe UI", size=11)
        status_font = tkfont.Font(family="Segoe UI", size=10)
        box_font    = tkfont.Font(family="Courier New", size=16, weight="bold")

       
        tk.Label(card, text="INGRESAR PALABRA",
                 font=pixel_font, bg="#B8D4E8", fg="#1a3a50",
                 wraplength=260, justify="center").pack(padx=50, pady=(26, 8))

       
        tk.Label(card, text="Ingresa la palabra secreta para esta ronda",
                 font=instr_font, bg="#B8D4E8", fg="#1a3a50").pack(pady=(0, 14))

     
        boxes_frame = tk.Frame(card, bg="#B8D4E8")
        boxes_frame.pack(pady=(0, 10))
        self.letter_boxes = []
        for i in range(5):
            box = tk.Label(boxes_frame, text=" ", width=2,
                           font=box_font, bg="#daeaf5", fg="#1a3a50",
                           relief="flat",
                           highlightbackground="#5AABCC", highlightthickness=2)
            box.grid(row=0, column=i, padx=5)
            self.letter_boxes.append(box)

       
        row_frame = tk.Frame(card, bg="#B8D4E8")
        row_frame.pack(pady=(4, 12))

        tk.Label(row_frame, text="Palabra:", font=label_font,
                 bg="#B8D4E8", fg="#1a3a50").grid(row=0, column=0, padx=(0, 10))

        self.entry_palabra = tk.Entry(
            row_frame, width=10, font=entry_font, justify="center",
            bg="#daeaf5", fg="#1a3a50", insertbackground="#1a3a50",
            relief="flat", highlightbackground="#5AABCC", highlightthickness=2
        )
        self.entry_palabra.grid(row=0, column=1)
        self.entry_palabra.bind("<Return>", lambda e: self.enviar_palabra())
        self.entry_palabra.bind("<KeyRelease>", self._actualizar_cajas)

     
        self.btn_enviar = tk.Button(
            card, text="Enviar palabra",
            font=button_font, width=16,
            bg="#C9A97A", fg="#3d2200",
            activebackground="#B8915F", activeforeground="#3d2200",
            relief="flat", bd=0,
            highlightbackground="#9A7040", highlightthickness=2,
            cursor="hand2", command=self.enviar_palabra
        )
        self.btn_enviar.pack(pady=(0, 10))

        # Estado
        self.lbl_estado = tk.Label(card, text="", font=status_font,
                                   bg="#B8D4E8", fg="#c0392b")
        self.lbl_estado.pack(pady=(0, 20))

  

    def _actualizar_cajas(self, event=None):
        texto = self.entry_palabra.get().upper()[:5]
        for i, box in enumerate(self.letter_boxes):
            box.config(text=texto[i] if i < len(texto) else " ",
                       fg="#1a3a50" if i < len(texto) else "#aac8dc")

   

    def habilitar_input(self):
        self.entry_palabra.config(state="normal")
        self.btn_enviar.config(state="normal",
                               bg="#C9A97A", cursor="hand2")
        self.entry_palabra.focus_set()

    def deshabilitar_input(self):
        self.entry_palabra.config(state="disabled")
        self.btn_enviar.config(state="disabled",
                               bg="#d6c9b8", cursor="arrow")

    def enviar_palabra(self):
        palabra = self.entry_palabra.get().strip().upper()

        if palabra == "":
            self.lbl_estado.config(text="Ingresa una palabra", fg="#c0392b")
            return

        if len(palabra) != 5:
            self.lbl_estado.config(text="La palabra debe tener 5 caracteres", fg="#c0392b")
            return

        self.master.send_server(f"SETWORD {palabra}")
        self.lbl_estado.config(text="Enviando palabra...", fg="#185FA5")
        self.entry_palabra.delete(0, tk.END)
        self._actualizar_cajas()
        self.deshabilitar_input()

    def handle_server_message(self, message):
        acciones = {
            "YOUR_TURN_SETWORD":  (self.habilitar_input,     "Es tu turno de ingresar palabra", "#1a5c30"),
            "ERROR INVALID_WORD": (self.habilitar_input,     "Palabra inválida",                "#c0392b"),
            "ERROR INVALID_FORMAT":(self.habilitar_input,    "Formato inválido",                "#c0392b"),
            "ERROR NOT_YOUR_TURN":(self.deshabilitar_input,  "No es tu turno",                  "#c0392b"),
        }
        if message == "WAITING_PLAYER":
            self.deshabilitar_input()
            self.master.show_waiting("Esperando a que el otro jugador adivine...")
            return
        if message in acciones:
            fn, texto, color = acciones[message]
            fn()
            self.lbl_estado.config(text=texto, fg=color)