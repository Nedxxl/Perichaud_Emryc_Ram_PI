//============================================================================
// Name        : RAM
// Author      : SG
// Version     :
// Copyright   : Your copyright notice
// Description : Template Panneau RAM
//============================================================================

#include <string>

#include "clavier.hpp"
#include "screen.hpp"
#include "alarme.hpp"
#include "acquisition.hpp"
#include "control.hpp"
#include "taskMqtt.hpp"
#include "ram.hpp"
#include "version.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  char car;
  int noCpu = 0;
  bool toggle = false;
  TRam::ram_t statusRam;

  // Initialisation task Principal
  TThread::initTaskMain(SCHED_FIFO, noCpu);

  // Création Clavier,console
  TClavier *clavier = TClavier::getInstance();
  TScreen *screen = new TScreen();
  screen->start();

  screen->dispStr(1, 1, "RAM Init...");

  TRam *ram = TRam::getInstance(screen);

  sleep(1);
  ram->init();

  // Création tâches
  noCpu = 1;
  TAlarme *alarme = new TAlarme("Alarme", screen, SCHED_FIFO, 80, noCpu);
  TControl *control = new TControl("Control", screen, SCHED_FIFO, 70, noCpu);
  noCpu = 2;
  TAcquisition *acquisition = new TAcquisition("Acquisition", screen, SCHED_FIFO, 90, noCpu);
  noCpu = 3;
  TTaskMqtt *taskMqtt = new TTaskMqtt("TaskMqttRam", screen, SCHED_FIFO, 60, noCpu);

  // Démarrage tâches
  acquisition->start();
  alarme->start();
  control->start();
  taskMqtt->start();

  // Traitement tâche principale
  string message;
  message = "Template RAM (Version : ";
  message += VERSION;
  message += "   ";
  message += DATE;
  message += "   ";
  message += AUTEUR;
  message += ")";
  screen->dispStr(1, 1, message.c_str());

  sleep(2);

  do
  {
    // Traitement
    ram->getStatusRam(&statusRam);

    // ...

    // clavier
    if (clavier->kbhit())
    {
      car = clavier->getch();

      if (car == 'p')
      {
        toggle = !toggle;
        ram->setPompe(toggle);
      }
    }

    usleep(250000); // 250ms
  } while ((car != 'q') && (car != 'Q'));

  ram->setMode(1); // manuel
  ram->init();
  sleep(1);

  // Destruction tâches
  if (acquisition)
    delete acquisition;
  if (control)
    delete control;
  if (alarme)
    delete alarme;
  if (taskMqtt)
    delete taskMqtt;

  // Destruction console
  if (screen)
    delete screen;

  return 0;
}
