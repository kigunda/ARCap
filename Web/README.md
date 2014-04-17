# README - ARCap Web Software

This folder contains the software that runs on the server and on the client browsers. The server is a PHP socket server which accepts connections from rovers and from players. The rovers and the server exchange ASCII data, while the players and the server communicate using the WebSocket protocol which requires an initial handshake exchanging a security key, performed in ASCII, followed by exchanges of masked binary data. The server distinguishes plaintext TCP connections from WebSocket connections using the first message received: if the message starts with the connect prefix +, then the client is a rover TCP socket; if the message starts with GET, then the client is a player browser WebSocket.

The files for the server are found in */arcap/socket/*. The files for the user interface are found in */arcap/game/*.
