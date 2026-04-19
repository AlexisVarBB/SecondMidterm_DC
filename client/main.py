from network import TCPClient

def main():
    client = TCPClient()

    if not client.connect():
        return

    try:
        while True:
            mensaje = input("Ingrese un mensaje para enviar al servidor: ")

            if mensaje.lower() == "salir":
                mensaje = "DISCONNECT"
                client.send_message(mensaje)
                break

            client.send_message(mensaje)

            while True:
                respuesta = client.receive_message()
                if respuesta is None:
                    break

                print(f"Respuesta del servidor: {respuesta}")

                if respuesta in [
    "LOGIN_FAIL",
    "REGISTER_OK",
    "REGISTER_FAIL",
    "ERROR USER_EXISTS",
    "ERROR INVALID_FORMAT",
    "ERROR INVALID_COMMAND",
    "WAITING_PLAYER",
    "START_GAME",
    "OPPONENT_DISCONNECTED"
]:
                    break

    finally:
        client.close()

if __name__ == "__main__":
    main()