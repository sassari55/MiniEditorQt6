//
// Created by giambattista on 16/10/26.
//

#ifndef FINESTRAPRINCIPALE_H
#define FINESTRAPRINCIPALE_H

#include <QMainWindow>
#include <QTextEdit>
#include <QString>
#include <QAction>
#include <QCloseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPageLayout>

class FinestraPrincipale : public QMainWindow {
    Q_OBJECT

public:
    explicit FinestraPrincipale(QWidget *parent = nullptr);
    ~FinestraPrincipale() = default;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void azioneNuovoFile();
    void azioneApriFile();
    bool azioneSalvaFile();
    void azioneInformazioni();

    void cambiaFont();
    void cambiaColoreTesto();
    void impostaGrassetto(bool checked);
    void impostaCorsivo(bool checked);
    void impostaSottolineato(bool checked);
    void aggiornaStatoFormattazione();

    void mostraNascondiBarraRicerca();
    void eseguiRicercaTesto(const QString &testo);
    void aggiornaConteggioParole();
    void cambiaOrientamentoStampa();
    void azioneStampa();

    bool controllaSalvataggioModifiche();

private:
    void creaMenuEToolbar(); // <--- Nome corretto allineato al file .cpp
    void creaBarraRicerca();

    QTextEdit *areaTesto;
    QString percorsoFileCorrente;

    QWidget *widgetRicerca;
    QLineEdit *inputRicerca;
    QLabel *lblStatistiche;

    QPageLayout::Orientation orientamentoPagina;

    QAction *actGrassetto;
    QAction *actCorsivo;
    QAction *actSottolineato;
};

#endif // FINESTRAPRINCIPALE_H
