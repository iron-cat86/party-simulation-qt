#include <QApplication>
#include <QThread>
#include <QSplashScreen>
#include "mainwindow.h"
#include "simulation.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString iconPath = ":/icon.png";
    QString imagePath = ":/splash.png";
    QPixmap pixmap(imagePath);

    if (pixmap.isNull())
    {
        qDebug() << "ERROR: Failed to load splash image!";
    }

    QPixmap originalPixmap(iconPath);

    if (originalPixmap.isNull())
    {
        qDebug() << "ERROR: Failed to load original image!";
    }

    QPixmap scaledIcon = originalPixmap.scaled(
        256, 256,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );

    QIcon icon(scaledIcon);

    if(icon.availableSizes().isEmpty())
    {
        qDebug()<<"ERROR: Fauld to load icon!";
    }

    a.setWindowIcon(icon);

    int desiredWidth = 800;
    int desiredHeight = 800;

    QPixmap scaledPixmap = pixmap.scaled(
        desiredWidth,
        desiredHeight,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );

    QSplashScreen splash(scaledPixmap);
    splash.show();
    splash.repaint();
    splash.raise();
    a.processEvents();
    QTime dieTime = QTime::currentTime().addSecs(2);

    while (QTime::currentTime() < dieTime)
    {
        a.processEvents(QEventLoop::AllEvents, 100);
    }

    QThread* simThread = new QThread();
    ConferenceSimulation *sim = new ConferenceSimulation(20, 1000);
    sim->moveToThread(simThread);

    QObject::connect(simThread, &QThread::started, sim, &ConferenceSimulation::process);

    QObject::connect(sim, &ConferenceSimulation::simulationEnded, simThread, &QThread::quit);

    MainWindow w(sim);
    w.setWindowTitle("Conference Party");
    w.show();
    splash.finish(&w);

    simThread->start();

    return a.exec();
}

