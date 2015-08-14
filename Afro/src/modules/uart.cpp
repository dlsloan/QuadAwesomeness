//enum UART_CONNECTION {AfroJack, GPS, XBee}; // The UART connections
#include <os.h>
#include <uart.h>
#include <task.h>
#include <stm32f10x_rcc.h>
#include "BlinkLed.h"


Uart Uart::uarts[NUMBER_OF_UARTS] = {Uart(UART1_BASE),Uart(UART2_BASE)};//,Uart(UART3_BASE),Uart(UART4_BASE),Uart(UART5_BASE)};

using namespace OS;

const err Uart::timeoutError = (err)"Uart read timed out";
const err Uart::inputBufferNull = (err)"Given input buffer dataBuffer is NULL";



// Here is the constructor. It will simply store the base address of the uart
Uart::Uart(u32 address) {
	uartBase = address;
	switch(uartBase) {
		case UART1_BASE:
			index = 0;
			break;
		case UART2_BASE:
			index = 1;
			break;
		case UART3_BASE:
			index = 2;
			break;
		case UART4_BASE:
			index = 3;
			break;
		case UART5_BASE:
			index = 4;
			break;

	}


}

void Uart::Init() {
	GPIO_InitTypeDef GPIO_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;


	switch(index) {
		case 0:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
			// Tx
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(GPIOA,&GPIO_InitStruct);

			// Rx
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(GPIOA,&GPIO_InitStruct);


			// Let's also enable the interrupts via the NVIC
			NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
			NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
			NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
			NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
			NVIC_Init(&NVIC_InitStruct);

			SetBaud(BAUD_RATE_USB);
			break;
		case 1:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
			// Tx
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(GPIOA,&GPIO_InitStruct);

			// Rx
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(GPIOA,&GPIO_InitStruct);


			// Let's also enable the interrupts via the NVIC
			NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
			NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
			NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
			NVIC_Init(&NVIC_InitStruct);

			SetBaud(BAUD_RATE_GPS);
			break;
		case 2:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
			// Tx
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(GPIOB,&GPIO_InitStruct);

			// Rx
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(GPIOB,&GPIO_InitStruct);

			// Let's also enable the interrupts via the NVIC
			NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
			NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
			NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
			NVIC_Init(&NVIC_InitStruct);
			break;





	}




	// This is where we will do the register initializations that are all based on the base_address.




	// Set the CR2 first
	UART_CR2(uartBase) = 0; // All default reset options are good

	// Set the CR1
	UART_CR1(uartBase) =	UART_CR1_UART_ENABLE | UART_CR1_WORD_LENGTH_8 | UART_CR1_WAKE_IDLE | UART_CR1_PARITY_DISABLE |
							UART_CR1_PARITY_SELECTION_EVEN | UART_CR1_PARITY_INTERRUPT_DISABLE | UART_CR1_TXEI_DISABLE |
							UART_CR1_TCI_DISABLE | UART_CR1_RXNEI_ENABLE | UART_CR1_IDLIE_DISABLE | UART_CR1_TE_ENABLE |
							UART_CR1_RE_ENABLE | UART_CR1_RWU_ACTIVE | UART_CR1_NO_BREAK_SEND;



	// For startup without interrupts
	/*
	UART_CR1(uartBase) =	UART_CR1_UART_ENABLE | UART_CR1_WORD_LENGTH_8 | UART_CR1_WAKE_IDLE | UART_CR1_PARITY_DISABLE |
							UART_CR1_PARITY_SELECTION_EVEN | UART_CR1_PARITY_INTERRUPT_DISABLE | UART_CR1_TXEI_DISABLE |
							UART_CR1_TCI_DISABLE | UART_CR1_RXNEI_DISABLE | UART_CR1_IDLIE_DISABLE | UART_CR1_TE_ENABLE |
							UART_CR1_RE_ENABLE | UART_CR1_RWU_ACTIVE | UART_CR1_NO_BREAK_SEND;*/







	uartTxWritePos = 0;
	uartTxReadPos = 0;
	uartRxWritePos = 0;
	uartRxReadPos = 0;
	uartRxOverflow = false;


}


