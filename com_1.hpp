#ifndef COM1_HPP
#define COM1_HPP

#include "com.hpp"
#include "sem.hpp"
#include "ram.hpp"
#include "screen.hpp"

class TCom1 : public TCom
    {
    private:
	char buffer[256];
	bool firstByteReponse;
	unsigned int len;
	unsigned int timeout;
	unsigned int cptTimeout;

	int resultReq;           // 0 -> normal, 1 -> timeout, 2 -> erreur communication
	TSemaphore repRequest;
	TMutex lockCom;

	TRam::partageRam_t *partageRam;
	TScreen *screen;
    public:
	TCom1(const char *name,void *sharedScreen,void *shared,int priority,baudrate_t baudRate = b115200,
                                parity_t parite = pNONE,dimData_t dimData = dS8,int timeoutRxMs = 25);
	~TCom1();

	// valeur de retour (0 -> normal, 1 -> timeout, 2 -> erreur communication)
	int waitRepRequest(void);
	unsigned int getCptTimeout(void);

	void takeCom1(void);
	void releaseCom1(void);

	virtual void rxCar(unsigned char car);
	virtual void rxTimeout(void);

	static void getName(std::string &name);
    };


#endif //COM1_HPP
