#include "control.hpp"

TControl::TControl(const char *name,void *shared,int policy,int priority,int noCpu) :
                                                      TThread(name,shared,policy,priority,noCpu)
    {
    screen = (TScreen *)shared;

    ram = TRam::getInstance(screen);
    partageRam = ram->getPartageRam();

    screen->dispStr(1,5,"Control :");
    }

TControl::~TControl()
    {
    }

void TControl::task(void)
    {
    // variable locale
    char strCar[2] = {'-','\0'};
    TPid pidGros(4.5,1.11111,0.05,0.0,100.0);
    TPid pidPetit(2.0,0.75,0.05,0.0,100.0);
    TPid pidTempFroid(10.0,0.5,0.05,0.0,100.0);
    TPid pidTempChaud(10.0,0.5,0.05,0.0,100.0);
    double valueGB;
    double valuePB;
    double valueTempFroid;
    double valueTempChaud;
    double filtreGbValue;
    double filtrePbValue;
    double filtreTempFroidValue;
    double filtreTempChaudValue;

    // synchronisation démarrage tâche
    signalStart();

    while(1)
	{
	// attendre acquisition complété
	partageRam->synControl.take();

	//traitement
    if(ram->getMode() == 0)
    {
        filtreGbValue = filtre.filtreGB(partageRam->status.niveauGrosBassin);
        valueGB = pidGros.compute(partageRam->status.consigneNiveauGrosBassin, filtreGbValue);
        ram->setValveGrosBassin(100-valueGB);
        ram->setFiltreGB(filtreGbValue);
        filtrePbValue = filtre.filtrePB(partageRam->status.niveauPetitBassin);
        valuePB = pidPetit.compute(partageRam->status.consigneNiveauPetitBassin, filtrePbValue);
        ram->setValvePetitBassin(valuePB);
        ram->setFiltrePB(filtrePbValue);
        //temperature
        filtreTempFroidValue = filtre.filtreTempFroid(partageRam->status.temperaturePetitBassin);
        valueTempFroid = pidTempFroid.compute(partageRam->status.consigneTemperaturePetitBassin, filtreTempFroidValue);
        ram->setValveEauFroide(valueTempFroid);
        ram->setFiltreTempFroid(filtreTempFroidValue);
        filtreTempChaudValue = filtre.filtreTempChaud(partageRam->status.temperatureGrosBassin);
        valueTempChaud = pidTempChaud.compute(partageRam->status.consigneTemperaturePetitBassin, filtreTempChaudValue);
        ram->setValveEauChaude(valueTempChaud);
        ram->setFiltreTempChaud(filtreTempChaudValue);
    }


	if(strCar[0] == '|')
		strCar[0] = '-';
	else
		strCar[0] = '|';
	screen->dispStr(11,5,strCar);
	}
    }
