#include "ram.hpp"
#include "com_1.hpp"

TRam::TRam(TScreen *s)
{
    screen = s;

    partageRam.status.mode = 1; // manuel
    partageRam.status.pompe = 0;
    partageRam.status.eauChaude = 0;
    partageRam.status.eauFroide = 0;
    partageRam.status.debordementGrosBassin = 1;  // pas de debordement
    partageRam.status.debordementPetitBassin = 1; // pas de debordement

    partageRam.status.valveGrosBassin = 0;
    partageRam.status.valvePetitBassin = 0;
    partageRam.status.valveEauChaude = 0;
    partageRam.status.valveEauFroide = 0;
    partageRam.status.niveauGrosBassin = 0;
    partageRam.status.niveauPetitBassin = 0;
    partageRam.status.temperatureGrosBassin = 0;
    partageRam.status.temperaturePetitBassin = 0;

    partageRam.status.filtreGB = 0;
    partageRam.status.filtrePB = 0;
    partageRam.status.filtreTempFroid = 0;
    partageRam.status.filtreTempChaud = 0;

    partageRam.status.consigneNiveauGrosBassin = 0;
    partageRam.status.consigneNiveauPetitBassin = 0;
    partageRam.status.consigneTemperaturePetitBassin = 0;

    partageRam.alarmeInfo.nvHighGB = 90;
    partageRam.alarmeInfo.tmpHighGB = 3;
    partageRam.alarmeInfo.tmpRepGB = 3;
    partageRam.alarmeInfo.nvHighPB = 90;
    partageRam.alarmeInfo.tmpHighPB = 3;
    partageRam.alarmeInfo.tmpRepGB = 3;
    partageRam.alarmeInfo.alarmeHighGbTrigger = 0;
    partageRam.alarmeInfo.alarmeHighPbTrigger = 0;

    // init semaphore pour synchronisation
    partageRam.synControl = 0;
    partageRam.synAlarme = 0;

    std::string portName;
    TCom1::getName(portName);
    com_1 = new TCom1(portName.c_str(), (void *)screen, (void *)&partageRam, 95, TCom::b115200, TCom::pNONE, TCom::dS8, 10);
    if (com_1)
        com_1->start();

    screen->dispStr(6, 3, portName);
}

TRam::~TRam()
{
    if (com_1)
        delete com_1;
}

// acquisition
void TRam::acquisition(void)
{
    // variable locale
    char bufCmd[10];

    /*
     * Acquisition
     */

    // Lecture AI (module 1)
    bufCmd[0] = '#';
    bufCmd[1] = '0';
    bufCmd[2] = '1';
    bufCmd[3] = '\x0D';

    com_1->takeCom1();
    com_1->setSignalTimeout(1);
    com_1->sendTx(bufCmd, 4);

    if (com_1->waitRepRequest() != 0)
    {
        sprintf(bufCmd, "%06d", com_1->getCptTimeout());
        screen->dispStr(54, 3, bufCmd);
    }
    com_1->releaseCom1();

    // Lecture DI (module 5)
    bufCmd[0] = '@';
    bufCmd[1] = '0';
    bufCmd[2] = '5';
    bufCmd[3] = '\x0D';
    com_1->takeCom1();
    com_1->setSignalTimeout(1);
    com_1->sendTx(bufCmd, 4);

    if (com_1->waitRepRequest() != 0)
    {
        sprintf(bufCmd, "%06d", com_1->getCptTimeout());
        screen->dispStr(54, 3, bufCmd);
    }
    com_1->releaseCom1();

    // Synchronisation échantillonage
    // broadcast
    bufCmd[0] = '#';
    bufCmd[1] = '*';
    bufCmd[2] = '*';
    bufCmd[3] = '\x0D';
    com_1->takeCom1();
    com_1->setSignalTimeout(0);
    com_1->sendTx(bufCmd, 4);
    com_1->releaseCom1();
}

void TRam::format(void)
{
    double aiValue[8];
    ram_t status;

    memcpy(aiValue, partageRam.rawDataAI, sizeof aiValue);
    getStatusRam(&status);

    // AI1
    status.valveEauChaude = CourantToPourcent /*CourantToPourcentValveEC*/ (aiValue[0]);
    status.valveEauChaude = AjustPourcent(status.valveEauChaude);
    // AI2
    status.valveGrosBassin = CourantToPourcent(aiValue[1]);
    status.valveGrosBassin = AjustPourcent(status.valveGrosBassin);
    // AI3
    status.valvePetitBassin = CourantToPourcent(aiValue[2]);
    status.valvePetitBassin = AjustPourcent(status.valvePetitBassin);
    // AI4
    status.valveEauFroide = CourantToPourcent /*CourantToPourcentValveEF*/ (aiValue[3]);
    status.valveEauFroide = AjustPourcent(status.valveEauFroide);
    // AI5					 // calibration
    status.niveauGrosBassin = CourantToPourcent(aiValue[4] + 0.1);
    status.niveauGrosBassin = AjustPourcent(status.niveauGrosBassin);
    // AI6
    status.niveauPetitBassin = CourantToPourcent(aiValue[5]);
    status.niveauPetitBassin = AjustPourcent(status.niveauPetitBassin);
    // AI7
    status.temperatureGrosBassin = AjustCourant(aiValue[6]);
    status.temperatureGrosBassin = CourantToTempGrosBassin(status.temperatureGrosBassin);
    // AI8
    status.temperaturePetitBassin = AjustCourant(aiValue[7]);
    status.temperaturePetitBassin = CourantToTempPetitBassin(status.temperaturePetitBassin);

    setStatusRam(&status);
}

