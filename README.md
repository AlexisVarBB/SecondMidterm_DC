# SecondMidterm_DC

## Project Description

This project consists of a **multiplayer Wordle-style game** developed using a **client-server architecture**. The system allows two players to connect to a central server, authenticate themselves, and play a turn-based word guessing game.

The game follows a structured two-round format:

- The game supports **two players** connected to the server.
- Each player must log in using a username and password before participating.
- The game consists of **two rounds**:
  
  **Round 1:**  
  Player 1 sets a secret word consisting of **5 letters**, and Player 2 attempts to guess the word within a maximum of **6 attempts**.

  **Round 2:**  
  Player 2 sets a new secret word, and Player 1 attempts to guess it under the same conditions.

- After each guess, the server evaluates the word and sends feedback indicating:
  - Correct letters in the correct position
  - Correct letters in the wrong position
  - Letters that are not part of the word

- At the end of both rounds, the server displays the final result of the match, which can be:
  - Both players guessed correctly
  - Only Player 1 guessed correctly
  - Only Player 2 guessed correctly
  - Neither player guessed correctly

The server coordinates all communication between players, validates inputs, manages game rounds, and ensures proper synchronization between clients.

This project demonstrates the implementation of **distributed systems**, **network communication**, **client-server architecture**, and **graphical user interface development**.

---

## Programming Languages Used

The project will be developed using the following programming languages:

### C Language (Server Side)

The server will be implemented in **C**, as required by the project specifications. The server will be responsible for:

- Managing TCP socket communication
- Accepting multiple client connections
- Handling concurrent processes using `fork()`
- Implementing the authentication mechanism
- Managing game logic and round control
- Validating words and guesses
- Sending responses and results to connected clients

C is used because it provides low-level control over sockets and process management, making it suitable for network-based applications.

---

### Python (Client Side)

The client application will be implemented in **Python**, using socket communication to connect to the C server. Python will also be used to build the **graphical user interface (GUI)**.

The client will be responsible for:

- Connecting to the server using TCP sockets
- Sending login credentials
- Sending words and guesses to the server
- Receiving responses from the server
- Displaying the game interface
- Showing visual feedback for each guess
- Managing user interaction

Python is used because it simplifies graphical interface development and allows faster implementation of user interaction features.

---

## System Architecture Overview

The project follows a **client-server architecture**, where:

- A **central server (C)** controls the game logic and communication.
- Two **client applications (Python)** connect to the server.
- All actions performed by players are sent to the server.
- The server processes requests and sends responses back to clients.
