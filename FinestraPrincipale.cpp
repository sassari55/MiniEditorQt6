//
// Created by giambattista on 16/10/26.
//
#include "FinestraPrincipale.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QIcon>
#include <QFontDialog>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <QDateTime>
#include <QRegularExpression>
#include <QFileInfo>

FinestraPrincipale::FinestraPrincipale(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Editor Avanzato Pro Qt6 (Nuovo Documento)");
    resize(900, 650);

    // Valore iniziale orientamento di stampa (Verticale)
    orientamentoPagina = QPageLayout::Portrait;

    // Layout Principale Fisso per integrare la barra di ricerca in alto senza sovrapposizioni
    QWidget *contenitoreCentrale = new QWidget(this);
    QVBoxLayout *layoutPrincipale = new QVBoxLayout(contenitoreCentrale);
    layoutPrincipale->setContentsMargins(0, 0, 0, 0);
    layoutPrincipale->setSpacing(0);

    // Crea la barra di ricerca (inizialmente nascosta)
    creaBarraRicerca();
    layoutPrincipale->addWidget(widgetRicerca);

    areaTesto = new QTextEdit(this);
    layoutPrincipale->addWidget(areaTesto);
    setCentralWidget(contenitoreCentrale);

    // Inizializza i widget della barra di stato per i conteggi
    lblStatistiche = new QLabel("Parole: 0  |  Caratteri: 0   ", this);
    statusBar()->addPermanentWidget(lblStatistiche);
    statusBar()->showMessage("Pronto.");

    creaMenuEToolbar();

    // Connessioni segnali
    connect(areaTesto, &QTextEdit::currentCharFormatChanged, this, &FinestraPrincipale::aggiornaStatoFormattazione);
    connect(areaTesto, &QTextEdit::textChanged, this, &FinestraPrincipale::aggiornaConteggioParole);

    connect(areaTesto->document(), &QTextDocument::modificationChanged, this, [this](bool modificato) {
        setWindowModified(modificato);
        QString nomeVisualizzato = percorsoFileCorrente.isEmpty() ? "Nuovo Documento" : QFileInfo(percorsoFileCorrente).fileName();
        QString titolo = "Editor Avanzato Pro Qt6 - " + nomeVisualizzato;
        if (modificato) titolo += " [*]";
        setWindowTitle(titolo);
    });
}

