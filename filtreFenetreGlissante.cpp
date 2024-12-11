#include "filtreFenetreGlissante.hpp"
#include <string.h>

TFiltreFenetreGlissante::TFiltreFenetreGlissante(int fenetre)
    {
    }

TFiltreFenetreGlissante::~TFiltreFenetreGlissante()
    {
    }

double TFiltreFenetreGlissante::filtreGB(double valAcquisition)
    {
    double valFiltre;

    // Traitement
    valFiltre = valAcquisition;
    static double buffer[11] = {0};
    static int index = 0;
    static int count = 0;

    buffer[index] = valAcquisition;
    index = (index + 1) % 11;
    if (count < 11) {
        count++;
    }

    double sum = 0;
    for (int i = 0; i < count; i++) {
        sum += buffer[i];
    }

    valFiltre = sum / count;

    return valFiltre;
    }

double TFiltreFenetreGlissante::filtrePB(double valAcquisition)
    {
    double valFiltre;

    // Traitement
    valFiltre = valAcquisition;
    static double buffer[11] = {0};
    static int index = 0;
    static int count = 0;

    buffer[index] = valAcquisition;
    index = (index + 1) % 11;
    if (count < 11) {
        count++;
    }

    double sum = 0;
    for (int i = 0; i < count; i++) {
        sum += buffer[i];
    }

    valFiltre = sum / count;

    return valFiltre;
    }

double TFiltreFenetreGlissante::filtreTempFroid(double valAcquisition)
    {
        double valFiltre;

        static double buffer[11] = {0};
        static int index = 0;
        static int count = 0;

        buffer[index] = valAcquisition;
        index = (index + 1) % 11;
        if (count < 11) {
            count++;
        }

        double sum = 0;
        for (int i = 0; i < count; i++) {
            sum += buffer[i];
        }

        valFiltre = sum / count;

        return valFiltre;
        }

double TFiltreFenetreGlissante::filtreTempChaud(double valAcquisition)
    {
    double valFiltre;

    static double buffer[11] = {0};
    static int index = 0;
    static int count = 0;

    buffer[index] = valAcquisition;
    index = (index + 1) % 11;
    if (count < 11) {
        count++;
    }

    double sum = 0;
    for (int i = 0; i < count; i++) {
        sum += buffer[i];
    }

    valFiltre = sum / count;

    return valFiltre;
    }
