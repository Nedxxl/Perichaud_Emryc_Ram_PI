#ifndef TTASK_MQTT_HPP
#define  TTASK_MQTT_HPP

#include "thread.hpp"
#include "screen.hpp"
#include "mqtt.hpp"
#include "ram.hpp"

class TTaskMqtt : public TThread
    {
    private:
		TScreen *screen;
		TMqtt *mqtt;
		TRam *ram;
    public:
		TTaskMqtt(const char *name,void *shared,int32_t policy,int32_t priority,int32_t cpu = -1);
		~TTaskMqtt();

		void task(void);
    };

#endif // TTASK_MQTT_HPP
