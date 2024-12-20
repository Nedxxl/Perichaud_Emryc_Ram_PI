#include "taskMqtt.hpp"
#include "screen.hpp"
#include "temps.hpp"
#include "alarme.hpp"

TTaskMqtt *tMqtt = NULL;

TTaskMqtt::TTaskMqtt(const char *name, void *shared, int32_t policy, int32_t priority, int32_t cpu) : TThread(name, shared, policy, priority, cpu)

{
    string nameMqtt;

    screen = (TScreen *)shared;

    ram = TRam::getInstance(screen);

    TMqtt::getName(nameMqtt);
    mqtt = new TMqtt(screen, "ramMqtt", (char *)nameMqtt.c_str());

    screen->dispStr(1, 7, "MQTT : ");
    screen->dispStr(10, 7, nameMqtt.c_str());
}

TTaskMqtt::~TTaskMqtt()
{
    if (mqtt)
        delete mqtt;
}

void TTaskMqtt::task(void)
{
    // variable locale
    char strCar[2] = {'-', '\0'};
    const char *pompeState;
    const char *eauChaudeState;
    const char *eauFroideState;
    double valveGbState;
    double valvePbState;
    double valveEauChaudeState;
    double valveEauFroideState;
    double GB;
    double PB;
    double TMP;
    double consigneGB;
    double consignePB;
    double consigneTMP;
    bool ovf_gb = false;
    bool ovf_pb = false;
    // bool mode = false;
    char grosBassin[7];
    char  petitBassin[7];

    // synchronisation démarrage tâche
    signalStart();

    sleep(1);

    while (1)
    {
        // traitement
        if (ram->getDebordementGrosBassin() == 0 && ovf_gb == false)
        {
            ovf_gb = true;
            mqtt->publish(NULL, "RAM/alarmes/états/ALR_GB_OVF", 2, "ON");
        }
        else
        {
            ovf_gb = false;
        }
        if (ram->getDebordementPetitBassin() == 0 && ovf_pb == false)
        {
            ovf_pb = true;
            mqtt->publish(NULL, "RAM/alarmes/états/ALR_PB_OVF", 2, "ON");
        }
        else
        {
            ovf_pb = false;
        }
        // if(mode != ram->getMode())
        // {
        //     mode = ram->getMode();
        //     mqtt->publish(NULL, "RAM/panneau/etats/Mode", strlen(mode ? "manuel" : "auto"), mode ? "manuel" : "auto");
        // }
        if (pompeState != (ram->getPompe() ? "on" : "off"))
        {
            pompeState = ram->getPompe() ? "on" : "off";
            mqtt->publish(NULL, "RAM/panneau/etats/Pompe", strlen(pompeState), pompeState);
        }
        if (eauChaudeState != (ram->getEauChaude() ? "on" : "off"))
        {
            eauChaudeState = ram->getEauChaude() ? "on" : "off";
            mqtt->publish(NULL, "RAM/panneau/etats/ValveEEC", strlen(eauChaudeState), eauChaudeState);
        }
        if (eauFroideState != (ram->getEauFroide() ? "on" : "off"))
        {
            eauFroideState = ram->getEauFroide() ? "on" : "off";
            mqtt->publish(NULL, "RAM/panneau/etats/ValveEEF", strlen(eauFroideState), eauFroideState);
        }
        if (valveGbState != 100 - ram->getValveGrosBassin())
        {
            valveGbState = 100 - ram->getValveGrosBassin();
            mqtt->publish(NULL, "RAM/panneau/etats/ValveGB", to_string(static_cast<int>(valveGbState)).length(), to_string(static_cast<int>(valveGbState)).c_str());
        }
        if (valvePbState != ram->getValvePetitBassin())
        {
            valvePbState = ram->getValvePetitBassin();
            mqtt->publish(NULL, "RAM/panneau/etats/ValvePB", to_string(static_cast<int>(valvePbState)).length(), to_string(static_cast<int>(valvePbState)).c_str());
        }
        if (valveEauChaudeState != ram->getValveEauChaude())
        {
            valveEauChaudeState = ram->getValveEauChaude();
            mqtt->publish(NULL, "RAM/panneau/etats/ValveEC", to_string(static_cast<int>(valveEauChaudeState)).length(), to_string(static_cast<int>(valveEauChaudeState)).c_str());
        }
        if (valveEauFroideState != 100 - ram->getValveEauFroide())
        {
            valveEauFroideState = 100 - ram->getValveEauFroide();
            mqtt->publish(NULL, "RAM/panneau/etats/ValveEF", to_string(static_cast<int>(valveEauFroideState)).length(), to_string(static_cast<int>(valveEauFroideState)).c_str());
        }
        if(ram->getMode() == 0)
        {
        if (GB != ram->getPartageRam()->status.filtreGB)
        {
            GB = ram->getPartageRam()->status.filtreGB;
            sprintf(grosBassin, "%0.2f", GB);
            mqtt->publish(NULL, "RAM/panneau/etats/NivGB", strlen(grosBassin), grosBassin);
        }
        if (PB != ram->getPartageRam()->status.filtrePB)
        {
            PB = ram->getPartageRam()->status.filtrePB;
            sprintf(petitBassin, "%0.2f", PB);
            mqtt->publish(NULL, "RAM/panneau/etats/NivPB", strlen(petitBassin), petitBassin);
        }
        }
        else if (ram->getMode() == 1)
        {
            if (GB != ram->getNiveauGrosBassin())
            {
                GB = ram->getNiveauGrosBassin();
                sprintf(grosBassin, "%0.2f", GB);
                mqtt->publish(NULL, "RAM/panneau/etats/NivGB", strlen(grosBassin), grosBassin);
            }
            if (PB != ram->getNiveauPetitBassin())
            {
                PB = ram->getNiveauPetitBassin();
                sprintf(petitBassin, "%0.2f", PB);
                mqtt->publish(NULL, "RAM/panneau/etats/NivPB", strlen(petitBassin), petitBassin);
            }
        }
        if (TMP != ram->getTemperaturePetitBassin())
        {
            TMP = ram->getTemperaturePetitBassin();
            mqtt->publish(NULL, "RAM/panneau/etats/TmpPB", to_string(static_cast<int>(TMP)).length(), to_string(static_cast<int>(TMP)).c_str());
        }
        if (consigneGB != ram->getPartageRam()->status.consigneNiveauGrosBassin)
        {
            consigneGB = ram->getPartageRam()->status.consigneNiveauGrosBassin;
            mqtt->publish(NULL, "RAM/panneau/etats/ConsNivGB", to_string(static_cast<int>(consigneGB)).length(), to_string(static_cast<int>(consigneGB)).c_str());
        }
        if (consignePB != ram->getPartageRam()->status.consigneNiveauPetitBassin)
        {
            consignePB = ram->getPartageRam()->status.consigneNiveauPetitBassin;
            mqtt->publish(NULL, "RAM/panneau/etats/ConsNivPB", to_string(static_cast<int>(consignePB)).length(), to_string(static_cast<int>(consignePB)).c_str());
        }
        if (consigneTMP != ram->getPartageRam()->status.consigneTemperaturePetitBassin)
        {
            consigneTMP = ram->getPartageRam()->status.consigneTemperaturePetitBassin;
            mqtt->publish(NULL, "RAM/panneau/etats/ConsTmpPB", to_string(static_cast<int>(consigneTMP)).length(), to_string(static_cast<int>(consigneTMP)).c_str());
        }
        if (ram->getPartageRam()->alarmeInfo.alarmeHighGbTrigger == 1)
        {
            ram->getPartageRam()->alarmeInfo.alarmeHighGbTrigger = 0;
            mqtt->publish(NULL, "RAM/alarmes/états/ALR_GB_NIV_MAX", 2, "ON");
        }
        if (ram->getPartageRam()->alarmeInfo.alarmeHighPbTrigger == 1)
        {
            ram->getPartageRam()->alarmeInfo.alarmeHighPbTrigger = 0;
            mqtt->publish(NULL, "RAM/alarmes/états/ALR_PB_NIV_MAX", 2, "ON");
        }

        if (strCar[0] == '|')
            strCar[0] = '-';
        else
            strCar[0] = '|';
        screen->dispStr(8, 7, strCar);

        if (mqtt->loop(200) != 0)
            mqtt->reconnect();
    }
}