// This function will get the correct uart connection based on the requested component (ie AfroJack, GPS, XBee).
// The purpose of this is to only create one software module for each hardware object (each uart connection)
//
// Inputs:
// UART_CONNECTION connection -> enumeration from the Uart class (ie Uart.AfroJack)
//
// Returns:
// A pointer to the specified uart.
Uart * Uart::GetUart(UART_CONNECTION connection) {
	Uart * pointer;

	switch(connection) {
	case USB:
		pointer = &uarts[connection];
		break;
	case GPS:
		pointer = &uarts[connection];
		break;
	default:
		pointer = NULL;
	}

	pointer->Init();
	return pointer;
}

Uart * Uart::GetUartIsrSafe(UART_CONNECTION connection) {
	Uart * pointer;
	GPIO_InitTypeDef GPIO_InitStruct;

	if(connection != GPS)
		return NULL;

	pointer = &uarts[connection];

	// Now if it was the GPS let's set up the port for output debugging without interrupts
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	// Tx
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	// Rx
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	// Now enable the UART

	// Set the initial baud rate
	pointer->SetBaud(BAUD_RATE_GPS);

	// Set the CR2 first
	UART_CR2(pointer->uartBase) = 0; // All default reset options are good

	// Set the CR1
	UART_CR1(pointer->uartBase) =	UART_CR1_UART_ENABLE | UART_CR1_WORD_LENGTH_8 | UART_CR1_WAKE_IDLE | UART_CR1_PARITY_DISABLE |
							UART_CR1_PARITY_SELECTION_EVEN | UART_CR1_PARITY_INTERRUPT_DISABLE | UART_CR1_TXEI_DISABLE |
							UART_CR1_TCI_DISABLE | UART_CR1_RXNEI_DISABLE | UART_CR1_IDLIE_DISABLE | UART_CR1_TE_ENABLE |
							UART_CR1_RE_ENABLE | UART_CR1_RWU_ACTIVE | UART_CR1_NO_BREAK_SEND;

	return pointer;

}

// This function will set the baud rate of the uart based on the baud number.
void Uart::SetBaud(int baudNumber) {
	// Baud rate = Periph clock / registerValue
	switch(this->index) {
	case 0:
		UART_BRR(uartBase) = UART_CLOCK2_FREQUENCY / baudNumber;
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		UART_BRR(uartBase) = UART_CLOCK1_FREQUENCY / baudNumber;
		break;
	}
}

//
void Uart::WriteBasic(u8 data) {
	// While the uart hardware buffer is full
	while(!(UART_SR(this->uartBase) & UART_SR_TRANSMIT_DATA_EMPTY_INTERRUPT));
	UART_DR(this->uartBase) = data;
}

void Uart::WriteIsrSafe(void * data, int nBytesToWrite) {

	u8 * dataTemp = (u8*)data;
	for(int i=0; i<nBytesToWrite; i++) {


		WriteBasic(dataTemp[i]);
	}
}

void Uart::WriteHex(u32 value, int digits, bool prefix) {
	if (prefix) {
		this->WriteBasic('0');
		this->WriteBasic('x');
	}
	for (int i = digits; i > 0; i--) {
		u8 val = (u8)((value >> ((i - 1) * 4)) & 0xF);
		if (val < 10) {
			this->WriteBasic((u8)(val + '0'));
		} else {
			this->WriteBasic((u8)(val - 10 + 'A'));
		}
	}
}