void FinestraPrincipale::creaBarraRicerca() {
    widgetRicerca = new QWidget(this);
    QHBoxLayout *layoutRicerca = new QHBoxLayout(widgetRicerca);
    layoutRicerca->setContentsMargins(5, 5, 5, 5);

    QLabel *lblCerca = new QLabel("Cerca nel testo:", widgetRicerca);
    inputRicerca = new QLineEdit(widgetRicerca);
    inputRicerca->setPlaceholderText("Digita il testo e premi Invio...");

    QPushButton *btnChiudiRicerca = new QPushButton("X", widgetRicerca);
    btnChiudiRicerca->setFixedWidth(30);

    layoutRicerca->addWidget(lblCerca);
    layoutRicerca->addWidget(inputRicerca);
    layoutRicerca->addWidget(btnChiudiRicerca);

    widgetRicerca->setVisible(false); // Nascondi all'avvio

    // Esegui la ricerca alla pressione del tasto Invio o al cambio di testo
    connect(inputRicerca, &QLineEdit::textChanged, this, &FinestraPrincipale::eseguiRicercaTesto);
    connect(btnChiudiRicerca, &QPushButton::clicked, this, [this]() { widgetRicerca->setVisible(false); });
}
void FinestraPrincipale::creaMenuEToolbar() {
    QMenuBar *barraMenu = menuBar();
    QMenu *menuFile = barraMenu->addMenu("&File");
    QMenu *menuFormato = barraMenu->addMenu("F&ormato");
    QMenu *menuStrumenti = barraMenu->addMenu("&Strumenti");

    QToolBar *barraFile = addToolBar("File");
    QToolBar *barraFormato = addToolBar("Formato");
    QToolBar *barraAvanzata = addToolBar("Strumenti Avanzati");

    // --- AZIONI FILE ---
    QAction *actNuovo = new QAction(QIcon(":/icone/nuovo.png"), "&Nuovo", this);
    actNuovo->setShortcut(QKeySequence::New);
    connect(actNuovo, &QAction::triggered, this, &FinestraPrincipale::azioneNuovoFile);
    menuFile->addAction(actNuovo);
    barraFile->addAction(actNuovo);

    QAction *actApri = new QAction(QIcon(":/icone/apri.png"), "&Apri...", this);
    actApri->setShortcut(QKeySequence::Open);
    connect(actApri, &QAction::triggered, this, &FinestraPrincipale::azioneApriFile);
    menuFile->addAction(actApri);
    barraFile->addAction(actApri);

    QAction *actSalva = new QAction(QIcon(":/icone/salva.png"), "&Salva", this);
    actSalva->setShortcut(QKeySequence::Save);
    connect(actSalva, &QAction::triggered, this, &FinestraPrincipale::azioneSalvaFile);
    menuFile->addAction(actSalva);
    barraFile->addAction(actSalva);

    // Azione Orientamento Stampa (Interruttore Orizzontale/Verticale)
    QAction *actOrientamento = new QAction(QIcon(":/icone/orientamento.png"), "Cambia Orientamento Stampa", this);
    connect(actOrientamento, &QAction::triggered, this, &FinestraPrincipale::cambiaOrientamentoStampa);
    menuFile->addAction(actOrientamento);
    barraFile->addAction(actOrientamento);

    QAction *actStampa = new QAction(QIcon(":/icone/stampa.png"), "&Stampa...", this);
    actStampa->setShortcut(QKeySequence::Print);
    connect(actStampa, &QAction::triggered, this, &FinestraPrincipale::azioneStampa);
    menuFile->addAction(actStampa);
    barraFile->addAction(actStampa);

    menuFile->addSeparator();
    QAction *actEsci = new QAction(QIcon(":/icone/esci.png"), "&Esci", this);
    actEsci->setShortcut(QKeySequence::Quit);
    connect(actEsci, &QAction::triggered, this, &FinestraPrincipale::close);
    menuFile->addAction(actEsci);
    barraFile->addAction(actEsci);

    // --- STRUMENTI AVANZATI (RICERCA) ---
    QAction *actCerca = new QAction(QIcon(":/icone/ricerca.png"), "Cerca testo...", this);
    actCerca->setShortcut(QKeySequence::Find);
    connect(actCerca, &QAction::triggered, this, &FinestraPrincipale::mostraNascondiBarraRicerca);
    menuStrumenti->addAction(actCerca);
    barraAvanzata->addAction(actCerca);

    // --- AZIONI FORMATO ---
    QAction *actFont = menuFormato->addAction("Scegli &Font...");
    connect(actFont, &QAction::triggered, this, &FinestraPrincipale::cambiaFont);
    QAction *actColore = menuFormato->addAction("Scegli &Colore...");
    connect(actColore, &QAction::triggered, this, &FinestraPrincipale::cambiaColoreTesto);
    menuFormato->addSeparator();

    actGrassetto = new QAction("&Grassetto", this); actGrassetto->setCheckable(true);
    connect(actGrassetto, &QAction::triggered, this, &FinestraPrincipale::impostaGrassetto);
    barraFormato->addAction(actGrassetto); menuFormato->addAction(actGrassetto);

    actCorsivo = new QAction("&Corsivo", this); actCorsivo->setCheckable(true);
    connect(actCorsivo, &QAction::triggered, this, &FinestraPrincipale::impostaCorsivo);
    barraFormato->addAction(actCorsivo); menuFormato->addAction(actCorsivo);

    actSottolineato = new QAction("&Sottolineato", this); actSottolineato->setCheckable(true);
    connect(actSottolineato, &QAction::triggered, this, &FinestraPrincipale::impostaSottolineato);
    barraFormato->addAction(actSottolineato); menuFormato->addAction(actSottolineato);

    // --- AZIONI ALLINEAMENTO ---
    QAction *actAlSinistra = new QAction(QIcon(":/icone/allinea_sinistra.png"), "Allinea a Sinistra", this);
    connect(actAlSinistra, &QAction::triggered, this, [this]() { areaTesto->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute); });
    barraFormato->addAction(actAlSinistra);

    QAction *actAlCentro = new QAction(QIcon(":/icone/allinea_centro.png"), "Centra", this);
    connect(actAlCentro, &QAction::triggered, this, [this]() { areaTesto->setAlignment(Qt::AlignCenter); });
    barraFormato->addAction(actAlCentro);

    QAction *actAlDestra = new QAction(QIcon(":/icone/allinea_destra.png"), "Allinea a Destra", this);
    connect(actAlDestra, &QAction::triggered, this, [this]() { areaTesto->setAlignment(Qt::AlignRight | Qt::AlignAbsolute); });
    barraFormato->addAction(actAlDestra);

    QAction *actAlGiustificato = new QAction(QIcon(":/icone/allinea_giustificato.png"), "Giustifica", this);
    connect(actAlGiustificato, &QAction::triggered, this, [this]() { areaTesto->setAlignment(Qt::AlignJustify); });
    barraFormato->addAction(actAlGiustificato);

    QMenu *menuAiuto = barraMenu->addMenu("&Aiuto");
    QAction *actInfo = menuAiuto->addAction("&Informazioni su...");
    connect(actInfo, &QAction::triggered, this, &FinestraPrincipale::azioneInformazioni);
}

