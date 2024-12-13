#ifndef ALARME_HPP
#define ALARME_HPP

#include "thread.hpp"
#include "ram.hpp"
#include "screen.hpp"
#include "temps.hpp"

class TAlarme : public TThread
    {
    private:
	TScreen *screen;
	TRam *ram;
	TRam::partageRam_t *partageRam;
	TTemps tempsGB;
	TTemps tempsPB;

    public:
	TAlarme(const char *name,void *shared,int policy,int priority,int noCpu = -1);
	~TAlarme();

	// tâche control
	void task(void);
    };

#endif //ALARME_HPP