// This function will write the input data with the specified number of bytes to the uart. It may cause blocking if the uart buffer is full.
// Inputs:
// void * data -> A pointer to the data to be written to the uart.
// int nBytesToWrite -> The number of bytes to write to the Uart from the void * data buffer.
//
// Returns:
// Void
void Uart::WriteBytes(void * data, int nBytesToWrite) {

	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/

	writeLock.PendLock();


	// First let's cast the data into a usable byte format
	u8 * data_b = (u8*)data;

	// For each byte on the data, we need to check the uart register to see if it's empty
	// In order to make sure it sends in the right order, we need to send the end of the write buffer first
	for(int i=0; i<nBytesToWrite; i++) {

		// Get the last TxWritePosition in the buffer
		int pos = uartTxWritePos;

		// Check if we need to wrap around to the beginning of the buffer again
		int new_position = (pos == UART_BUFFER_SIZE - 1) ? 0 : pos + 1;

		// If the Uart has not taken any of the data yet do a task switch.
		// This would happen when we fill up the buffer completely and the hardware hasn't caught up yet.

		// NOTE THIS IS COMMENTED OUT BECAUSE TASKSWITCH DOESN'T EXIST YET
		/******************************************/
		/***************UNCOMMENT******************/
		/******************************************/
		while(uartTxReadPos == new_position) OS::SwitchTask();

		// Once the buffer is clear, we can write the data to the buffer
		uartBufferTx[pos] = data_b[i];

		// Now we just update the write position
		uartTxWritePos = new_position;

		// And now we can enable the interrupt for the TXE (transfer empty in the uart).
		// This will allow us to shift in more data via the interrupt service routine
		UART_CR1(uartBase) |= UART_CR1_TXEI_ENABLE;
	}



	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/
	writeLock.Release();




}

// This function will write the input data with the specified number of bytes to the uart. It may cause blocking if the uart buffer is full.
// Inputs:
// void * data -> A pointer to the data to be written to the uart.
// int nBytesToWrite -> The number of bytes to write to the Uart from the void * data buffer.
//
// Returns:
// Void
void Uart::WriteByte(u8 data) {

	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/

	writeLock.PendLock();




	// For each byte on the data, we need to check the uart register to see if it's empty
	// In order to make sure it sends in the right order, we need to send the end of the write buffer first

	// Get the last TxWritePosition in the buffer
	int pos = uartTxWritePos;

	// Check if we need to wrap around to the beginning of the buffer again
	int new_position = (pos == UART_BUFFER_SIZE - 1) ? 0 : pos + 1;

	// If the Uart has not taken any of the data yet do a task switch.
	// This would happen when we fill up the buffer completely and the hardware hasn't caught up yet.

	// NOTE THIS IS COMMENTED OUT BECAUSE TASKSWITCH DOESN'T EXIST YET
	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/
	while(uartTxReadPos == new_position) OS::SwitchTask();

	// Once the buffer is clear, we can write the data to the buffer
	uartBufferTx[pos] = data;

	// Now we just update the write position
	uartTxWritePos = new_position;

	// And now we can enable the interrupt for the TXE (transfer empty in the uart).
	// This will allow us to shift in more data via the interrupt service routine
	UART_CR1(uartBase) |= UART_CR1_TXEI_ENABLE;




	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/
	writeLock.Release();




}


// This function will attempt to write the number of specified bytes and data to the uart. If it does not all fit in the buffer, it will fail and not block.
// Inputs:
// void * data -> A pointer to the data to be written to the uart.
// int nBytesToWrite -> The number of bytes to write to the Uart from the void * data buffer.
//
// Returns:
// The number of bytes successfully written to the uart.
int Uart::WriteBytesNonBlocking(void * data, int nBytesToWrite) {

	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/

	writeLock.PendLock();

	// First let's cast the data into a usable byte format
	u8 * dataB = (u8*)data;
	int nBytesSuccessful = 0;

	// For each byte on the data, we need to check the uart register to see if it's empty
	for(int i=0; i<nBytesToWrite; i++) {

		// Get the last TxWritePosition in the buffer
		int pos = uartTxWritePos;

		// Check if we need to wrap around to the beginning of the buffer again
		int newPosition = (pos == UART_BUFFER_SIZE - 1) ? 0 : pos + 1;

		// If the Uart has not taken any of the data yet do a task switch.
		// This would happen when we fill up the buffer completely and the hardware hasn't caught up yet.

		// In this variation, if we aren't able to write to the buffer because it is full, we need to simply return
		// the number of bytes that were successfully written
		if(uartTxReadPos == newPosition) {
			/******************************************/
			/***************UNCOMMENT******************/
			/******************************************/
			// Call the operating system here to set the error
			SetError(timeoutError);

			break;
		}

		// Once the buffer is clear, we can write the data to the buffer
		uartBufferTx[pos] = dataB[i];

		// Now we just update the write position
		uartTxWritePos = newPosition;

		// Update the number of successful bytes written
		nBytesSuccessful++;

		// And now we can enable the interrupt for the TXE (transfer empty in the uart).
		// This will allow us to shift in more data via the interrupt service routine
		UART_CR1(uartBase) |= UART_CR1_TXEI_ENABLE;
	}

	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/
	writeLock.Release();

	return nBytesSuccessful;
}

