import socket
from config import HOST, PORT, BUFFER_SIZE

class TCPClient:
    def __init__(self):
        self.sock = None

    def connect(self):
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect((HOST, PORT))
            print(f"Conectado al servidor en {HOST}:{PORT}")
            return True
        except Exception as e:
            print(f"Error al conectar al servidor: {e}")
            self.sock = None
            return False

    def send_message(self, message):
        try:
            if self.sock is not None:
                self.sock.sendall(message.encode("utf-8"))
            else:
                print("No hay conexión activa con el servidor")
        except Exception as e:
            print(f"Error al enviar mensaje: {e}")

    def receive_message(self):
        try:
            if self.sock is not None:
                datos = self.sock.recv(BUFFER_SIZE)
                if not datos:
                    print("El servidor cerró la conexión")
                    return None
                return datos.decode("utf-8")
            else:
                print("No hay conexión activa con el servidor")
                return None
        except Exception as e:
            print(f"Error al recibir mensaje: {e}")
            return None

    def close(self):
        try:
            if self.sock is not None:
                self.sock.close()
                self.sock = None
                print("Conexión cerrada")
        except Exception as e:
            print(f"Error al cerrar conexión: {e}")