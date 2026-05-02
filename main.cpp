#include <QApplication>
#include <QThread>
#include <QSplashScreen>
#include <QObject>
#include "mainwindow.h"
#include "simulation.h"
#include "app.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(App::UI::makeIcon(":/icon.png"));

    QSplashScreen *splash = App::UI::makeSplashScreen(":/splash.png");

    a.processEvents();
    QTime dieTime = QTime::currentTime().addSecs(2);

    while (QTime::currentTime() < dieTime)
    {
        a.processEvents(QEventLoop::AllEvents, 100);
    }

    QThread* simThread = new QThread();
    ConferenceSimulation *sim = new ConferenceSimulation(5, 1000);
    sim->moveToThread(simThread);
    QObject::connect(simThread, &QThread::started, sim, &ConferenceSimulation::process);
    QObject::connect(sim, &ConferenceSimulation::simulationEnded, simThread, &QThread::quit);

    MainWindow w(sim);
    w.setWindowTitle("Conference Party");
    w.show();
    splash->finish(&w);
    simThread->start();

    return a.exec();
}