void FinestraPrincipale::cambiaFont() {
    bool ok;
    QFont f = QFontDialog::getFont(&ok, areaTesto->textCursor().charFormat().font(), this);
    if (ok) areaTesto->setCurrentFont(f);
}

void FinestraPrincipale::cambiaColoreTesto() {
    QColor c = QColorDialog::getColor(areaTesto->textCursor().charFormat().foreground().color(), this);
    if (c.isValid()) areaTesto->setTextColor(c);
}

void FinestraPrincipale::impostaGrassetto(bool checked) { areaTesto->setFontWeight(checked ? QFont::Bold : QFont::Normal); }
void FinestraPrincipale::impostaCorsivo(bool checked) { areaTesto->setFontItalic(checked); }
void FinestraPrincipale::impostaSottolineato(bool checked) { areaTesto->setFontUnderline(checked); }
void FinestraPrincipale::aggiornaStatoFormattazione() {
    actGrassetto->setChecked(areaTesto->fontWeight() == QFont::Bold);
    actCorsivo->setChecked(areaTesto->fontItalic());
    actSottolineato->setChecked(areaTesto->fontUnderline());
}
void FinestraPrincipale::aggiornaConteggioParole() {
    QString testo = areaTesto->toPlainText();
    int caratteri = testo.length();

    static const QRegularExpression regex("\\b\\w+\\b");
    int parole = 0;
    auto matchIterator = regex.globalMatch(testo);
    while (matchIterator.hasNext()) {
        matchIterator.next();
        parole++;
    }

    lblStatistiche->setText(QString("Parole: %1  |  Caratteri: %2   ").arg(parole).arg(caratteri));
}

void FinestraPrincipale::mostraNascondiBarraRicerca() {
    bool visibile = !widgetRicerca->isVisible();
    widgetRicerca->setVisible(visibile);
    if (visibile) {
        inputRicerca->setFocus();
        inputRicerca->selectAll();
    }
}

void FinestraPrincipale::eseguiRicercaTesto(const QString &testo) {
    if (testo.isEmpty()) return;
    bool trovato = areaTesto->find(testo);
    if (!trovato) {
        areaTesto->moveCursor(QTextCursor::Start);
        areaTesto->find(testo);
    }
}

void FinestraPrincipale::cambiaOrientamentoStampa() {
    if (orientamentoPagina == QPageLayout::Portrait) {
        orientamentoPagina = QPageLayout::Landscape;
        statusBar()->showMessage("Orientamento di stampa: Orizzontale (Landscape)", 3000);
    } else {
        orientamentoPagina = QPageLayout::Portrait;
        statusBar()->showMessage("Orientamento di stampa: Verticale (Portrait)", 3000);
    }
}

