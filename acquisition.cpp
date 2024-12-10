/*
 * acquisition.cpp
 *
 *  Created on: 2018-11-23
 *  Author: SG
 */

#include "acquisition.hpp"

TAcquisition::TAcquisition(const char *name,void *shared,int policy,int priority,int noCpu) :
                                                                            TThread(name,shared,policy,priority,noCpu)
    {
    screen = (TScreen *)shared;

    ram = TRam::getInstance(screen);
    partageRam = ram->getPartageRam();

    screen->dispStr(1,4,"Acquisition : ");
    }

TAcquisition::~TAcquisition()
    {
    syncDestruction.take();
    }

void TAcquisition::task(void)
    {
    char strCar[2] = {'-','\0'};

    // synchronisation démarrage tâche
    signalStart();

    while(1)
	{
	//traitement
	syncDestruction.take();

	ram->acquisition();

	ram->format();     // format donnee brute

	// déclenche tâche alarme
	partageRam->synAlarme.release();
	if(ram->getMode() != 1) // mode auto
	    {
	    // déclenche tâche control
	    partageRam->synControl.release();
	    }

	syncDestruction.release();

	if(strCar[0] == '|')
		strCar[0] = '-';
	else
		strCar[0] = '|';
	screen->dispStr(15,4,strCar);

	usleep(50000);   // 50 ms
	}
    }
