from network import TCPClient

def main():
    client = TCPClient()

    if not client.connect():
        return

    try:
        usuario = input("Usuario: ").strip()
        password = input("Password: ").strip()
        client.send_message(f"LOGIN {usuario} {password}")

        while True:
            respuesta = client.receive_message()
            if respuesta is None:
                break

            print(f"Respuesta del servidor: {respuesta}")

            if respuesta == "LOGIN_OK":
                continue

            elif respuesta == "LOGIN_FAIL":
                print("Credenciales incorrectas.")
                usuario = input("Usuario: ").strip()
                password = input("Password: ").strip()
                client.send_message(f"LOGIN {usuario} {password}")

            elif respuesta == "REGISTER_OK":
                print("Usuario registrado correctamente.")

            elif respuesta == "REGISTER_FAIL":
                print("No se pudo registrar el usuario.")

            elif respuesta == "ERROR USER_EXISTS":
                print("Ese usuario ya existe.")

            elif respuesta == "ERROR INVALID_FORMAT":
                print("Formato inválido.")

            elif respuesta == "ERROR INVALID_COMMAND":
                print("Comando inválido.")

            elif respuesta == "ERROR INVALID_WORD":
                print("Palabra inválida. Debe tener 5 letras, solo letras y sin repetidas.")

            elif respuesta == "ERROR AUTH_REQUIRED":
                print("Debes autenticarte antes.")

            elif respuesta == "ERROR NOT_YOUR_TURN":
                print("No es tu turno.")

            elif respuesta == "WAITING_PLAYER":
                print("Esperando al otro jugador...")

            elif respuesta == "START_GAME":
                print("La partida ha comenzado.")

            elif respuesta == "YOUR_TURN_SETWORD":
                palabra = input("Es tu turno de poner palabra: ").strip()
                client.send_message(f"SETWORD {palabra}")

            elif respuesta == "YOUR_TURN_GUESS":
                intento = input("Es tu turno de adivinar: ").strip()
                client.send_message(f"GUESS {intento}")

            elif respuesta.startswith("RESULT"):
                print(f"Resultado recibido: {respuesta}")

            elif respuesta == "ROUND_END":
                print("La ronda terminó.")

            elif respuesta == "GAME_OVER BOTH":
                print("Resultado final: ambos jugadores acertaron.")
                break

            elif respuesta == "GAME_OVER J1":
                print("Resultado final: solo el jugador 1 acertó.")
                break

            elif respuesta == "GAME_OVER J2":
                print("Resultado final: solo el jugador 2 acertó.")
                break

            elif respuesta == "GAME_OVER NONE":
                print("Resultado final: ningún jugador acertó.")
                break

            elif respuesta == "OPPONENT_DISCONNECTED":
                print("El oponente se desconectó.")
                break

            else:
                print("Mensaje no manejado por el cliente.")

    except KeyboardInterrupt:
        client.send_message("DISCONNECT")

    finally:
        client.close()

if __name__ == "__main__":
    main()