void TRam::init(void)
{
    // numérique
    setPompe(0);

    setEauFroide(0);
    setEauChaude(0);

    // analogique
    setValveGrosBassin(100); // normalement ouverte
    setValvePetitBassin(0);  // normalement fermee

    setValveEauFroide(100); // normalement ouverte
    setValveEauChaude(0);   // normalement fermee
}

// Actionneur
void TRam::setPompe(bool onOff)
{
    char bufCmd[10];

    // module 4 DO1
    sprintf(bufCmd, "#041%d%02d", 0, onOff);
    bufCmd[7] = '\x0D';

    com_1->takeCom1();
    com_1->setSignalTimeout(1);
    com_1->sendTx(bufCmd, 8);

    if (com_1->waitRepRequest() != 0)
    {
        sprintf(bufCmd, "%06d", com_1->getCptTimeout());
        screen->dispStr(54, 3, bufCmd);
    }
    else
    {
        partageRam.lock.take();
        partageRam.status.pompe = onOff;
        partageRam.lock.release();
    }
    com_1->releaseCom1();
}

void TRam::setEauFroide(bool onOff)
{
    char bufCmd[10];

    // module 4 DO3
    sprintf(bufCmd, "#041%d%02d", 2, onOff);
    bufCmd[7] = '\x0D';

    com_1->takeCom1();
    com_1->setSignalTimeout(1);
    com_1->sendTx(bufCmd, 8);

    if (com_1->waitRepRequest() != 0)
    {
        sprintf(bufCmd, "%06d", com_1->getCptTimeout());
        screen->dispStr(54, 3, bufCmd);
    }
    else
    {
        partageRam.lock.take();
        partageRam.status.eauFroide = onOff;
        partageRam.lock.release();
    }
    com_1->releaseCom1();
}

void TRam::setEauChaude(bool onOff)
{
    char bufCmd[10];

    // module 4 DO2
    sprintf(bufCmd, "#041%d%02d", 3, onOff);
    bufCmd[7] = '\x0D';

    com_1->takeCom1();
    com_1->setSignalTimeout(1);
    com_1->sendTx(bufCmd, 8);

    if (com_1->waitRepRequest() != 0)
    {
        sprintf(bufCmd, "%06d", com_1->getCptTimeout());
        screen->dispStr(54, 3, bufCmd);
    }
    else
    {
        partageRam.lock.take();
        partageRam.status.eauChaude = onOff;
        partageRam.lock.release();
    }
    com_1->releaseCom1();
}

void TRam::setFiltreGB(double val)
{
    partageRam.lock.take();
    partageRam.status.filtreGB = val;
    partageRam.lock.release();
}

void TRam::setFiltrePB(double val)
{
    partageRam.lock.take();
    partageRam.status.filtrePB = val;
    partageRam.lock.release();
}

void TRam::setFiltreTempFroid(double val)
{
    partageRam.lock.take();
    partageRam.status.filtreTempFroid = val;
    partageRam.lock.release();
}

void TRam::setFiltreTempChaud(double val)
{
    partageRam.lock.take();
    partageRam.status.filtreTempChaud = val;
    partageRam.lock.release();
}

bool TRam::getPompe(void)
{
    bool retour;

    partageRam.lock.take();
    retour = partageRam.status.pompe;
    partageRam.lock.release();

    return retour;
}

bool TRam::getEauFroide(void)
{
    bool retour;

    partageRam.lock.take();
    retour = partageRam.status.eauFroide;
    partageRam.lock.release();

    return retour;
}

bool TRam::getEauChaude(void)
{
    bool retour;

    partageRam.lock.take();
    retour = partageRam.status.eauChaude;
    partageRam.lock.release();

    return retour;
}

