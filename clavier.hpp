#ifndef CLAVIER_HPP
#define CLAVIER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <stdint.h>

#include "singleton.hpp"

class TClavier : public TSingleton<TClavier>
    {
	friend class TSingleton<TClavier>;
    private:
		struct termios oldTermios;
		const int timeoutMs;		// temps d'attende d'un caractère au clavier

		int16_t reconfigureTerminal(void);
		int16_t restaureTerminal(void);

		TClavier(void);
		~TClavier();

		TClavier(const TClavier &) = delete;
		TClavier &operator=(const TClavier &) = delete;

    public:
		int16_t kbhit(void);
		int16_t getch(void);
    };

#endif // CLAVIER_HPP
