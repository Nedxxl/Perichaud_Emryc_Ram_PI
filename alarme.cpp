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
                ram->setPompe(false);
        }
        if (ram->getDebordementPetitBassin() == 0)
        {
                ram->setPompe(false);
                ram->setEauChaude(false);
                ram->setEauFroide(false);
        }
        if (ram->getNiveauGrosBassin() > ram->getPartageRam()->alarmeInfo.nvHighGB)
        {
            if(activerTempGB == false)
            {
                tempsGB.startMesure();
                activerTempGB = true;
            }
            tempsGB.stopMesure();
            if (tempsGB.mesure_us() >= (ram->getPartageRam()->alarmeInfo.tmpHighGB * 1000000))
            {
                ram->setAlarmeHighGbTrigger(true);
            }
        }
        else
        {
            activerTempGB = false;
        }
        if (ram->getNiveauPetitBassin() > ram->getPartageRam()->alarmeInfo.nvHighPB)
        {
            if(activerTempPB == false)
            {
                tempsPB.startMesure();
                activerTempPB = true;
            }
            if (tempsPB.mesure_us() >= (ram->getPartageRam()->alarmeInfo.tmpHighPB * 1000000))
            {
                ram->setAlarmeHighPbTrigger(true);
            }
        }
        else
        {
            activerTempPB = false;
        }



        if (strCar[0] == '|')
            strCar[0] = '-';
        else
            strCar[0] = '|';
        screen->dispStr(11, 6, strCar);
    }
}
