#ifndef _os_h
#define _os_h

#include <types.h>
#include <task.h>

namespace OS {

	typedef int (*taskFunction)(u32);

	void Init();

	void SetError(err*);
	err* GetError();

	//Task* CurrentTask();

	void SwitchTask();

	u32 SystemTicks();

	//void __addTask(task_link*);
	//void __endTask();
	void __enterCritical();
	void __exitCritical();
}
#endif
