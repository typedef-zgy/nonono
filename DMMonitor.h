#ifndef DMMONITOR_H
#define DMMONITOR_H

#include <chrono>
#include <condition_variable>
#include <mutex>

#include "DMNonCopyable.h"

class DMMonitor : public DMNonCopyable {
public:
	DMMonitor();
	explicit DMMonitor(std::mutex * mtx);
	explicit DMMonitor(DMMonitor* monitor);
	virtual ~DMMonitor();

	std::mutex& mutex() const;
	virtual void lock() const;
	virtual void unlock() const;

	/* when you call waitxxx, i asume that the monitor is locked. so i don't lock in these functions.
	*/
	//wait a duration
	int waitForTimeRelative(const std::chrono::milliseconds& timeout) const;
	int waitForTimeRelative(uint64_t timeout_ms) const { return waitForTimeRelative(std::chrono::milliseconds(timeout_ms)); };

	//wait until a timepoint
	int waitForTime(const std::chrono::time_point<std::chrono::steady_clock>& abstime) const;

	int waitForever() const;

	void wait(const std::chrono::milliseconds& timeout) const;
	void wait(uint64_t timeout_ms) const { this->wait(std::chrono::milliseconds(timeout_ms)); }

	virtual void notify() const;
	virtual void notifyAll() const;

private:
	class Imp;
	Imp *imp_;
};

#endif	// DMMONITOR_H
