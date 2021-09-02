#include <memory>
#include "DMTypeDef.h"
#include "DMMonitor.h"

class DMMonitor::Imp {
public:
	Imp() : ownedMutex_(), conditionVariable_(), mutex_(nullptr) { init(ownedMutex_.get()); }
	explicit Imp(std::mutex* mtx) : ownedMutex_(nullptr), conditionVariable_(), mutex_(nullptr) { init(mtx); }
	explicit Imp(DMMonitor* monitor) : ownedMutex_(nullptr), conditionVariable_(), mutex_(nullptr) { init(&(monitor->mutex())); }

	std::mutex& mutex() { return *mutex_; }
	void lock() { mutex_->lock(); }
	void unlock() { mutex_->unlock(); }

	//when you call waitxxx, asume that the monitor is locked.
	int waitForTimeRelative(const std::chrono::milliseconds& timeout) {
		if (0 == timeout.count()) return waitForever();
		//don't lock the mutex on construction.
		std::unique_lock<std::mutex> lk(*mutex_, std::adopt_lock);
		bool bTimedout = (conditionVariable_.wait_for(lk, timeout)
			== std::cv_status::timeout);

		/*the unique_lock no longer manages any mutex object.
		it meas the lock won't unlock by deconstruction.
		*/
		lk.release();

		return bTimedout ? DM_OK : DM_ETIMEDOUT;
	}
	int waitForTime(const std::chrono::time_point<std::chrono::steady_clock>& abstime) {
		std::unique_lock<std::mutex> lk(*mutex_, std::adopt_lock);
		bool bTimedout = (conditionVariable_.wait_until(lk, abstime)
			== std::cv_status::timeout);
		lk.release();
		return bTimedout ? DM_OK : DM_ETIMEDOUT;
	}

	int waitForever() {
		std::unique_lock<std::mutex> lk(*mutex_, std::adopt_lock);
		conditionVariable_.wait(lk);
		lk.release();
		return 0;
	}
	void wait(const std::chrono::milliseconds& timeout) {
		waitForTimeRelative(timeout);
	}

	void notify() {	conditionVariable_.notify_one(); }
	void notifyAll() { conditionVariable_.notify_all(); }

private:
	void init(std::mutex* mtx) { mutex_ = mtx; }
	const std::unique_ptr<std::mutex> ownedMutex_;
	std::condition_variable_any conditionVariable_;
	std::mutex* mutex_;
};

DMMonitor::DMMonitor() { imp_ = new DMMonitor::Imp(); }
DMMonitor::DMMonitor(std::mutex * mtx) { imp_ = new DMMonitor::Imp(mtx); }
DMMonitor::DMMonitor(DMMonitor* monitor) { imp_ = new DMMonitor::Imp(monitor); }
DMMonitor::~DMMonitor() { delete imp_; }

std::mutex& DMMonitor::mutex() const { return const_cast<DMMonitor::Imp*>(imp_)->mutex(); }

void DMMonitor::lock() const { const_cast<DMMonitor::Imp*>(imp_)->lock(); }
void DMMonitor::unlock() const { const_cast<DMMonitor::Imp*>(imp_)->unlock(); }

void DMMonitor::wait(const std::chrono::milliseconds& timeout) const {
	const_cast<DMMonitor::Imp*>(imp_)->wait(timeout);
}

int DMMonitor::waitForever() const { return const_cast<DMMonitor::Imp*>(imp_)->waitForever(); }

int DMMonitor::waitForTime(const std::chrono::time_point<std::chrono::steady_clock>& abstime) const {
	return const_cast<DMMonitor::Imp*>(imp_)->waitForTime(abstime);
}

int DMMonitor::waitForTimeRelative(const std::chrono::milliseconds& timeout) const {
	return const_cast<DMMonitor::Imp*>(imp_)->waitForTimeRelative(timeout);
}

void DMMonitor::notify() const { const_cast<DMMonitor::Imp*>(imp_)->notify(); }
void DMMonitor::notifyAll() const { const_cast<DMMonitor::Imp*>(imp_)->notifyAll(); }