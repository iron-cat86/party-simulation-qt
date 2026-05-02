#include <QApplication>
#include <QThread>
#include <QSplashScreen>
#include "mainwindow.h"
#include "simulation.h"

QIcon makeIcon(QString iconPath)
{
    QPixmap originalPixmap(iconPath);

    if (originalPixmap.isNull())
    {
        qDebug() << "ERROR: Failed to load original image for icon!";
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
    return icon;
}

QSplashScreen* makeSplashScreen(QString imagePath)
{
    QPixmap pixmap(imagePath);

    if (pixmap.isNull())
    {
        qDebug() << "ERROR: Failed to load splash image!";
    }

    int desiredWidth = 800;
    int desiredHeight = 800;

    QPixmap scaledPixmap = pixmap.scaled(
        desiredWidth,
        desiredHeight,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );
    QSplashScreen *splash = new QSplashScreen(scaledPixmap);
    splash->show();
    splash->repaint();
    splash->raise();
    return splash;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QIcon icon = makeIcon(":/icon.png");
    a.setWindowIcon(icon);

    QSplashScreen *splash = makeSplashScreen(":/splash.png");

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

