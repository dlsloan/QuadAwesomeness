#include <task.h>
#include <os.h>

namespace OS {

	Task::Task(taskFunction taskStart, u32* taskStack, int stackLength) {
		this->next = NULL;
		this->stack = taskStack;
		this->stackLength = stackLength;
		this->running = false;
		this->taskStart = taskStart;
	}

	void Task::Start(u32 startArg) {
		if (this->isActive()) {
			OS::SetError(Task::ErrorTaskActive);
			return;
		}
		_StartTask(this->taskStart, startArg, this->stack, this->stackLength);
	}
}
