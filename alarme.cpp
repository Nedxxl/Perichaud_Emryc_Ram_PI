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
            if (temps.mesure_us() >= 1000000)
                ram->setPompe(false);
        }
        else if (ram->getDebordementPetitBassin() == 0)
        {
            temps.startMesure();
            if (temps.mesure_us() >= 1000000)
            {
                ram->setPompe(false);
                ram->setEauChaude(false);
                ram->setEauFroide(false);
            }
        }
        else if (ram->getNiveauGrosBassin() > ram->getPartageRam()->alarmeInfo.nvHighGB)
        {
            temps.startMesure();
            if (temps.mesure_us() >= (ram->getPartageRam()->alarmeInfo.tmpHighGB*1000000))
            {
                ram->getPartageRam()->alarmeInfo.alarmeHighGbTrigger = 1;
            }
        }
        else if (ram->getNiveauPetitBassin() > ram->getPartageRam()->alarmeInfo.nvHighPB)
        {
            temps.startMesure();
            if (temps.mesure_us() >= (ram->getPartageRam()->alarmeInfo.tmpHighPB*1000000))
            {
                ram->getPartageRam()->alarmeInfo.alarmeHighPbTrigger = 1;
            }
        }
        else
        {
            temps.stopMesure();
        }
    }
}
