#include "alarme.hpp"

TAlarme::TAlarme(const char *name, void *shared, int policy, int priority, int noCpu) : TThread(name, shared, policy, priority, noCpu)
{
    screen = (TScreen *)shared;

    ram = TRam::getInstance(screen);
    partageRam = ram->getPartageRam();

    screen->dispStr(1, 6, "Alarme :");
}

TAlarme::~TAlarme()
{
}

void TAlarme::task(void)
{
    // variable locale
    char strCar[2] = {'-', '\0'};

    // synchronisation démarrage tâche
    signalStart();

    while (1)
    {
        // attendre acquisition complété
        partageRam->synAlarme.take();

        // traitement
        if (ram->getDebordementGrosBassin() == 0)
        {
            temps.startMesure();
            if (temps.mesure_us() >= 1500000)
                ram->setPompe(false);
        }
        else
        {
            temps.stopMesure();
        }
        if (ram->getDebordementPetitBassin() == 0)
        {
            temps.startMesure();
            if (temps.mesure_us() >= 1500000)
            {
                ram->setPompe(false);
                ram->setEauChaude(false);
                ram->setEauFroide(false);
            }
        }
        else
        {
            temps.stopMesure();
        }

        if (strCar[0] == '|')
            strCar[0] = '-';
        else
            strCar[0] = '|';
        screen->dispStr(11, 6, strCar);
    }
}
