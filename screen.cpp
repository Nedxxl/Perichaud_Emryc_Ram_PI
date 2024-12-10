#include "screen.hpp"

using namespace std;

TScreen::TScreen(void) : TThread("Display",NULL,SCHED_FIFO,10)
    {
    openDisplay();

    system("clear");
    }

TScreen::~TScreen()
    {
    closeDisplay();
    }

void TScreen::dispStr(int32_t x,int32_t y,char *strDebug)
    {
    #if USE_QUEUE == QUEUE_KERNEL
    message_t message;

    message.buffer = NULL;
    #else
    TQueue::node_t *nodeMessage = NULL;
    message_t *message = NULL;
    #endif


    #ifdef MUT_ALLOC
    mutAlloc.take();
    #endif

    #if USE_QUEUE == QUEUE_KERNEL
    message.buffer = new char[strlen(strDebug) + 1];
    #else
    nodeMessage = new TQueue::node_t;
    message = new message_t;
    if(message)
	message->buffer = new char[strlen(strDebug) + 1];
    #endif

    #ifdef MUT_ALLOC
    mutAlloc.release();
    #endif

    #if USE_QUEUE == QUEUE_KERNEL
    message.x = x;
    message.y = y;
    #else
    if(message)
	{
	message->x = x;
	message->y = y;
	}
    #endif

    #if USE_QUEUE == QUEUE_KERNEL
    if(message.buffer != NULL)
    #else
    if( (message != NULL) && (message->buffer != NULL) && (nodeMessage != NULL) )
    #endif
	{

	#if USE_QUEUE == QUEUE_KERNEL
	    strcpy(message.buffer,strDebug);
	    mq_send(queueDisplay,(char *)&message,sizeof(message_t),0);
	    // mq_timedsend pourrait être utilisé
	#else
	    strcpy(message->buffer,strDebug);
	    queueDisplay.queue_push(nodeMessage,message);
	#endif
	}
    else
	{
	#ifdef MUT_ALLOC
	mutAlloc.take();
	#endif

	#if USE_QUEUE == QUEUE_KERNEL
	if(message.buffer)
	    delete message.buffer;
	#else
	if(nodeMessage)
	    delete nodeMessage;
	if((message) && (message->buffer))
	    delete message->buffer;
	if(message)
	    delete message;
	#endif

	#ifdef MUT_ALLOC
	mutAlloc.release();
	#endif
	}
    }

void TScreen::dispStr(int32_t x,int32_t y,const char *strDebug)
    {
    dispStr(x,y,(char *)strDebug);
    }

void TScreen::dispStr(int32_t x,int32_t y,unsigned char *strDebug)
    {
    dispStr(x,y,(char *)strDebug);
    }

void TScreen::dispStr(int32_t x,int32_t y,const unsigned char *strDebug)
    {
    dispStr(x,y,(char *)strDebug);
    }

void TScreen::dispStr(int32_t x,int32_t y,string strDebug)
	{
    dispStr(x,y,strDebug.c_str());
	}

void TScreen::dispStr(int32_t x,int32_t y, int8_t val)
	{
	char strDebug[20];

	sprintf(strDebug,"%d",val);
    dispStr(x,y,(char *)strDebug);
	}

void TScreen::dispStr(int32_t x,int32_t y, int16_t val)
	{
	char strDebug[20];

	sprintf(strDebug,"%d",val);
    dispStr(x,y,(char *)strDebug);
	}

void TScreen::dispStr(int32_t x,int32_t y, int32_t val)
	{
	char strDebug[20];

	sprintf(strDebug,"%d",val);
    dispStr(x,y,(char *)strDebug);
	}

void TScreen::dispStr(int32_t x,int32_t y, uint8_t val)
	{
	char strDebug[20];

	sprintf(strDebug,"%d",val);
    dispStr(x,y,(char *)strDebug);
	}

void TScreen::dispStr(int32_t x,int32_t y, uint16_t val)
	{
	char strDebug[20];

	sprintf(strDebug,"%d",val);
    dispStr(x,y,(char *)strDebug);
	}

void TScreen::dispStr(int32_t x,int32_t y, uint32_t val)
	{
	char strDebug[20];

	sprintf(strDebug,"%d",val);
    dispStr(x,y,(char *)strDebug);
	}

void TScreen::dispStr(int32_t x,int32_t y, float val)
	{
	char strDebug[20];

	sprintf(strDebug,"%.2f",val);
    dispStr(x,y,(char *)strDebug);
	}

void TScreen::dispStr(int32_t x,int32_t y, double val)
	{
	char strDebug[20];

	sprintf(strDebug,"%.2lf",val);
    dispStr(x,y,(char *)strDebug);
	}

void TScreen::openDisplay(void)
    {
    #if USE_QUEUE == QUEUE_KERNEL

    struct mq_attr attrQueue;

    attrQueue.mq_flags = 0;
    attrQueue.mq_maxmsg = 16;                   // 16 messages maximun
    attrQueue.mq_msgsize = sizeof(message_t);   // dimension (octet) des messages
    attrQueue.mq_curmsgs = 0;

	if( (queueDisplay = mq_open("/qDisplay",O_RDWR)) >= 0)
		{
		closeDisplay();
		}
	if( (queueDisplay = mq_open("/qDisplay",O_CREAT | O_RDWR,0,&attrQueue)) < 0)
		{
		fprintf(stderr,"Error: code from mq_open (Thread : %s)\n",thread.name);
		exit(1);
		}

    #endif
    }

void TScreen::closeDisplay(void)
    {
    #if USE_QUEUE == QUEUE_KERNEL
    mq_close(queueDisplay);
    mq_unlink("/qDisplay");
    #endif
    }

void TScreen::task(void)
    {
    #if USE_QUEUE == QUEUE_KERNEL
    message_t message;
    message.buffer = NULL;
    #else
    message_t *message;
    TQueue::node_t *nodeMessage;
    #endif

    // synchronisation démarrage tâche
    signalStart();

    while(1)
		{
		#if USE_QUEUE == QUEUE_KERNEL
		if(queueDisplay >= 0)
			{
			mq_receive(queueDisplay,(char *)&message,sizeof(message_t),NULL);
			if(message.buffer != NULL)
				{
				printf("%c[%d;%dH%s",0x1B,message.y,message.x,message.buffer);
				fflush(stdout);
				}
			}
		#else
			nodeMessage = queueDisplay.queue_pop();
			message = (message_t *)nodeMessage->state;

			if( (message != NULL) && (message->buffer != NULL) )
			{
			printf("%c[%d;%dH%s",0x1B,message->y,message->x,message->buffer);
			fflush(stdout);
			}
		#endif

		#ifdef MUT_ALLOC
		mutAlloc.take();
		#endif

		#if USE_QUEUE == QUEUE_KERNEL
		if(message.buffer)
			delete message.buffer;
		#else
		if( (message) && (message->buffer) )
			delete (message->buffer);
		if(message)
			delete message;
		if(nodeMessage)
			delete nodeMessage;
		#endif

		#ifdef MUT_ALLOC
		mutAlloc.release();
		#endif
		}
}

