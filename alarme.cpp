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
    bool highGbOnce = false;
    bool highPbOnce = false;

    bool activerTempGB;
    bool activerTempPB;
    bool activerRepGB;
    bool activerRepPB;

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
            if (ram->getPartageRam()->alarmeInfo.ackGB == false)
            {
                if (activerTempGB == false)
                {
                    tempsGB.startMesure();
                    activerTempGB = true;
                }
                tempsGB.stopMesure();
                if ((tempsGB.mesure_us() >= (ram->getPartageRam()->alarmeInfo.tmpHighGB * 1000000)) && (highGbOnce == false))
                {
                    highGbOnce = true;
                    activerTempGB = false;
                    activerRepGB = false;
                    ram->setAlarmeHighGbTrigger(true);
                }
            }
            else if (ram->getPartageRam()->alarmeInfo.ackGB == true)
            {
                if (activerRepGB == false)
                {
                    tempsGB.startMesure();
                    activerRepGB = true;
                }
                tempsGB.stopMesure();
                if ((tempsGB.mesure_us() >= (ram->getPartageRam()->alarmeInfo.tmpRepGB * 1000000)))
                {
                    activerRepGB = false;
                    ram->setAckGB(false);
                    ram->setAlarmeHighGbTrigger(true);
                }
            }
        }
        else
        {
            ram->setAckGB(false);
            activerTempGB = false;
            highGbOnce = false;
        }
        if (ram->getNiveauPetitBassin() > ram->getPartageRam()->alarmeInfo.nvHighPB)
        {
            if (ram->getPartageRam()->alarmeInfo.ackPB == false)
            {
                if (activerTempPB == false)
                {
                    tempsPB.startMesure();
                    activerTempPB = true;
                }
                tempsPB.stopMesure();
                if ((tempsPB.mesure_us() >= (ram->getPartageRam()->alarmeInfo.tmpHighPB * 1000000)) && (highPbOnce == false))
                {
                    highPbOnce = true;
                    activerTempPB = false;
                    activerRepPB = false;
                    ram->setAlarmeHighPbTrigger(true);
                }
            }
            else if (ram->getPartageRam()->alarmeInfo.ackPB == true)
            {
                if (activerRepPB == false)
                {
                    tempsPB.startMesure();
                    activerRepPB = true;
                }
                tempsPB.stopMesure();
                if ((tempsPB.mesure_us() >= (ram->getPartageRam()->alarmeInfo.tmpRepPB * 1000000)))
                {
                    activerRepPB = false;
                    ram->setAckPB(false);
                    ram->setAlarmeHighPbTrigger(true);
                }
            }
        }
        else
        {
            ram->setAckPB(false);
            activerTempPB = false;
            highPbOnce = false;
        }

        if (strCar[0] == '|')
            strCar[0] = '-';
        else
            strCar[0] = '|';
        screen->dispStr(11, 6, strCar);
    }
}
