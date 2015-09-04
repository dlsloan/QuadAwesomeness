#include <os_dbg.h>
#include <uart.h>
#include <string.h>
#include <core_cmInstr.h>
#include <core_cmFunc.h>

const char* startMessage = "Debugging OS Initialization...\r\n";

Uart* uart2;


//this is a place holder for now
typedef struct _taskNode {
	u32 stackPointer;
	struct _taskNode* next;

} taskNode;

typedef int (*taskStartFunc)(u32);

bool inKernel = true;
int taskCount = 0;
#define TestStackSize (50)
u32 testStack[TestStackSize];
taskNode testTask;

extern "C" bool toggleKernel() {
	//return true;
	inKernel = taskCount == 0 || !inKernel;
	return inKernel;
}

extern "C" bool isInKernel() {
	//return true;
	return inKernel;
}

void Print(const char* str) {
	uart2->WriteIsrSafe((u8*)str, strlen(str));
}

extern "C" void WriteHex32(u32 value) {
	uart2->WriteHex(value, 8, true);
}

extern "C" void WriteLine(void) {
	uart2->WriteIsrSafe((u8*)"\r\n", 2);
}

extern "C" void CallTask(u32 funcPtr, u32 arg, u32 taskPtr) {
	Print("Task Started\r\nArgs\r\n");
	WriteHex32(funcPtr);
	WriteLine();
	WriteHex32(arg);
	WriteLine();
	WriteHex32(taskPtr);
	WriteLine();
	WriteLine();
	taskStartFunc func = (taskStartFunc)funcPtr;
	int retVal = func(arg);
	Print("Task Ended");
	WriteHex32(retVal);
	WriteLine();
	while(1) SCB->ICSR = SCB_ICSR_PENDSVSET;
}

int Test(u32 arg) {
	int i = 10;
	while(i--) {
		Print("Task 2\r\nArg:");
		WriteHex32(arg);
		WriteLine();
		volatile int cnt = 1000000;
		while(cnt--) SCB->ICSR = SCB_ICSR_PENDSVSET;
	}
	return -1;
}

void StartTask(taskNode* task, taskStartFunc func, u32 startArg) {
	u32* sp = (u32*)task->stackPointer;
	__set_PSP(task->stackPointer);
	//increment sp by scratch register size
	sp = &sp[8];
	//r0 -> arg1
	sp[0] = (u32)func;
	//r1 -> arg2
	sp[1] = startArg;
	//r2 -> arg3
	sp[2] = (u32)task;
	//pc -> current task position
	sp[6] = (u32)CallTask;
	//psr
	sp[7] = 0x21000000;
	taskCount ++;

	Print("SP");
	WriteHex32((u32)sp);
	WriteLine();
	WriteHex32((u32)__get_PSP());
	WriteLine();
	Print("Expected Task Register State\r\n");
	for (int i = 0; i < 8; i++) {
		WriteHex32(sp[i]);
		WriteLine();
	}
	WriteLine();
}

void os_dbg::Init() {
	uart2 = Uart::GetUartIsrSafe(Uart::GPS);
	uart2->WriteIsrSafe((u8*)startMessage, strlen(startMessage));

	Print("Kernel only task switch...\r\n");
	SCB->ICSR = SCB_ICSR_PENDSVSET;

	Print("Starting SysTick...\r\n");
	//SysTick_Config(SystemCoreClock / OS_SW_FRQ);
	SysTick_Config(SystemCoreClock / 100000);

	Print("Setting up task...\r\n");
	testTask.stackPointer = (u32)&testStack[TestStackSize - 8 * 2];
	StartTask(&testTask, Test, 0);

	Print("Task switching...\r\n");
	SCB->ICSR = SCB_ICSR_PENDSVSET;
	Print("Back to kernel\r\n");
}

extern "C" void SysTick_Handler(void) {
	SCB->ICSR = SCB_ICSR_PENDSVSET;
}
