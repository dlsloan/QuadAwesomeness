#include <os.h>
#include <_boot.h>

#define OS_SW_FRQ 	(10000)

bool _inKernel = true;
int _taskCount = 0;

extern "C" bool _isInKernel() {
	return _inKernel;
}

extern "C" bool _toggleKernel() {
	_inKernel = !_inKernel || _taskCount == 0;
	return _inKernel;
}

extern "C" void _CallTask(u32 funcPtr, u32 arg, u32 taskPtr) {
	//todo add task structure updates
	OS::taskFunction func = (OS::taskFunction)funcPtr;
	int retVal = func(arg);
	//loop until os removes task
	while(1) SCB->ICSR = SCB_ICSR_PENDSVSET;
}

namespace OS {
	static u32 _time;

	static void RunKernel();

	void Init() {
		__enable_irq();
		SysTick_Config(SystemCoreClock / OS_SW_FRQ);
		RunKernel();
	}

	inline void __attribute__((always_inline)) SwitchTask() {
		SCB->ICSR = SCB_ICSR_PENDSVSET;
	}

	void SetError(err*) {
		//todo
	}

	err* GetError() {
		return NULL;//todo
	}

	u32 SystemTicks() {
		return _time;
	}

	inline void __attribute__((always_inline)) __enterCritical() {
		__disable_irq();
	}
	inline void __attribute__((always_inline)) __exitCritical() {
		__enable_irq();
	}

	void _StartTask(taskFunction func, u32 startArg, u32* stack, int stackSize) {
		//todo add error handling (stack to small) and task structure update
		u32* sp = &stack[stackSize - 8 * 2];
		//increment sp by scratch register size
		sp = &sp[8];
		//r0 -> arg1
		sp[0] = (u32)func;
		//r1 -> arg2
		sp[1] = startArg;
		//r2 -> arg3
		sp[2] = (u32)NULL; //todo
		//pc -> current task position
		sp[6] = (u32)_CallTask;
		//psr
		sp[7] = 0x21000000;
		_taskCount ++;
		//todo stack filler to check for stack overflows (stacks should not exceed stack[4?]) <- pick reasonable limit
	}

	static void RunKernel() {
		while (true) {
			SwitchTask();
			//if stack checking is enabled scan stack and check psp for overflow
			_boot_load();
		}
	}
}

/*	SysTick_isr
 * 		Handles counter tick for round robin scheduling and timer updates
 */

extern "C" void SysTick_Handler(void) {
	//TODO: implementation (differed switching following voluntary switch)
	OS::_time += 1;
	OS::SwitchTask();
}
