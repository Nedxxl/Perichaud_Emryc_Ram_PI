#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <pthread.h>

class TMutex
    {
    private:
	pthread_mutex_t mutex;
    public:
	TMutex(void);
	~TMutex();

	void take(void);
	void release(void);
    };

#endif // MUTEX_HPP
