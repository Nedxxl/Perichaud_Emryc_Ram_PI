#ifndef CONTROL_HPP
#define CONTROL_HPP

#include "thread.hpp"
#include "screen.hpp"
#include "ram.hpp"
#include "filtreFenetreGlissante.hpp"
#include "pid.hpp"

class TControl : public TThread
    {
    private:
	TScreen *screen;
	TRam *ram;
	TRam::partageRam_t *partageRam;

    public:
	TControl(const char *name,void *shared,int policy,int priority,int noCpu = -1);
	~TControl();

	// tâche control
	void task(void);
    };

#endif //CONTROL_HPP
