#pragma once

#include <chrono>
#include "Particle.h"

class SoftwareWatchdog
{
	volatile system_tick_t timeout;
	static volatile system_tick_t last_checkin;

	std::function<void(void)> timeout_fn;

	Thread* thread;

	static void start(void* pointer);

	void loop();

public:
    static const unsigned DEFAULT_STACK_SIZE = 512;

	SoftwareWatchdog(unsigned timeout_ms, std::function<void(void)> fn, unsigned stack_size=DEFAULT_STACK_SIZE) :
		timeout(timeout_ms),
		timeout_fn(fn)
	{
		checkin();
        thread = new Thread("appwdt", start, this, OS_THREAD_PRIORITY_CRITICAL, stack_size);
	}
	SoftwareWatchdog(std::chrono::milliseconds ms, std::function<void(void)> fn, unsigned stack_size=DEFAULT_STACK_SIZE) : SoftwareWatchdog(ms.count(), fn, stack_size) {}

    // This constuctor helps to resolve overloaded function types, such as System.reset(), which is not always
    // possible in case of std::function
    SoftwareWatchdog(unsigned timeout_ms, void (*fn)(), unsigned stack_size=DEFAULT_STACK_SIZE) :
        SoftwareWatchdog(timeout_ms, std::function<void()>(fn), stack_size)
    {
    }
    SoftwareWatchdog(std::chrono::milliseconds ms, void (*fn)(), unsigned stack_size=DEFAULT_STACK_SIZE) : SoftwareWatchdog(ms.count(), fn, stack_size) {}

	~SoftwareWatchdog() {
 		if (thread) {
 			// NOTE: this may have to wait up to original timeout for the thread to exit
 			delete thread;
 		}
 	}

	/**
	 * Lifesign that the application is still working normally.
	 */
	static void checkin()
	{
		last_checkin = HAL_Timer_Get_Milli_Seconds();
	}

	/**
	 * @brief Call this before sleep
	 * 
	 * This eliminates the coupling of the watchdog timeout and the sleep duration. The problem
	 * is that without this, when the device wakes from STOP or ULP sleep modes, the millis 
	 * tick counter will be incremented, which can cause the watchdog timer to fire. Because the
	 * watchdog timer thread is a high priority thread, it's likely to run before the user
	 * thread can call checkin().
	 */
	static void stop() 
	{
		last_checkin = 0;
	}
};
  