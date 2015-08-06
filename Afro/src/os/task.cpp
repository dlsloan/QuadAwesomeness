#include <task.h>
#include <os.h>

namespace OS {
	typedef struct {
	  u32 r0;
	  u32 r1;
	  u32 r2;
	  u32 r3;
	  u32 r12;
	  u32 lr;
	  u32 pc;
	  u32 psr;
	} hw_stack_frame;

	typedef struct {
	  u32 r4;
	  u32 r5;
	  u32 r6;
	  u32 r7;
	  u32 r8;
	  u32 r9;
	  u32 r10;
	  u32 r11;
	} sw_stack_frame;

	static void _TaskCleanUp() {
		OS::CurrentTask()->_End();
		OS::__endTask();
	}

	void Task::_End() {
		this->running = false;
	}

	Task::Task(StackEntry* taskStack, int stackLength) {
		this->errMsg = NULL;
		this->stack = (void*)taskStack;
		this->stackSize = stackLength * sizeof(StackEntry);
		this->running = false;
	}

	void Task::Start(taskFunc entryFunc, void* startParam) {
		this->link.task = this;
		this->link.stackPointer = (void*)((u32)this->stack + this->stackSize - sizeof(hw_stack_frame) - sizeof(sw_stack_frame));
		hw_stack_frame* frame = (hw_stack_frame*)((u32)this->stack + this->stackSize - sizeof(hw_stack_frame));
		frame->r0 = (u32)startParam;
		frame->r1 = 0;
		frame->r2 = 0;
		frame->r3 = 0;
		frame->r12 = 0;
		frame->pc = (u32)entryFunc;
		frame->lr = (u32)_TaskCleanUp;
		frame->psr = 0x21000000;
		OS::__addTask(&this->link);
		this->running = true;
	}

	void Task::Switch() {
		OS::SwitchTask();
	}
}
