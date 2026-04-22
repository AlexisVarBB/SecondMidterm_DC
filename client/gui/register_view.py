import tkinter as tk


class RegisterView(tk.Frame):
    def __init__(self, master):
        super().__init__(master)
        self.master = master
        self.build_ui()

    def build_ui(self):
        titulo = tk.Label(self, text="REGISTER", font=("Arial", 20, "bold"))
        titulo.pack(pady=30)

        form_frame = tk.Frame(self)
        form_frame.pack(pady=20)

        lbl_usuario = tk.Label(form_frame, text="Nuevo usuario:", font=("Arial", 12))
        lbl_usuario.grid(row=0, column=0, padx=10, pady=10, sticky="e")

        self.entry_usuario = tk.Entry(form_frame, width=25, font=("Arial", 12))
        self.entry_usuario.grid(row=0, column=1, padx=10, pady=10)

        lbl_password = tk.Label(form_frame, text="Nueva contraseña:", font=("Arial", 12))
        lbl_password.grid(row=1, column=0, padx=10, pady=10, sticky="e")

        self.entry_password = tk.Entry(form_frame, width=25, font=("Arial", 12), show="*")
        self.entry_password.grid(row=1, column=1, padx=10, pady=10)

        botones_frame = tk.Frame(self)
        botones_frame.pack(pady=20)

        btn_register = tk.Button(botones_frame, text="Registrarse", width=15, command=self.register)
        btn_register.grid(row=0, column=0, padx=10)

        btn_back = tk.Button(botones_frame, text="Volver", width=15, command=self.go_login)
        btn_back.grid(row=0, column=1, padx=10)

        self.lbl_state = tk.Label(self, text="", font=("Arial", 11), fg="red")
        self.lbl_state.pack(pady=10)

    def register(self):
        usuario =  self.entry_usuario.get().strip()
        password = self.entry_password.get().strip()

        if usuario == "" or password == "":
            self.lbl_state.config(text="Completa usuario y contraseña")
            return
        
        if not self.master.ensure_connection():
            self.lbl_state.config(text="No se pudo conectar al servidor")
            return
        
        mensaje = f"REGISTER {usuario} {password}"
        self.master.send_server(mensaje)
        self.lbl_state.config(text="Enviando registro...", fg="blue")


    def handle_server_message(self, respuesta):
        if respuesta == "REGISTER_OK":
            self.lbl_state.config(text="Registro exitoso", fg="green")
            self.after(1000, self.master.show_login)
        elif respuesta == "ERROR USER_EXISTS":
            self.lbl_state.config(text="Ese usuario ya existe", fg="red")
        elif respuesta == "REGISTER_FAIL":
            self.lbl_state.config(text="No se pudo registrar el usuario", fg="red")
        elif respuesta == "ERROR INVALID_FORMAT":
            self.lbl_state.config(text="Formato inválido", fg="red")
        elif respuesta == "ERROR INVALID_COMMAND":
            self.lbl_state.config(text="Comando inválido", fg="red")


        ##self.lbl_state.config(text=f"Registro pendiente para: {usuario}")

    def go_login(self):
        self.master.show_login()