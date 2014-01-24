ARCap
=====

Augement Reality Capture the Flag

The goal of this design project is to create an augmented-reality capture the flag game with two RC vehicles. Each vehicle will contain an on-board Altera DE0 Nano FPGA board, a CMOS camera module, and a Wi-Fi module. HTTP will be used for players to communicate with a server, which will facilitate communication with vehicles through Wi-Fi. The camera module will be interface to the Wi-Fi module and server to allow for simulated flag capture, hitting targets, and shooting opponents. Images will be pre-processed on the DE0 board, and then sent to the server through the Wi-Fi interface for further processing.
