#include "SoftwareWatchdog.h"

volatile system_tick_t SoftwareWatchdog::last_checkin = 0;

os_thread_return_t SoftwareWatchdog::start(void* pointer)
{
	SoftwareWatchdog& wd = *(SoftwareWatchdog*)pointer;
	wd.loop();
	os_thread_cleanup(nullptr);
}

void SoftwareWatchdog::loop()
 {
    auto wakeupTimestamp = last_checkin + timeout;
 	auto now = HAL_Timer_Get_Milli_Seconds();

 	if (wakeupTimestamp > now) {
 		HAL_Delay_Milliseconds(wakeupTimestamp - now);
 	}

 	while (true) {
        if ((now-last_checkin)>=timeout) {
 			break;
 		}
 		HAL_Delay_Milliseconds(timeout);
    }

 	if (timeout>0 && timeout_fn) {
 		timeout_fn();
 		timeout_fn = std::function<void(void)>();
 	}
 }
