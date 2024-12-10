#include "com_1.hpp"

TCom1::TCom1(const char *name,void *sharedScreen,void *shared,int priority,baudrate_t baudRate,
                                            parity_t parite,dimData_t dimData,int timeoutRxMs) :
                                                           TCom(name,NULL,priority,baudRate,parite,dimData,timeoutRxMs)
    {
    screen = (TScreen *)sharedScreen;
    partageRam = (TRam::partageRam_t *)shared;

    firstByteReponse = true;
    len = 0;
    timeout = 0;
    cptTimeout = 0;
    memset(buffer,0,sizeof buffer);

    resultReq  = 0;
    repRequest = 0; //init semaphore

    screen->dispStr(1,3,"Com:");
    screen->dispStr(30,3,"(Erreur communication : 000000)");
    }

TCom1::~TCom1()
    {
    }

// valeur de retour (0 -> normal, 1 -> timeout, 2 -> erreur communication)
int TCom1::waitRepRequest(void)
    {
    if(com != -1)
	repRequest.take();
    else
	{
	resultReq = 1; //timeout
	cptTimeout++;
	}

    return resultReq;
    }

unsigned int TCom1::getCptTimeout(void)
    {
    return cptTimeout;
    }

void TCom1::takeCom1(void)
    {
    lockCom.take();
    }

void TCom1::releaseCom1(void)
    {
    lockCom.release();
    }

void TCom1::rxCar(unsigned char car)
    {
    if(firstByteReponse)
	{
	firstByteReponse = false;
	len = 0;
	timeout = 0;
	memset(buffer,0,sizeof buffer);
	}

    buffer[len] = car;
    len++;
    if(len > 255)
	len = 255;
    }

void TCom1::rxTimeout(void)
   {
   if(len > 0)
	{
	if(buffer[0] != '>')
	    resultReq = 2; // erreur
	else
	    {
	    resultReq = 0;
	    if( (len > 1) && (buffer[1] == '+') ) // module AI
		{
		double aiValue[8];

		sscanf(&buffer[1],"%lf",&aiValue[0]);  // AI1
		sscanf(&buffer[8],"%lf",&aiValue[1]);  // AI2
		sscanf(&buffer[15],"%lf",&aiValue[2]); // AI3
		sscanf(&buffer[22],"%lf",&aiValue[3]); // AI4
		sscanf(&buffer[29],"%lf",&aiValue[4]); // AI5
		sscanf(&buffer[36],"%lf",&aiValue[5]); // AI6
		sscanf(&buffer[43],"%lf",&aiValue[6]); // AI7
		sscanf(&buffer[50],"%lf",&aiValue[7]); // AI8

		memcpy(partageRam->rawDataAI,aiValue,sizeof aiValue);
		}
	    else if(len >= 6) // module DI
		{
		unsigned int di;
		sscanf(&buffer[1],"%X",&di);
		partageRam->lock.take();
		partageRam->status.debordementGrosBassin = di & 0x01;           // DI1
		partageRam->status.debordementPetitBassin = (di >> 1) & 0x01;   // DI2
		partageRam->lock.release();
		}
	    }

	len = 0;
	timeout = 0;
	firstByteReponse = true;
	signalTimeout = false;
	repRequest.release();    // syn end request
	}
    else
	{
	// timeout
	if(++timeout > 1000)  // 10 s
	    {
	    resultReq = 1;  //timeout
	    len = 0;
	    timeout = 0;
	    cptTimeout++;
	    firstByteReponse = true;
	    signalTimeout = false;
	    repRequest.release();  // syn end request
	    }
	}
    }

void TCom1::getName(std::string &name)
    {
    FILE *file;
    char tty[50];

    // obtenir le nom du port serie
    file = fopen("com.def","r");
    if(file != NULL)
    	{
    	fgets(tty,sizeof tty,file);
    	fclose(file);
    	tty[strlen(tty) - 1] = '\0';  // elimine le CR
    	}
    else
    	strcpy(tty,"/dev/ttyS0");

    name = tty;
    }

