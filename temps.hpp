#ifndef TEMPS_HPP
#define TEMPS_HPP

#include <time.h>
#include <sys/time.h>
#include <string.h>

class TTemps
    {
    private:
	char strTemps[50];
	time_t temps;
	struct tm tmTemps;

	struct timeval tDebut,tFin;

    public:
	TTemps(void);

	char *now(void);

	void startMesure(void);
	void stopMesure(void);
	double mesure_us(void);
    };

#endif // TEMPS_HPP
