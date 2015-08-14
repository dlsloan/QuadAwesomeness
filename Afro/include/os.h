#ifndef _os_h
#define _os_h

#include <types.h>

namespace OS {

	typedef int (*taskFunction)(u32);

	class Task {
		public:
			const err ErrorTaskActive = "Error Task Active";

			Task(taskFunction taskStart, u32* stack, int stackLength);

			void Start(u32 startArg);

			inline bool __attribute__((always_inline)) isActive() {
				return this->active || this->running;
			}

		private:
			taskFunction taskStart;
			Task* next;
			u32* sp;
			u32* stack;
			err* errMsg;
			int stackLength;
			int retVal;
			bool running;
			bool active;

			void SignalStarted();
			void SignalReturned(int);
			void SignalStopped();

			static void CallTaskStart(u32 funcPtr, u32 arg, u32 taskPtr);
	};

	void Init();

	void SetError(err);
	err GetError();

	//Task* CurrentTask();

	void SwitchTask();

	u32 SystemTicks();

	void __enterCritical();
	void __exitCritical();

	void _StartTask(taskFunction, u32, u32*, int);
}
#endif