void FinestraPrincipale::azioneStampa() {
    QPrinter stampante(QPrinter::HighResolution);
    stampante.setPageOrientation(orientamentoPagina);

    QPrintDialog finestraStampa(&stampante, this);
    finestraStampa.setWindowTitle("Stampa Documento Avanzata");

    if (finestraStampa.exec() == QDialog::Accepted) {
        QPainter painter;
        if (!painter.begin(&stampante)) {
            QMessageBox::warning(this, "Errore", "Impossibile avviare la stampa.");
            return;
        }

        QTextDocument *doc = areaTesto->document();
        QRect rectPagina = stampante.pageRect(QPrinter::DevicePixel).toRect();

        QString nomeFileStampa = percorsoFileCorrente.isEmpty() ? "Nuovo Documento Anonimo" : QFileInfo(percorsoFileCorrente).fileName();
        painter.setFont(QFont("Helvetica", 10));
        painter.drawText(rectPagina.left() + 50, rectPagina.top() + 100, "Documento: " + nomeFileStampa);
        painter.drawText(rectPagina.right() - 200, rectPagina.top() + 100, QDateTime::currentDateTime().toString("dd/MM/yyyy"));

        painter.drawLine(rectPagina.left() + 50, rectPagina.top() + 130, rectPagina.right() - 50, rectPagina.top() + 130);
        painter.drawLine(rectPagina.left() + 50, rectPagina.bottom() - 130, rectPagina.right() - 50, rectPagina.bottom() - 130);
        painter.drawText(rectPagina.left() + 50, rectPagina.bottom() - 100, "Sviluppato con Qt6 su AlmaLinux 9");
        painter.drawText(rectPagina.right() - 150, rectPagina.bottom() - 100, "Pagina 1 di 1");

        doc->drawContents(&painter, QRectF(rectPagina.left() + 50, rectPagina.top() + 180, rectPagina.width() - 100, rectPagina.height() - 360));
        painter.end();
        statusBar()->showMessage("Inviato alla stampante.", 3000);
    }
}

bool FinestraPrincipale::controllaSalvataggioModifiche() {
    if (!areaTesto->document()->isModified()) return true;
    QMessageBox::StandardButton r = QMessageBox::warning(this, "Salva", "Vuoi salvare le modifiche?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (r == QMessageBox::Save) return azioneSalvaFile();
    return (r != QMessageBox::Cancel);
}

void FinestraPrincipale::closeEvent(QCloseEvent *event) { if (controllaSalvataggioModifiche()) event->accept(); else event->ignore(); }
void FinestraPrincipale::azioneNuovoFile() { if (controllaSalvataggioModifiche()) { areaTesto->clear(); percorsoFileCorrente.clear(); areaTesto->document()->setModified(false); } }

void FinestraPrincipale::azioneApriFile() {
    if (!controllaSalvataggioModifiche()) return;
    QString n = QFileDialog::getOpenFileName(this, "Apri", "", "File di testo (*.txt);;Pagine HTML (*.html);;Tutti i file (*)");
    if (!n.isEmpty()) {
        QFile f(n);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            if (n.endsWith(".html", Qt::CaseInsensitive)) areaTesto->setHtml(in.readAll());
            else areaTesto->setPlainText(in.readAll());
            f.close();
            percorsoFileCorrente = n;
            areaTesto->document()->setModified(false);
        }
    }
}

bool FinestraPrincipale::azioneSalvaFile() {
    QString n = percorsoFileCorrente;
    if (n.isEmpty()) {
        n = QFileDialog::getSaveFileName(this, "Salva", "", "Pagine HTML (*.html);;File di testo (*.txt);;Tutti i file (*)");
        if (n.isEmpty()) return false;
    }
    QFile f(n);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f);
    if (n.endsWith(".html", Qt::CaseInsensitive)) out << areaTesto->toHtml();
    else out << areaTesto->toPlainText();
    f.close();
    percorsoFileCorrente = n;
    areaTesto->document()->setModified(false);
    return true;
}

void FinestraPrincipale::azioneInformazioni() { QMessageBox::about(this, "Info", "Versione Finale Completa."); }

