/* Interrupt service routine triggered whenever the status of the IR emitter pushbutton changes. */
static void isr_on_ir_pushbutton( void * context) {
	// Read the state of the pushbutton and post it to the queue.
	int state = IR_PUSHBUTTON_MESSAGE + IORD_ALTERA_AVALON_PIO_DATA(IR_PUSHBUTTON_BASE);
	OSQPost(queue, (void*)state);
	// Mask to mark the end of the ISR	
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(IR_PUSHBUTTON_BASE, IR_PUSHBUTTON_BIT_CLEARING_EDGE_REGISTER);
}

/* Controllers the IR emitter based on the value of the pushbutton. */
void ir_task(void* pdata)
{
	INT8U err;
	while (1)
	{
		char message[LCD_MESSAGE_SIZE];
		int status = (int)OSQPend(queue, WAIT_FOREVER, &err) - IR_PUSHBUTTON_MESSAGE;
		if (err == OS_NO_ERR) {
			IOWR_ALTERA_AVALON_PIO_DATA(IR_EMITTER_BASE, status);
		}
	}
}