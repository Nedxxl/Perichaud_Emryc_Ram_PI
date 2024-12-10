#ifndef COM_HPP
#define COM_HPP

#include "thread.hpp"
#include <termio.h>
#include <mqueue.h>
#include "pid.hpp"

class TCom : public TThread
    {
    public:
	enum baudrate_t
	    {
	    b300,
	    b1200,
	    b2400,
	    b4800,
	    b9600,
	    b19200,
	    b38400,
	    b57600,
	    b115200,
	    b230400,
	    };

	enum parity_t
	    {
	    pNONE,
	    pPAIR,
	    pIMPAIR,
	    };

	enum dimData_t
	    {
	    dS5,
	    dS6,
	    dS7,
	    dS8,
	    };
    private:
	struct message_t
	    {
	    int32_t length;
	    char *buffer;
	    };

	char nomQueue[20];
	const int32_t timeoutRx_ms;

	mqd_t queueCom;

	int16_t openSerie(const char *nameCom,baudrate_t baudRate,parity_t parite,dimData_t dimData);
	void closeSerie(void);
    protected:
	int16_t com;
	bool signalTimeout;
	threads_t threadTx;
	static void *handlerTx(void *arg);
	static TCom *ptrThisTx;
    public:
	TCom(const char *name,void *shared,int priority,baudrate_t baudRate = b115200,
		                          parity_t parite = pNONE,dimData_t dimData = dS8,int32_t timeoutRxMs = 25);
	virtual ~TCom();

	virtual void start(void);

	virtual void rxCar(unsigned char car) = 0;
	virtual void rxTimeout(void) = 0;

	// rx
	void task(void);
	// tx
	void taskTx(void);

	void sendTx(void *buffer,int32_t length);
	void setSignalTimeout(bool onOff);
    };

#endif // COM_HPP
