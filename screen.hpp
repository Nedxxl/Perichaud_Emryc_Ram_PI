#ifndef SCREEN_HPP
#define SCREEN_HPP

#define QUEUE_KERNEL  0
#define QUEUE_LOCAL   1

#define USE_QUEUE QUEUE_KERNEL
//#define USE_QUEUE QUEUE_LOCAL
#define MUT_ALLOC

#include <stdint.h>
#include <string>
#include "thread.hpp"

#if USE_QUEUE == QUEUE_KERNEL
    #include <mqueue.h>
#else
    #include "queue.hpp"
#endif

#ifdef MUT_ALLOC
	#include "mutex.hpp"
#endif

class TScreen  : public TThread
    {
    private:
	struct message_t
	    {
	    int32_t x;
	    int32_t y;
	    char *buffer;
	    };

	#if USE_QUEUE == QUEUE_KERNEL
	mqd_t queueDisplay;
	#else
	TQueue queueDisplay;
	#endif

	#ifdef MUT_ALLOC
    	TMutex mutAlloc;
	#endif

	void openDisplay(void);
	void closeDisplay(void);

    public:
	TScreen(void);
	~TScreen();

	void dispStr(int32_t x,int32_t y,char *strDebug);
	void dispStr(int32_t x,int32_t y,const char *strDebug);
	void dispStr(int32_t x,int32_t y,unsigned char *strDebug);
	void dispStr(int32_t x,int32_t y,const unsigned char *strDebug);
	void dispStr(int32_t x,int32_t y,std::string strDebug);

	void dispStr(int32_t x,int32_t y,int8_t val);
	void dispStr(int32_t x,int32_t y,int16_t val);
	void dispStr(int32_t x,int32_t y,int32_t val);

	void dispStr(int32_t x,int32_t y,uint8_t val);
	void dispStr(int32_t x,int32_t y,uint16_t val);
	void dispStr(int32_t x,int32_t y,uint32_t val);

	void dispStr(int32_t x,int32_t y,float val);
	void dispStr(int32_t x,int32_t y,double val);

	void task(void);
    };

#endif // SCREEN_HPP
