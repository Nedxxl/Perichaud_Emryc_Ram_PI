#include "mqtt.hpp"
#include "screen.hpp"
#include "ram.hpp"
#include "alarme.hpp"

TMqtt::TMqtt(void *shared,const char *id,char *host,int port) : mosqpp::mosquittopp(id)
	{
	screen = (TScreen *)shared;
	ram = TRam::getInstance(screen);

	mqttTable["RAM/panneau/cmd/Mode"] = &TMqtt::mode;
	mqttTable["RAM/panneau/cmd/Pompe"] = &TMqtt::pompe;
	mqttTable["RAM/panneau/cmd/ValveEEF"] = &TMqtt::entrerEauFroide;
	mqttTable["RAM/panneau/cmd/ValveEEC"] = &TMqtt::entrerEauChaude;
	mqttTable["RAM/panneau/cmd/ValveEF"] = &TMqtt::eauFroide;
	mqttTable["RAM/panneau/cmd/ValveEC"] = &TMqtt::eauChaude;
	mqttTable["RAM/panneau/cmd/ValveGB"] = &TMqtt::valveGb;
	mqttTable["RAM/panneau/cmd/ValvePB"] = &TMqtt::valvePb;
	mqttTable["RAM/panneau/cmd/ConsNivGB"] = &TMqtt::consigneGb;
	mqttTable["RAM/panneau/cmd/ConsNivPB"] = &TMqtt::consignePb;
	mqttTable["RAM/panneau/cmd/ConsTmpPB"] = &TMqtt::consigneTempPb;


	mosqpp::lib_init();

	connect(host,port,120);
	}

TMqtt::~TMqtt()
	{
	mosqpp::lib_cleanup();
	}

void TMqtt::on_connect(int rc)
	{
	if(rc == 0)
	    {
	    screen->dispStr(30,7,"(Connected : OK)");

		subscribe(NULL,"RAM/#",0);
	    }
	else
	    screen->dispStr(30,7,"(Connected : Fail)");
	}

void TMqtt::on_message(const struct mosquitto_message *message)
	{
	string topic = message->topic;
	string payload = (char *)message->payload;

	auto it = mqttTable.find(topic);
	if(it != mqttTable.end())
	    it->second(this,payload);
	}

void TMqtt::on_subscribe(int mid,int qos_count,const int *granted_pos)
	{
	screen->dispStr(50,7,"(Subcribe)");
	}

void TMqtt::getName(string &name)
	{
	name = "172.17.15.174";
	}

void TMqtt::mode(string val)
	{
		if(val == "auto")
			ram->setMode(false);
		else if(val == "manuel")
			ram->setMode(true);
	}

	void TMqtt::pompe(string val)
	{
		if(val == "on")
			ram->setPompe(true);
		else if(val == "off")
			ram->setPompe(false);
	}

	void TMqtt::entrerEauFroide(string val)
	{
		if(ram->getPartageRam()->status.mode == 1)
		{
		if(val == "on")
			ram->setEauFroide(true);
		else if(val == "off")
			ram->setEauFroide(false);
		}
	}

	void TMqtt::entrerEauChaude(string val)
	{
		if(ram->getPartageRam()->status.mode == 1)
		{
		if(val == "on")
			ram->setEauChaude(true);
		else if(val == "off")
			ram->setEauChaude(false);
		}
	}

	void TMqtt::eauFroide(string val)
	{
		if(ram->getPartageRam()->status.mode == 1)
		{
		ram->setValveEauFroide(100-stod(val));
		}
	}

	void TMqtt::eauChaude(string val)
	{
		if(ram->getPartageRam()->status.mode == 1)
		{
		ram->setValveEauChaude(stod(val));
		}
	}

	void TMqtt::valveGb(string val)
	{
		if(ram->getPartageRam()->status.mode == 1)
		{
		ram->setValveGrosBassin(100-stod(val));
		}
	}

	void TMqtt::valvePb(string val)
	{
		if(ram->getPartageRam()->status.mode == 1)
		{
		ram->setValvePetitBassin(stod(val));
		}
	}

	void TMqtt::consigneGb(string val)
	{
		if(ram->getPartageRam()->status.mode == 0)
		{
		ram->setConsigneNiveauGrosBassin(stod(val));
		}
	}

	void TMqtt::consignePb(string val)
	{
		if(ram->getPartageRam()->status.mode == 0)
		{
		ram->setConsigneNiveauPetitBassin(stod(val));
		}
	}

	void TMqtt::consigneTempPb(string val)
	{
		if(ram->getPartageRam()->status.mode == 0)
		{
		ram->setConsigneTemperaturePetitBassin(stod(val));
		}
	}

