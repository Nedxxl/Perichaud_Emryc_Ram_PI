#include "clavier.hpp"

TClavier::TClavier(void) : timeoutMs(10)
    {
    reconfigureTerminal();
    }

TClavier::~TClavier()
    {
    restaureTerminal();
    }

/* cette méthode indique si une touche a été activée
 * sur le terminal
 * retourne 0 si aucune touche appuiyée,1 autrement
 */
int16_t TClavier::kbhit(void)
    {
    int16_t retour;
    fd_set setRead;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = (timeoutMs * 1000);
    FD_ZERO(&setRead);
    FD_SET(fileno(stdin),&setRead);
    // attendre la reception de caractere sur stdin ou un timeout de timeoutMs
    select(FD_SETSIZE,&setRead,NULL,NULL,&tv);
    if(FD_ISSET(fileno(stdin),&setRead))
      retour = 1;
    else
      retour = 0;

    return retour;
    }

int16_t TClavier::getch(void)
    {
    return getchar();
    }

/* cette méthode reconfigure le terminal, et emmagasine
 * la configuration initiale dans la structure oldTermios
 */
int16_t TClavier::reconfigureTerminal(void)
    {
    int16_t retour = 0;
    struct termios newTermios;

    if(tcgetattr(fileno(stdin),&oldTermios) == -1)
      {
      perror("tcgetattr");
      retour = -1;
      }

    newTermios = oldTermios;
    newTermios.c_lflag &= ~ICANON;   // mode non-canonique
    newTermios.c_lflag &= ~ECHO;     // mode sans echo

    newTermios.c_cc[VMIN]  = 1;              // nombre minimun de carateres lors d'une lecture en mode non-canonique

    if(tcsetattr(fileno(stdin),TCSANOW,&newTermios) == -1)
      {
      perror("tcsetattr");
      retour = -1;
      }

    return retour;
    }

/* cette méthode restaure le terminal avec la
 * configuration emmagasinée dans la structure oldTermios
 */
int16_t TClavier::restaureTerminal(void)
    {
    return tcsetattr(fileno(stdin),TCSANOW,&oldTermios);
    }
