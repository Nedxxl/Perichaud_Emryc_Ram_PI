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
  bool pompe = false;
  bool mode = false;
  bool eec = false;
  bool eef = false;
  int consigneGb = 0;
  int consignePb = 0;
  int consigneTempPb = 0;
  int valveGb = 0;
  int valvePb = 0;
  int valveEauFroide = 0;
  int valveEauChaude = 0;

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
    //mode = statusRam.mode;
    pompe = statusRam.pompe;
    eec = statusRam.eauChaude;
    eef = statusRam.eauFroide;
    consigneGb = statusRam.consigneNiveauGrosBassin;
    consignePb = statusRam.consigneNiveauPetitBassin;
    consigneTempPb = statusRam.consigneTemperaturePetitBassin;
    valveGb = statusRam.valveGrosBassin;
    valvePb = statusRam.valvePetitBassin;
    valveEauFroide = statusRam.valveEauFroide;
    valveEauChaude = statusRam.valveEauChaude;
    // ...

    // clavier
    if (clavier->kbhit())
    {
      car = clavier->getch();

      if (car == 'q')
      {
        mode = !mode;
        ram->setMode(mode);
      }

      if (mode == 0)
      {
        switch (car)
        {
        case 'w':
          pompe = !pompe;
          ram->setPompe(pompe);
          break;
        case 'e':
          eec = !eec;
          ram->setEauChaude(eec);
          break;
        case 'r':
          eef = !eef;
          ram->setEauFroide(eef);
          break;
        case 'a':
          consigneGb += 5;
          ram->setConsigneNiveauGrosBassin(consigneGb);
          break;
        case 'A':
          consigneGb -= 5;
          ram->setConsigneNiveauGrosBassin(consigneGb);
          break;
        case 's':
          consignePb += 5;
          ram->setConsigneNiveauPetitBassin(consignePb);
          break;
        case 'S':
          consignePb -= 5;
          ram->setConsigneNiveauPetitBassin(consignePb);
          break;
        case 'd':
          consigneTempPb += 5;
          ram->setConsigneTemperaturePetitBassin(consigneTempPb);
          break;
        case 'D':
          consigneTempPb -= 5;
          ram->setConsigneTemperaturePetitBassin(consigneTempPb);
          break;
        default:
          break;
        }
      }
      else if (mode == 1)
      {
        switch (car)
        {
        case 'z':
          valveGb += 5;
          ram->setValveGrosBassin(100-valveGb);
          break;
        case 'Z':
          valveGb -= 5;
          ram->setValveGrosBassin(100-valveGb);
          break;
        case 'x':
          valvePb += 5;
          ram->setValvePetitBassin(valvePb);
          break;
        case 'X':
          valvePb -= 5;
          ram->setValvePetitBassin(valvePb);
          break;
        case 'c':
          valveEauFroide += 5;
          ram->setValveEauFroide(valveEauFroide);
          break;
        case 'C':
          valveEauFroide -= 5;
          ram->setValveEauFroide(valveEauFroide);
          break;
        case 'v':
          valveEauChaude += 5;
          ram->setValveEauChaude(valveEauChaude);
          break;
        case 'V':
          valveEauChaude -= 5;
          ram->setValveEauChaude(valveEauChaude);
          break;
        default:
          break;        
        }
      }
    }
    screen->dispStr(1, 9, "Mode (q): " + to_string(mode) + "  ");
    screen->dispStr(1, 10, "Pompe (w): " + to_string(pompe) + "  ");
    screen->dispStr(1, 11, "EEC (e): " + to_string(eec) + "  ");
    screen->dispStr(1, 12, "EEF (r): " + to_string(eef) + "  ");
    screen->dispStr(1, 13, "ConsNivGB (a = +5)(A = -5): " + to_string(consigneGb) + "  ");
    screen->dispStr(1, 14, "ConsNivPB (s = +5)(S = -5): " + to_string(consignePb) + "  ");
    screen->dispStr(1, 15, "ConsTmpPB (d = +5)(D = -5): " + to_string(consigneTempPb) + "  ");
    screen->dispStr(1, 16, "ValveGB (z = +5)(Z = -5): " + to_string(valveGb) + "  ");
    screen->dispStr(1, 17, "ValvePB (x = +5)(X = -5): " + to_string(valvePb) + "  ");
    screen->dispStr(1, 18, "ValveEF (c = +5)(C = -5): " + to_string(valveEauFroide) + "  ");
    screen->dispStr(1, 19, "ValveEC (v = +5)(V = -5): " + to_string(valveEauChaude) + "  ");

    usleep(250000); // 250ms
  } while ((car != '|') && (car != '~'));

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
