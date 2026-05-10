#include <QApplication>
#include <QThread>
#include <QSplashScreen>
#include <QObject>
#include "mainwindow.h"
#include "simulation.h"
#include "app.h"

int main(int argc, char *argv[])
{
    int nParticipants = 20; // Значение по умолчанию

    if (argc > 1)
    {
        bool ok;
        int val = QString(argv[1]).toInt(&ok);

        if (ok && val > 1) // Минимум 2 человека для общения
        {
            nParticipants = val;
        }
        else
        {
            qDebug() << "Warning: Invalid N provided. Using default (20).";
        }
    }

    QApplication a(argc, argv);

    a.setWindowIcon(App::UI::makeIcon(":/icon.png"));

    QSplashScreen *splash = App::UI::makeSplashScreen(":/splash.png");

    a.processEvents();
    QTime dieTime = QTime::currentTime().addSecs(2);

    while (QTime::currentTime() < dieTime)
    {
        a.processEvents(QEventLoop::AllEvents, 100);
    }

    ConferenceSimulation *sim = new ConferenceSimulation(nParticipants, 1000);

        // 2. Создаем окно и передаем туда симуляцию
        MainWindow w(sim);
        w.setWindowTitle(QString("Multi-threaded Party (%1 Participants)").arg(nParticipants));
        w.show();

        // Закрываем сплэш (если он есть в твоем коде выше)

        // 3. Прямо запускаем процесс.
        // Внутри sim->process() теперь вызывается QtConcurrent::run для каждого участника.
        sim->process();
    w.setWindowTitle("Conference Party");
    w.show();
    splash->finish(&w);
    //simThread->start();

    return a.exec();
}

