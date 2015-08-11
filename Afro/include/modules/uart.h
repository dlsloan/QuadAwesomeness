#ifndef uart_h
#define uart_h

#include "types.h"
#include "mutex.h"
#include "os.h"

#define UART_CLOCK1_FREQUENCY (36000000)
#define UART_CLOCK2_FREQUENCY (72000000)
#define UART_BUFFER_SIZE 512

//********** UART MEMORY BASE ADDRESSES ***********
#define UART1_BASE 0x40013800
#define UART2_BASE 0x40004400
#define UART3_BASE 0x40004800
//#define UART4_BASE 0x40004C00
//#define UART5_BASE 0X40005000

#define UART1_INTERRUPT_ADDRESS 0x000000D4
#define UART2_INTERRUPT_ADDRESS 0x000000D8
#define UART3_INTERRUPT_ADDRESS 0x000000DC
#define UART4_INTERRUPT_ADDRESS 0x00000110
#define UART5_INTERRUPT_ADDRESS 0x00000114

//*********** UART REGISTERS ****************
#define UART_SR(UART_BASE) (*(volatile u32*)(UART_BASE + 0x00))
#define UART_DR(UART_BASE) (*(volatile u32*)(UART_BASE + 0x04))
#define UART_BRR(UART_BASE) (*(volatile u32*)(UART_BASE + 0x08))
#define UART_CR1(UART_BASE) (*(volatile u32*)(UART_BASE + 0x0C))
#define UART_CR2(UART_BASE) (*(volatile u32*)(UART_BASE + 0x10))
#define UART_CR3(UART_BASE) (*(volatile u32*)(UART_BASE + 0x14))
#define UART_GTPR(UART_BASE) (*(volatile u32*)(UART_BASE + 0x18))

//************ UART Configuration Register 1 ***********
#define UART_CR1_UART_ENABLE 0x00002000
#define UART_CR1_WORD_LENGTH_8 0x0
#define UART_CR1_WAKE_IDLE 0x0
#define UART_CR1_PARITY_DISABLE 0x0
#define UART_CR1_PARITY_SELECTION_EVEN 0x0
#define UART_CR1_PARITY_INTERRUPT_DISABLE 0x0
#define UART_CR1_TXEI_ENABLE  0x00000080
#define UART_CR1_TXEI_DISABLE 0x0
#define UART_CR1_TCI_DISABLE 0x0
#define UART_CR1_RXNEI_ENABLE 0x00000020
#define UART_CR1_RXNEI_DISABLE 0x0
#define UART_CR1_IDLIE_DISABLE 0x0
#define UART_CR1_TE_ENABLE 0x00000008
#define UART_CR1_RE_ENABLE 0x00000004
#define UART_CR1_RWU_ACTIVE 0x0
#define UART_CR1_NO_BREAK_SEND 0x0

//************ UART Configuration Register 1 ***********
#define UART_CR2_LIN_DISABLE 0x0
#define UART_CR2_STOP_1_BIT 0x0
#define UART_CR2_CLK_DISABLE 0x0
#define UART_CR2_CLK_POLARITY_LOW 0x0
#define UART_CR2_CLK_PHASE_FIRST 0x0

//************ UART Status Register ***********
#define UART_SR_READ_DATA_NOT_EMPTY_INTERRUPT 0x00000020
#define UART_SR_TRANSMIT_DATA_EMPTY_INTERRUPT 0X00000080


#define NUMBER_OF_UARTS 2
#define BAUD_RATE_USB 115200
#define BAUD_RATE_GPS 115200
class Uart {
public:
	enum UART_CONNECTION {USB = 0, GPS = 1}; // The UART connections

	static Uart * GetUart(UART_CONNECTION connection); // Statically get one of the pre-allocated UART objects based on the enumeration
	static Uart * GetUartIsrSafe(UART_CONNECTION connection);
	void SetBaud(int baudNumber); // Set the baud rate of the UART

	void WriteIsrSafe(void * data, int nBytesToWrite); // Writes the data to the UART buffer and may block/cause a task switch until all of the data is written.
	void WriteHex(u32 value, int digits, bool prefix);

	void WriteBasic(u8 data);

	void WriteBytes(void * data, int nBytesToWrite); // Writes the data to the UART buffer and may block/cause a task switch until all of the data is written.
	int WriteBytesNonBlocking(void * data, int nBytesToWrite); // Writes the data to the UART buffer without blocking or task switching. It returns the number of bytes that were successfully written

	void WriteByte(u8 data); // Writes a byte to the UART buffer. If there is no space, it will block until the data is written
	void WriteByteNonBlocking(u8 data); // Attempts to write a byte to the UART buffer without blocking. You must check the error code to determine if the byte was successfully written

	int ReadBytes(void * dataBuffer, int nBytesToWaitAndRead, int timeout); // Reads data from the UART. It will store the number of bytes read into the n_bytes variable. If there is no data, it will block until there is.
	int ReadBytesNonBlocking(void * dataBuffer, int nBytesToWaitAndRead);

	u8 ReadByte(int timeout); // Reads a byte from the UART. It will block if there is no byte available.
	u8 ReadByteNonBlocking(); // Reads a byte from the UART without blocking. You must read the error code to determine if it succeeded


	// Define all of the buffers and signals
	volatile u8 uartBufferTx[UART_BUFFER_SIZE];
	volatile u8 uartBufferRx[UART_BUFFER_SIZE];
	volatile int uartTxWritePos;
	volatile int uartRxWritePos;
	volatile int uartTxReadPos;
	volatile int uartRxReadPos;
	volatile bool uartRxOverflow;

	static Uart uarts[NUMBER_OF_UARTS]; // The array of all the pre-allocated UART objects
	u32 uartBase; // The base address of the UART
	u8 index;

	static const err timeoutError;
	static const err inputBufferNull;


private:
	OS::Mutex writeLock;
	OS::Mutex readLock;



	Uart(u32 address); // Constructor containing the base address for the UART.
	void Init(); // Initialize the uart

	// Now we put the new/new[]/delete/delete[] operators here so they are private
	//void * operator new (u32);
	//void * operator new[] (size_t);
	void operator delete (void *);
	void operator delete[] (void *);





protected:

};
#endif
