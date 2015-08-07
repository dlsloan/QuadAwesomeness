#include <os.h>
#include <mutex.h>

using namespace OS;


Mutex::Mutex() {
	this->locked = false;
}

void Mutex::PendLock() {
	while(true) {
		if (this->locked)
			OS::SwitchTask();
		else {
			OS::__enterCritical();
			if (this->locked) {
				OS::__exitCritical();
				continue;
			} else {
				this->locked = true;
				OS::__exitCritical();
				return;
			}
		}
	}
}

bool Mutex::PendLock(int timeout) {
	int start = OS::SystemTicks();
	while((int)(OS::SystemTicks() - start) < timeout) {
		if (this->locked)
			OS::SwitchTask();
		else {
			OS::__enterCritical();
			if (this->locked) {
				OS::__exitCritical();
				continue;
			} else {
				this->locked = true;
				OS::__exitCritical();
				return true;
			}
		}
	}
	return false;
}

bool Mutex::TryLock() {
	if(this->locked) return false;
	OS::__enterCritical();
	if(this->locked) {
		OS::__exitCritical();
		return false;
	} else {
		this->locked = true;
		OS::__exitCritical();
		return true;
	}
}

void Mutex::Release() {
	this->locked = false;
}

void Mutex::Release(Mutex* lock) {
	lock->locked = false;
}


