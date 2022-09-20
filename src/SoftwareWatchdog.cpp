#include "SoftwareWatchdog.h"

volatile system_tick_t SoftwareWatchdog::last_checkin = 0;

os_thread_return_t SoftwareWatchdog::start(void* pointer)
{
	SoftwareWatchdog& wd = *(SoftwareWatchdog*)pointer;

    //when we start, trigger an initial checkin pulse
    last_checkin = HAL_Timer_Get_Milli_Seconds();

	wd.loop();
	os_thread_cleanup(nullptr);
}

void SoftwareWatchdog::loop()
 {
    while (true) {
        auto now = HAL_Timer_Get_Milli_Seconds();
        auto timeSinceLastCheckin = (now-last_checkin);
        if (timeSinceLastCheckin >= timeout) {
 			break;
 		}
 		HAL_Delay_Milliseconds(timeout - timeSinceLastCheckin);
    }

 	if (timeout>0 && timeout_fn) {
 		timeout_fn();
 		timeout_fn = std::function<void(void)>();
 	}
 }
