/*
 * acquisition.hpp
 *
 *  Created on: 2018-11-23
 *  Author: SG
 */

#ifndef ACQUISITION_HPP_
#define ACQUISITION_HPP_

#include "thread.hpp"
#include "screen.hpp"
#include "ram.hpp"

class TAcquisition : public TThread
    {
    private:
	TScreen *screen;
	TRam *ram;
	TRam::partageRam_t *partageRam;
	TMutex syncDestruction;

    public:
	TAcquisition(const char *name,void *shared,int policy,int priority,int noCpu = -1);
	~TAcquisition();

	// tâche control
	void task(void);
    };

#endif /* ACQUISITION_HPP_ */