void TRam::setValveGrosBassin(double ouverturePourcent)
{
    char bufCmd[15];

    // conversion courant
    ouverturePourcent = PourcentToCourant(ouverturePourcent);
    ouverturePourcent = 24.0 - ouverturePourcent;

    // module 2 AO2
    sprintf(bufCmd, "#%02d%d%+07.3lf", 2, 1, ouverturePourcent);
    bufCmd[11] = '\x0D';

    com_1->takeCom1();
    com_1->setSignalTimeout(1);
    com_1->sendTx(bufCmd, 12);

    if (com_1->waitRepRequest() != 0)
    {
        sprintf(bufCmd, "%06d", com_1->getCptTimeout());
        screen->dispStr(54, 3, bufCmd);
    }
    else
    {
        // partageRam.lock.take();
        // partageRam.status.valveGrosBassin = ouverturePourcent;
        // partageRam.lock.release();
    }
    com_1->releaseCom1();
}
void TRam::setValvePetitBassin(double ouverturePourcent)
{
    char bufCmd[15];

    // conversion courant
    ouverturePourcent = PourcentToCourant(ouverturePourcent);
    ouverturePourcent = 24.0 - ouverturePourcent;

    // module 2 AO3
    sprintf(bufCmd, "#%02d%d%+07.3lf", 2, 2, ouverturePourcent);
    bufCmd[11] = '\x0D';

    com_1->takeCom1();
    com_1->setSignalTimeout(1);
    com_1->sendTx(bufCmd, 12);

    if (com_1->waitRepRequest() != 0)
    {
        sprintf(bufCmd, "%06d", com_1->getCptTimeout());
        screen->dispStr(54, 3, bufCmd);
    }
    else
    {
        // partageRam.lock.take();
        // partageRam.status.valvePetitBassin = ouverturePourcent;
        // partageRam.lock.release();
    }
    com_1->releaseCom1();
}

void TRam::setValveEauChaude(double ouverturePourcent)
{
    char bufCmd[15];

    // conversion courant
    ouverturePourcent = PourcentToCourant(ouverturePourcent);
    ouverturePourcent = 24.0 - ouverturePourcent;

    // module 2 AO1
    sprintf(bufCmd, "#%02d%d%+07.3lf", 2, 0, ouverturePourcent);
    bufCmd[11] = '\x0D';

    com_1->takeCom1();
    com_1->setSignalTimeout(1);
    com_1->sendTx(bufCmd, 12);

    if (com_1->waitRepRequest() != 0)
    {
        sprintf(bufCmd, "%06d", com_1->getCptTimeout());
        screen->dispStr(54, 3, bufCmd);
    }
    else
    {
        // partageRam.lock.take();
        // partageRam.status.valveEauChaude = ouverturePourcent;
        // partageRam.lock.release();
    }
    com_1->releaseCom1();
}

void TRam::setValveEauFroide(double ouverturePourcent)
{
    char bufCmd[15];

    // conversion courant
    ouverturePourcent = PourcentToCourant(ouverturePourcent); // PourcentToCourantValveEF(ouverturePourcent);
    ouverturePourcent = 24.0 - ouverturePourcent;

    // module 2 AO4
    sprintf(bufCmd, "#%02d%d%+07.3lf", 2, 3, ouverturePourcent);
    bufCmd[11] = '\x0D';

    com_1->takeCom1();
    com_1->setSignalTimeout(1);
    com_1->sendTx(bufCmd, 12);

    if (com_1->waitRepRequest() != 0)
    {
        sprintf(bufCmd, "%06d", com_1->getCptTimeout());
        screen->dispStr(54, 3, bufCmd);
    }
    else
    {
        // partageRam.lock.take();
        // partageRam.status.valveEauFroide = ouverturePourcent;
        // partageRam.lock.release();
    }
    com_1->releaseCom1();
}

double TRam::getValveGrosBassin(void)
{
    double retour;

    partageRam.lock.take();
    retour = partageRam.status.valveGrosBassin;
    partageRam.lock.release();

    return retour;
}

double TRam::getValvePetitBassin(void)
{
    double retour;

    partageRam.lock.take();
    retour = partageRam.status.valvePetitBassin;
    partageRam.lock.release();

    return retour;
}

double TRam::getValveEauChaude(void)
{
    double retour;

    partageRam.lock.take();
    retour = partageRam.status.valveEauChaude;
    partageRam.lock.release();

    return retour;
}

double TRam::getValveEauFroide(void)
{
    double retour;

    partageRam.lock.take();
    retour = partageRam.status.valveEauFroide;
    partageRam.lock.release();

    return retour;
}

// Capteur
bool TRam::getDebordementGrosBassin(void)
{
    bool retour;

    partageRam.lock.take();
    retour = partageRam.status.debordementGrosBassin;
    partageRam.lock.release();

    return retour;
}

bool TRam::getDebordementPetitBassin(void)
{
    bool retour;

    partageRam.lock.take();
    retour = partageRam.status.debordementPetitBassin;
    partageRam.lock.release();

    return retour;
}

