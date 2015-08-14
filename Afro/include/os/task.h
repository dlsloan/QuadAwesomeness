#ifndef _task_h
#define _task_h

#include <types.h>

class Task;

#include <os.h>

namespace OS {
	class Task {
	public:
		const err ErrorTaskActive = "Error Task Active";

		Task(OS::taskFunction taskStart, u32* stack, int stackLength);

		void Start(u32 startArg);

		inline void __attribute__((always_inline))
			_SignalStarted() { this->running = true; this->active = true; }
		inline void __attribute__((always_inline))
			_SignalReturned(int retVal) { this->running = false; this->retVal = retVal; }
		inline void __attribute__((always_inline))
			_SignalStopped() { this->running = false; this->active = false;}
		inline bool __attribute__((always_inline))
			_isRunning() { return this->running; }
		inline bool __attribute__((always_inline)) isActive() {
			return this->active || this->running;
		}

	private:
		OS::taskFunction taskStart;
		Task* next;
		u32* sp;
		u32* stack;
		err* errMsg;
		int stackLength;
		int retVal;
		bool running;
		bool active;
	};
}

#endif
