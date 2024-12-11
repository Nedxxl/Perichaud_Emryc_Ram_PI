#ifndef FILTRE_FENETRE_GLISSANTE_HPP
#define FILTRE_FENETRE_GLISSANTE_HPP

class TFiltreFenetreGlissante
    {
    private:
    public:
	TFiltreFenetreGlissante(int fenetre = 3);
	~TFiltreFenetreGlissante();

	double filtreGB(double valAcquisition);
    double filtrePB(double valAcquisition);
    double filtreTempFroid(double valAcquisition);
    double filtreTempChaud(double valAcquisition);
    };

#endif // FILTRE_FENETRE_GLISSANTE_HPP
