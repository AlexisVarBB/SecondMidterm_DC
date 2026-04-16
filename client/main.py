from network import TCPClient

def main():
    client = TCPClient()
    if not client.connect():
        return

    try:
      mensaje_servidor = client.receive_message()
      if mensaje_servidor is not None:
            print(f"Mensaje del servidor: {mensaje_servidor}")

            mensaje = input("Ingrese un mensaje para enviar al servidor (o 'salir' para terminar): ")
            client.send_message(mensaje)

            respuesta = client.receive_message()
            if respuesta is None:
                 print(f"Respuesta del servidor: {respuesta}")
    finally:
        client.close()

if __name__ == "__main__":    main()