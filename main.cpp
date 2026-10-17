#include <QApplication>
#include "FinestraPrincipale.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Istanziamo e mostriamo la nostra finestra principale complessa
    FinestraPrincipale finestra;
    finestra.show();

    return app.exec();
}
