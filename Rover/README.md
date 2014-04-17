# README - ARCap Rover Software

This folder contains the software that runs on the rovers. There are a total of seven tasks, which communicate through queues. Each of the tasks is managed through a C++ singleton class, i.e. there is one object instantiated for each class. In general each class has an update task, with the exception of the *NetworkReceiver* class which has both an update and a connect task. There are two categories of classes: 
	* the datasource classes *InfraredReceiver* and *NetworkReceiver* poll periodically to check for updates
	* the listener classes *InfraredSender*, *MotorHandler*, and *NetworkReceiver* wait on queues for new events from datasource classes and process them as they arrive.

Class 				| Description
-------------------	| ----------------
InfraredReceiver 	| Reads input from the infrared receivers through the analog-to-digital converter (ADC) and posts infrared hit events to a listener.
InfraredSender 		| Listens for send commands on a queue and applies them to the infrared emitters.
Listener 			| Common base for classes that listen to data sources.
MotorHandler 		| Listens for commands on a queue and uses them to control the motor. The command must specify: (a) which motor to run {l(eft), r(ight)}, (b) the direction to run the motor in {(f)orward, (b)ackward}, (c) the speed to run the motor at [0 to 127].
NetworkReceiver 	| Listens for messages from the server using a wifi handler and forwards them to handlers that execute the corresponding effects on the rover.
NetworkSender 		| Listens for events from handlers on the rover and forwards them to the server using a wifi handler.
WifiHandler 		| Provides a high-level view of the interfacing to the Xbee wifi module.

File 				| Description
------------------	| ----------------
Status.h			| Contains status info for the project, such as classes of exceptions that may occur.
Tasks.h				| Contains definitions of task stack sizes and priorities.
Messages.h			| Defines the protocol used for message between tasks and between the rover and server.