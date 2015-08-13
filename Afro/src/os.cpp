
#include <os.h>
#include <task.h>
#include <core_cmInstr.h>

#define MAIN_RETURN 0xfffffff9
#define THREAD_RETURN 0xfffffffd

#define OS_SW_FRQ 	(10000)

extern int kernel_main(void* args);

namespace OS {

	static void* ReadTaskStackPtr();
	static void SaveContext();
	static void LoadContext();
	static void WriteTaskStackPtr(void* sp);
	static task_link* CurrentTaskLink();
	static void AdvanceAndCleanupTask();
	static bool AtKernelTask();
	static void callKernel(void*);

	static task_link kernelTask;
	static Task::StackEntry kernelStack[2];
	static Task tkernel(kernelStack, 2);
	static task_link* currentTask = NULL;
	static task_link* lastTask = NULL;
	static u32 _time = 0;

	static volatile bool canSwitch = true;
	static volatile bool triedSwitch = true;
	static volatile bool rmTask = false;

	void Init() {
		__enable_irq();

		SysTick_Config(SystemCoreClock / OS_SW_FRQ);

		//kernelTask.task = &tkernel;
		//kernelTask.next = &kernelTask;
		//currentTask = &kernelTask;
		//lastTask = currentTask;
		tkernel.Start(callKernel, NULL);
		//TODO: add idle task
	}

	u32 SystemTicks() {
		return OS::_time;
	}

	static void callKernel(void* args) {
		kernel_main(args);
	}

	void SetError(err* errMsg) {
		OS::CurrentTask()->errMsg = errMsg;
	}

	err* GetError() {
		return OS::CurrentTask()->errMsg;
	}

	Task* CurrentTask() {
		return OS::CurrentTaskLink()->task;
	}

	void SwitchTask() {
		SCB->ICSR = SCB_ICSR_PENDSVSET;
	}

	void __addTask(task_link* task) {
		OS::__enterCritical();
		if (OS::currentTask == NULL) {
			OS::currentTask = task;
			OS::lastTask = task;
			task->next = task;
			callKernel(NULL);
		} else {
			task_link* next = OS::currentTask->next;
			OS::currentTask->next = task;
			task->next = next;
		}
		OS::__exitCritical();
	}

	void __endTask() {
		rmTask = true;
		OS::SwitchTask();
		while(true) {
			//error should never reach here
		}
	}

	void __enterCritical() {
		canSwitch = false;
	}

	void __exitCritical() {
		//canSwitch = true;
		if (triedSwitch) {
			triedSwitch = false;
			SCB->ICSR = SCB_ICSR_PENDSVSET;
		}
	}

	static void AdvanceAndCleanupTask() {
		OS::SaveContext();
		OS::CurrentTaskLink()->stackPointer = OS::ReadTaskStackPtr();
		if (rmTask) {
			OS::lastTask->next = OS::currentTask->next;
			OS::currentTask = OS::lastTask;
			rmTask = false;
		}
		OS::lastTask = OS::currentTask;
		OS::currentTask = OS::currentTask->next;
		OS::WriteTaskStackPtr(OS::CurrentTaskLink()->stackPointer);
		OS::LoadContext();
	}

	static task_link* CurrentTaskLink() {
		return OS::currentTask;
	}

	static bool AtKernelTask() {
		return OS::currentTask == &OS::kernelTask;
	}

	//Low level functions --------------------------------------------------------------

	/*	Read current stack pointer
	 * 		Used to store pointer when switching tasks
	 */
	static void* ReadTaskStackPtr() {
		void* ret;
		asm volatile ("MRS %0, psp"
				:"=r" (ret));
		return ret;
	}

	/*	Save registers to stack
	 * 		Saves current register values when switching tasks.
	 * 		Registers r0-r3 are ignored as these are backed up when the isr is called
	 * 		ISRs using more than r0-r3 could cause issues, PendSV MUST BE the LOWEST proirity interrupt to guarenty no registers are garbled on task switching
	 */
	static void SaveContext() {
		asm volatile (
				"MRS r0, psp;"
				"stmdb r0!, {r4-r11};"
				"MSR psp, r0;"
				:
				:
				: "%r0"
		);
	}

	/*	Restores registers from stack
	 * 		Loads previous register values from stack when switching tasks.
		 */
	static void LoadContext() {
		asm volatile (
				"MRS r0, psp;"
				"LDMFD r0!, {r4-r11};"
				"MSR psp, r0;"
				:
				:
				: "%r0"
		);
	}

	/*	Write current stack pointer
	 * 		Used to actually switch tasks
	 */
	static void WriteTaskStackPtr(void* sp) {
		asm volatile (
				"MSR psp, %0"
				:
				: "r" (sp)
		);
	}
}

//Interrupt Service Routines -------------------------------------------------------
/*	PendSV_isr
 * 		Primary OS interrupt service routine, handles all task switching
 * 		MUST be LOWEST priority interrupt to prevent register (r4-r11) garbling
 */

/*
extern "C" void SVC_Handler(void) {

}

extern "C" void PendSV_Handler(void) {
	if (OS::canSwitch) {
			OS::AdvanceAndCleanupTask();
			int ret = OS::AtKernelTask() ? MAIN_RETURN : THREAD_RETURN;
			asm volatile (
					"mov lr, %0;"
					//"bx lr"  this is a return statement
					:
					: "r" (ret)
			);
			return;
		} else {
			OS::triedSwitch = true;
		}
}
// */

/*	SysTick_isr
 * 		Handles counter tick for round robin scheduling and timer updates
 */
/*
extern "C" void SysTick_Handler(void) {
	//TODO: implementation (differed switching following voluntary switch)
	OS::_time += 1;
	OS::SwitchTask();
}
// */