// This function will read the number of bytes specified in the input parameter. It will block continu
// Inputs:
// void * dataBuffer -> a pointer to a preallocated data structure in which to read the data
// int nBytesToWaitAndRead -> The number of bytes to read from the Uart and store into the dataBuffer
// int timeout -> The maximum number of OS ticks to wait for the data before quitting the function. A timeout value of 0 will make it wait forever.
//
// Returns:
// The number of successful bytes written to the dataBuffer and read from the Uart
int Uart::ReadBytes(void * dataBuffer, int nBytesToWaitAndRead, int timeout) {

	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/
	readLock.PendLock();

	// Now get the starting tick point
	u32 startTick = SystemTicks();

	if(dataBuffer == NULL){
		// Write the error code to the operating system
		return 0;
	}

	u8 * dataB = (u8*)dataBuffer;
	int nBytesSuccessful = 0;

	for(int i=0; i<nBytesToWaitAndRead; i++) {
		// Get the current read position
		int pos = uartRxReadPos;

		// If there's nothing in the buffer to read, we will Task Switch
		while(pos == uartRxWritePos) {
			/******************************************/
			/***************UNCOMMENT******************/
			/******************************************/
			// Now we check the number of ticks and see the timeout needs to occur
			u32 currentTick =  SystemTicks();

			if(timeout != 0 && (int)(currentTick - startTick) >= timeout) {
				// Write the timeout error to the OS
				SetError(timeoutError);

				return nBytesSuccessful;
			}

			OS::SwitchTask();

		}

		// Once there is data, let's read it
		dataB[i] = uartBufferRx[pos++];

		// Now check if we need to roll over the position
		if(pos == UART_BUFFER_SIZE)
			pos = 0;

		nBytesSuccessful++;

		// Now update the read position
		uartRxReadPos = pos;

	}

	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/
	readLock.Release();

	return nBytesSuccessful;
}


// This function will attempt to read the number of bytes specified. If there is not any data to read from the Uart at any point in time, it will not block, it will simply return.
// Inputs:
// void * dataBuffer -> a pointer to a preallocated data structure in which to read the data
// int nBytesToWaitAndRead -> The number of bytes to read from the Uart and store into the dataBuffer
//
// Return:
// The number of bytes successfully read and stored in the dataBuffer.
int Uart::ReadBytesNonBlocking(void * dataBuffer, int nBytesToWaitAndRead) {


	if(dataBuffer == NULL){
		// Write the error code to the operating system
		SetError(inputBufferNull);

		return 0;
	}
	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/
	readLock.PendLock();

	u8 * dataB = (u8*) dataBuffer;
	int nBytesSuccessful = 0;

	for(int i=0; i<nBytesToWaitAndRead; i++) {
		// Get the current read position
		int pos = uartRxReadPos;

		// If there's nothing to read we need to exit
		if(pos == uartRxWritePos) {
			// Write the error to the os code

			return nBytesSuccessful;
		}

		// Once there is data, let's read it
		dataB[i] = uartBufferRx[pos++];

		// Now check if we need to roll over the position
		if(pos == UART_BUFFER_SIZE)
			pos = 0;

		nBytesSuccessful++;

		// Now update the read position
		uartRxReadPos = pos;

	}

	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/
	readLock.Release();

	return nBytesSuccessful;
}

