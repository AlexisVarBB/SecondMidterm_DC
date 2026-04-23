import tkinter as tk
from tkinter import font as tkfont


class RegisterView(tk.Frame):
    def __init__(self, master):
        super().__init__(master, bg="#7EC8E3")
        self.master = master
        self.build_ui()

    def build_ui(self):
       
        card = tk.Frame(self, bg="#B8D4E8", relief="flat",
                        highlightbackground="#5AABCC", highlightthickness=3)
        card.place(relx=0.5, rely=0.5, anchor="center")

       
        try:
            pixel_font = tkfont.Font(family="Press Start 2P", size=13, weight="bold")
        except Exception:
            pixel_font = tkfont.Font(family="Courier", size=13, weight="bold")

        label_font  = tkfont.Font(family="Segoe UI", size=11, weight="bold")
        entry_font  = tkfont.Font(family="Segoe UI", size=11)
        button_font = tkfont.Font(family="Segoe UI", size=11)
        status_font = tkfont.Font(family="Segoe UI", size=10)

        
        titulo = tk.Label(card, text="REGISTER",
                          font=pixel_font, bg="#B8D4E8", fg="#1a3a50",
                          wraplength=280, justify="center")
        titulo.pack(padx=50, pady=(28, 20))

        
        form = tk.Frame(card, bg="#B8D4E8")
        form.pack(padx=40, pady=(0, 14))

     
        tk.Label(form, text="Nuevo usuario:", font=label_font,
                 bg="#B8D4E8", fg="#1a3a50", anchor="e").grid(
                 row=0, column=0, padx=(0, 10), pady=8, sticky="e")

        self.entry_usuario = tk.Entry(
            form, width=22, font=entry_font,
            bg="#daeaf5", fg="#1a3a50", insertbackground="#1a3a50",
            relief="flat", highlightbackground="#5AABCC", highlightthickness=2
        )
        self.entry_usuario.grid(row=0, column=1, pady=8)

        # Contraseña
        tk.Label(form, text="Nueva contraseña:", font=label_font,
                 bg="#B8D4E8", fg="#1a3a50", anchor="e").grid(
                 row=1, column=0, padx=(0, 10), pady=8, sticky="e")

        self.entry_password = tk.Entry(
            form, width=22, font=entry_font, show="*",
            bg="#daeaf5", fg="#1a3a50", insertbackground="#1a3a50",
            relief="flat", highlightbackground="#5AABCC", highlightthickness=2
        )
        self.entry_password.grid(row=1, column=1, pady=8)

        # --- Botones ---
        btn_frame = tk.Frame(card, bg="#B8D4E8")
        btn_frame.pack(pady=(4, 10))

        def make_btn(parent, text, cmd, secondary=False):
            bg = "#d6c9b8" if secondary else "#C9A97A"
            abg = "#c4b49e" if secondary else "#B8915F"
            return tk.Button(
                parent, text=text, font=button_font,
                width=13, command=cmd,
                bg=bg, fg="#3d2200",
                activebackground=abg, activeforeground="#3d2200",
                relief="flat", bd=0,
                highlightbackground="#9A7040", highlightthickness=2,
                cursor="hand2"
            )

        make_btn(btn_frame, "Registrarse", self.register).grid(row=0, column=0, padx=8)
        make_btn(btn_frame, "Volver", self.go_login, secondary=True).grid(row=0, column=1, padx=8)

        # --- Estado ---
        self.lbl_state = tk.Label(card, text="", font=status_font,
                                  bg="#B8D4E8", fg="#c0392b")
        self.lbl_state.pack(pady=(2, 20))

    # ---------- lógica (sin cambios) ----------

    def register(self):
        usuario  = self.entry_usuario.get().strip()
        password = self.entry_password.get().strip()

        if usuario == "" or password == "":
            self.lbl_state.config(text="Completa usuario y contraseña", fg="#c0392b")
            return

        if not self.master.ensure_connection():
            self.lbl_state.config(text="No se pudo conectar al servidor", fg="#c0392b")
            return

        self.master.send_server(f"REGISTER {usuario} {password}")
        self.lbl_state.config(text="Enviando registro...", fg="#185FA5")

    def handle_server_message(self, respuesta):
        mensajes = {
            "REGISTER_OK":          ("Registro exitoso", "#1a5c30"),
            "ERROR USER_EXISTS":    ("Ese usuario ya existe", "#c0392b"),
            "REGISTER_FAIL":        ("No se pudo registrar el usuario", "#c0392b"),
            "ERROR INVALID_FORMAT": ("Formato inválido", "#c0392b"),
            "ERROR INVALID_COMMAND":("Comando inválido", "#c0392b"),
        }
        if respuesta in mensajes:
            texto, color = mensajes[respuesta]
            self.lbl_state.config(text=texto, fg=color)
            if respuesta == "REGISTER_OK":
                self.after(1000, self.master.show_login)

    def go_login(self):
        self.master.show_login()