double TRam::getNiveauGrosBassin(void)
{
    double retour;

    partageRam.lock.take();
    retour = partageRam.status.niveauGrosBassin;
    partageRam.lock.release();

    return retour;
}

double TRam::getNiveauPetitBassin(void)
{
    double retour;

    partageRam.lock.take();
    retour = partageRam.status.niveauPetitBassin;
    partageRam.lock.release();

    return retour;
}

double TRam::getTemperatureGrosBassin(void)
{
    double retour;

    partageRam.lock.take();
    retour = partageRam.status.temperatureGrosBassin;
    partageRam.lock.release();

    return retour;
}

double TRam::getTemperaturePetitBassin(void)
{
    double retour;

    partageRam.lock.take();
    retour = partageRam.status.temperaturePetitBassin;
    partageRam.lock.release();

    return retour;
}

// Consigne
void TRam::setMode(bool manAuto)
{
    partageRam.lock.take();
    partageRam.status.mode = manAuto;
    partageRam.lock.release();

    // setPompe(0);

    // setValveGrosBassin(100); // normalement ouverte
    // setValvePetitBassin(0);  // normalement fermee

    // setValveEauFroide(100); // normalement ouverte
    // setValveEauChaude(0);   // normalement fermee

    if (manAuto == 0) // auto
    {
        setEauFroide(1);
        setEauChaude(1);
    }
    else
    {
        setEauFroide(0);
        setEauChaude(0);
    }
}

bool TRam::getMode(void)
{
    bool retour;

    partageRam.lock.take();
    retour = partageRam.status.mode;
    partageRam.lock.release();

    return retour;
}

void TRam::setAlarmeHighGb(int nvHighGB)
{
    partageRam.lock.take();
    partageRam.alarmeInfo.nvHighGB = nvHighGB;
    partageRam.lock.release();
}

void TRam::setAlarmeTmpHighGb(int tmpHighGB)
{
    partageRam.lock.take();
    partageRam.alarmeInfo.tmpHighGB = tmpHighGB;
    partageRam.lock.release();
}

void TRam::setAlarmeHighPb(int nvHighPB)
{
    partageRam.lock.take();
    partageRam.alarmeInfo.nvHighPB = nvHighPB;
    partageRam.lock.release();
}

void TRam::setAlarmeTmpHighPb(int tmpHighPB)
{
    partageRam.lock.take();
    partageRam.alarmeInfo.tmpHighPB = tmpHighPB;
    partageRam.lock.release();
}

void TRam::setAlarmeHighGbTrigger(bool trigger)
{
    partageRam.lock.take();
    partageRam.alarmeInfo.alarmeHighGbTrigger = trigger;
    partageRam.lock.release();
}

void TRam::setAlarmeHighPbTrigger(bool trigger)
{
    partageRam.lock.take();
    partageRam.alarmeInfo.alarmeHighPbTrigger = trigger;
    partageRam.lock.release();
}

void TRam::setAlarmeTmpRepGb(int tmpRepGB)
{
    partageRam.lock.take();
    partageRam.alarmeInfo.tmpRepGB = tmpRepGB;
    partageRam.lock.release();
}

void TRam::setAlarmeTmpRepPb(int tmpRepPB)
{
    partageRam.lock.take();
    partageRam.alarmeInfo.tmpRepPB = tmpRepPB;
    partageRam.lock.release();
}

void TRam::setAckGB(bool ack)
{
    partageRam.lock.take();
    partageRam.alarmeInfo.ackGB = ack;
    partageRam.lock.release();
}

void TRam::setAckPB(bool ack)
{
    partageRam.lock.take();
    partageRam.alarmeInfo.ackPB = ack;
    partageRam.lock.release();
}

void TRam::setConsigneNiveauGrosBassin(double niveauPourcent)
{
    partageRam.lock.take();
    partageRam.status.consigneNiveauGrosBassin = niveauPourcent;
    partageRam.lock.release();
}

void TRam::setConsigneNiveauPetitBassin(double niveauPourcent)
{
    partageRam.lock.take();
    partageRam.status.consigneNiveauPetitBassin = niveauPourcent;
    partageRam.lock.release();
}

void TRam::setConsigneTemperaturePetitBassin(double temperature)
{
    partageRam.lock.take();
    partageRam.status.consigneTemperaturePetitBassin = temperature;
    partageRam.lock.release();
}

// Partage
TRam::partageRam_t *TRam::getPartageRam(void)
{
    partageRam_t *result = NULL;

    partageRam.lock.take();
    result = &partageRam;
    partageRam.lock.release();

    return result;
}

void TRam::setStatusRam(ram_t *status)
{
    partageRam.lock.take();
    partageRam.status = *status;
    partageRam.lock.release();
}

void TRam::getStatusRam(ram_t *status)
{
    partageRam.lock.take();
    *status = partageRam.status;
    partageRam.lock.release();
}