// This function will read a byte from the Uart. It will block until the data is available or a timeout occurs.
// Inputs:
// int timeout -> The maximum number of OS ticks to wait for the data before quitting the function. A timeout value of 0 will make it wait forever.
//
// Returns:
// The data read from the Uart.
u8 Uart::ReadByte(int timeout) {
	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/
	readLock.PendLock();

	// Get the current read position
	int pos = uartRxReadPos;

	// Now get the starting tick point
	u32 startTick = 0;//GetTicks();

	// If there's nothing in the buffer to read, we will Task Switch
	while(pos == uartRxWritePos) {
		u32 currentTick = SystemTicks();

		if(timeout != 0 && (int)(currentTick - startTick) >= timeout) {
			// Write the error to the os
			OS::SetError(timeoutError);

			return 0;
		}

		OS::SwitchTask();
	}

	// Once there is data, let's read it
	u8 data = uartBufferRx[pos++];

	// Now check if we need to roll over the position
	if(pos == UART_BUFFER_SIZE)
		pos = 0;

	// Now update the read position
	uartRxReadPos = pos;

	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/
	readLock.Release();

	// Now return the data
	return data;

}

// This function will read a byte from the Uart. It will NOT block if there is no data available.
// Returns:
// The data read from the Uart. You must check the error code in the OS to see if it succeeded
u8 Uart::ReadByteNonBlocking() {

	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/
	readLock.PendLock();

	// Get the current read position
	int pos = uartRxReadPos;

	// If there's nothing in the buffer to read, we will Task Switch
	if(pos == uartRxWritePos){
		return 0;
	}

	// Once there is data, let's read it
	u8 data = uartBufferRx[pos++];

	// Now check if we need to roll over the position
	if(pos == UART_BUFFER_SIZE)
		pos = 0;

	// Now update the read position
	uartRxReadPos = pos;

	/******************************************/
	/***************UNCOMMENT******************/
	/******************************************/
	readLock.Release();

	// Now return the data
	return data;
}



inline void __attribute__((always_inline)) USART_IRQHandler(int uartNumber){

	u32 baseAddress = Uart::uarts[uartNumber].uartBase;

	// Let's see which interrupt it was
	// If the read data is not empty, that means we have data to read into the buffer
	if(UART_SR(baseAddress) & UART_SR_READ_DATA_NOT_EMPTY_INTERRUPT) {


		int pos = Uart::uarts[uartNumber].uartRxWritePos;
		Uart::uarts[uartNumber].uartBufferRx[pos++] = (u8)UART_DR(baseAddress);

		// If we reached the end of the buffer, wrap around
		if(pos == UART_BUFFER_SIZE)
			pos = 0;

		// Update the pointer
		Uart::uarts[uartNumber].uartRxWritePos = pos;
		if(pos == Uart::uarts[uartNumber].uartRxReadPos)
			Uart::uarts[uartNumber].uartRxOverflow = true;
	}

	// Let's see if the transmit data register empty interrupt is set as well
	// If it is, that means we can send more data
	if(UART_SR(baseAddress) & UART_SR_TRANSMIT_DATA_EMPTY_INTERRUPT) {
		int pos = Uart::uarts[uartNumber].uartTxReadPos;

		// If there is no data to write, just clear the bit
		if(pos == Uart::uarts[uartNumber].uartTxWritePos) {
			UART_CR1(baseAddress) &= ~UART_CR1_TXEI_ENABLE; // Turn off the interrupt
		}
		else {
			u8 data = Uart::uarts[uartNumber].uartBufferTx[pos++];
			UART_DR(baseAddress) = data;

			// Handle the buffer wrap around
			if(pos == UART_BUFFER_SIZE)
				pos = 0;

			// Update the pointers
			Uart::uarts[uartNumber].uartTxReadPos = pos;
		}
	}

}

// Interrupt Service Routine for the specific Uart
extern "C" void USART1_IRQHandler() {
	USART_IRQHandler(0);
}

extern "C" void USART2_IRQHandler() {
	USART_IRQHandler(1);
}

extern "C" void USART3_IRQHandler() {
	USART_IRQHandler(2);
}

extern "C" void UART4_IRQHandler() {
	USART_IRQHandler(3);
}

extern "C" void UART5_IRQHandler() {
	USART_IRQHandler(4);
}
