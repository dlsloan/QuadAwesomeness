#ifndef _mutex_h
#define _mutex_h


namespace OS {
	class Mutex {
		private:
			volatile bool locked;
		public:
			Mutex();
			//lock must not be called from ISRs (including tryLock)
			//  ISRs can execute during critical section allowing non-atomic locks
			void PendLock();
			bool PendLock(int timeout);
			bool TryLock();

			void Release();
			//static for ISRs
			static void Release(Mutex* lock);
	};
}


#endif
