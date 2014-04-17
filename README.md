ARCap
=====

Augmented Reality Capture the Flag

The goal of this design project was to create an augmented-reality capture the flag game with two RC vehicles. Each vehicle contains an on-board Altera DE0 Nano FPGA board, an infrared emitter circuit using the LTE-4208, 4 infrared receiver circuits using the LTR-3208E, an XBee S6 Wi-Fi module, and a Polulu Low-Voltage Motor Controller. 7 AA batteries are used on each rover; four batteries power the DE0 Nano board, which in turn power the Wi-Fi module and the infrared circuitry, and three batteries power the motor controller and the motor. On-board components communicate with the players through a web server and through HTTP. The infrared module was interfaced to the Wi-Fi module and server to allow for the shooting of opponents. Users log onto a web server to control vehicle movement. Players will be able to control the speed of each track (right or left), and a skid steer mechanism will be used to control rover movement. The infrared circuitry, composed of 2 infrared receivers on each of the 4 sides, and 3 infrared emitters on the front of each rover, will allow for simulated ‘shooting’ of opponents. Overall the project was definite success, and almost all intended functionality was properly implemented in the final product.

For more details see the final report.