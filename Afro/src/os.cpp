#include <os.h>
#include <task.h>
#include <_boot.h>

#define OS_SW_FRQ 	(10000)

bool _inKernel = true;
int _taskCount = 0;

static Task* root = NULL;
static Task* prev = NULL;

extern "C" bool _isInKernel() {
	return _inKernel;
}

extern "C" bool _toggleKernel() {
	_inKernel = !_inKernel || _taskCount == 0;
	return _inKernel;
}

/*extern "C" void _CallTask(u32 funcPtr, u32 arg, u32 taskPtr) {
	((OS::Task*)taskPtr)->_SignalStarted();
	OS::taskFunction func = (OS::taskFunction)funcPtr;
	int retVal = func(arg);
	((OS::Task*)taskPtr)->_SignalReturned(retVal);
	//wait for OS to remove task
	while(1) SCB->ICSR = SCB_ICSR_PENDSVSET;
}*/

namespace OS {

	Task::Task(taskFunction taskStart, u32* taskStack, int stackLength) {
		this->next = NULL;
		this->stack = taskStack;
		this->stackLength = stackLength;
		this->running = false;
		this->taskStart = taskStart;
		this->errMsg = NULL;
	}

	void Task::SignalStarted() {
		this->running = false;
	}

	void Task::SignalReturned(int) {

	}

	void Task::SignalStopped() {

	}

	void Task::CallTaskStart(u32 funcPtr, u32 arg, u32 taskPtr) {
		((OS::Task*)taskPtr)->SignalStarted();
		OS::taskFunction func = (OS::taskFunction)funcPtr;
		int retVal = func(arg);
		((OS::Task*)taskPtr)->SignalReturned(retVal);
		//wait for OS to remove task
		while(1) SCB->ICSR = SCB_ICSR_PENDSVSET;
	}

	void Task::Start(u32 startArg) {
		if (this->isActive()) {
			OS::SetError(Task::ErrorTaskActive);
			return;
		}
		//todo add error handling (stack to small) and task structure update
		u32* sp_tmp = &this->stack[this->stackLength - 8 * 2];
		this->sp = sp_tmp;
		if (_taskCount == 0) __set_PSP((u32)this->sp);
		//increment sp by scratch register size
		sp_tmp = &sp_tmp[8];
		//r0 -> arg1
		sp_tmp[0] = (u32)this->taskStart;
		//r1 -> arg2
		sp_tmp[1] = startArg;
		//r2 -> arg3
		sp_tmp[2] = (u32)this;
		//pc -> current task position
		sp_tmp[6] = (u32)Task::CallTaskStart;
		//psr
		sp_tmp[7] = 0x21000000;
		_taskCount++;
		//todo stack filler to check for stack overflows (stacks should not exceed stack[4?]) <- pick reasonable limit

		//todo: add to task list
	}

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

	void SetError(err) {
		//todo
	}

	err GetError() {
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

	/*void _StartTask(Task* task) {//taskFunction func, u32 startArg, u32* stack, int stackSize) {
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
	}*/

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
