import tkinter as tk
from tkinter import messagebox

class LoginView(tk.Frame):
    def __init__(self, master):
        super().__init__(master)
        self.master = master
        self.build_ui()
    
    def build_ui(self):
        top_frame = tk.Frame(self)
        top_frame.pack(fill="x", pady=10, padx=10)
        
        btn_about = tk.Button(
            top_frame,
            text="About",
            width=10,
            command=self.mostrar_acerca_de
        )
        btn_about.pack(side="right")
        
        
        titulo = tk.Label(self, text="LOGIN", font=("Arial", 20, "bold"))
        titulo.pack(pady=30)

        form_frame = tk.Frame(self)
        form_frame.pack(pady=20)

        lbl_usuario = tk.Label(form_frame, text="Usuario:", font=("Arial", 12))
        lbl_usuario.grid(row=0, column=0, padx=10, pady=10, sticky="e")

        self.entry_usuario = tk.Entry(form_frame, width=25, font=("Arial", 12))
        self.entry_usuario.grid(row=0, column=1, padx=10, pady=10)

        lbl_password = tk.Label(form_frame, text="Contraseña:", font=("Arial", 12))
        lbl_password.grid(row=1, column=0, padx=10, pady=10, sticky="e")

        self.entry_password = tk.Entry(form_frame, width=25, font=("Arial", 12), show="*")
        self.entry_password.grid(row=1, column=1, padx=10, pady=10)

        botones_frame = tk.Frame(self)
        botones_frame.pack(pady=20)

        btn_login = tk.Button(botones_frame, text="Login", font=("Arial", 12), width=15, command=self.login)
        btn_login.grid(row=0, column=0, padx=10)

        btn_register = tk.Button(botones_frame, text="Register", font=("Arial", 12), width=15, command=self.go_register)
        btn_register.grid(row=0, column=1, padx=10)

        self.lbl_state = tk.Label(self, text="", font=("Arial", 12), fg="red")
        self.lbl_state.pack(pady=10)
        
    def mostrar_acerca_de(self):
        texto = (
            "Team members:\n"
            "Alberto Stephen Dubin Hernandez\n"
            "Jorge Enrique Ruiz Liera\n"
            "Alexis Vargas Moreno\n\n"
            "Course: Distributed Computing\n\n"
            "Professor: Dr. Juan Carlos López Pimentel\n\n"
            "How to play and how to win:\n"
            "Two players participate. In round 1, Player 1 enters a secret 5-letter word "
            "and Player 2 has up to 6 attempts to guess it. In round 2, roles switch: "
            "Player 2 enters a secret word and Player 1 guesses. After each guess, the "
            "server returns feedback for each letter. Green means correct letter in the "
            "correct position, yellow means the letter exists but is in a different position, "
            "and gray means the letter is not in the word. At the end, the game shows whether "
            "both players guessed correctly, only one did, or neither did."
        )

        messagebox.showinfo("About", texto)

    def login(self):
        usuario = self.entry_usuario.get().strip()
        password = self.entry_password.get().strip()

        if usuario == "" or password == "":
            self.lbl_state.config(text="Completa usuario y contraseña")
            return

        if not self.master.ensure_connection():
            self.lbl_state.config(text="No se pudo conectar al servidor")
            return

        mensaje = f"LOGIN {usuario} {password}"
        self.master.send_server(mensaje)

        self.lbl_state.config(text="Enviando login...", fg="blue")

    def handle_server_message(self, respuesta):
        if respuesta == "LOGIN_OK":
            self.lbl_state.config(text="Login exitoso", fg="green")
        elif respuesta == "LOGIN_FAIL":
            self.lbl_state.config(text="Credenciales incorrectas", fg="red")
        elif respuesta == "ERROR INVALID_FORMAT":
            self.lbl_state.config(text="Formato inválido", fg="red")
        elif respuesta == "ERROR INVALID_COMMAND":
            self.lbl_state.config(text="Comando inválido", fg="red")

    def go_register(self):
        self.master.show_register()
