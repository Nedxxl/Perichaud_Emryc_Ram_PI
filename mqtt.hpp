#ifndef MQTT_HPP
#define MQTT_HPP

#include <mosquittopp.h>
#include <string>
#include <unordered_map>
#include <functional>
#include "screen.hpp"
#include "ram.hpp"

using namespace std;

class TMqtt : public mosqpp::mosquittopp
    {
    private:
		unordered_map<string,std::function<void (TMqtt *,string)>> mqttTable;
		TScreen *screen;
		TRam *ram;

		void mode(std::string val);
		void pompe(std::string val);
		void entrerEauFroide(std::string val);
		void entrerEauChaude(std::string val);
		void eauFroide(std::string val);
		void eauChaude(std::string val);
		void valveGb(std::string val);
		void valvePb(std::string val);
		void consigneGb(std::string val);
		void consignePb(std::string val);
		void consigneTempPb(std::string val);

    public:
		TMqtt(void *shared,const char *id,char *host,int port = 1883);
		virtual ~TMqtt();

		void on_connect(int rc);
		void on_message(const struct mosquitto_message *message);
		void on_subscribe(int mid,int qos_count,const int *granted_pos);

		static void getName(string &name);
    };

#endif //MQTT_HPP
