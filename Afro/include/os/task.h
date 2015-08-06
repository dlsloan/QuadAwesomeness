#ifndef _task_h
#define _task_h

#include <types.h>

namespace OS {
	class Task;

	typedef struct _task_link {
	  Task* task;
	  void* stackPointer;
	  struct _task_link* next;
	} task_link;


	class Task {
	public:
		typedef struct {
			u8 entry[32];
		} StackEntry;

		typedef void (*taskFunc)(void*);

		Task(StackEntry* stack, int stackLength);

		void Start(taskFunc entryFunc, void* startParam);

		void _End();

		static void Switch();

		err* errMsg;
	private:
		void* stack;
		int stackSize; // bytes
		task_link link;
		bool running;
	};
}

#endif
