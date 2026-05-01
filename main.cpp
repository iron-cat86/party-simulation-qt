#include <QApplication>
#include <QThread>
#include "mainwindow.h"
#include "simulation.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 1. Создаем поток-"двигатель"
    QThread* simThread = new QThread();

    // 2. Создаем объект логики (Воркер).
    ConferenceSimulation *sim = new ConferenceSimulation(20, 1000);

    // 3. Теперь вся логика sim будет жить в simThread
    sim->moveToThread(simThread);

    // 4. КОННЕКТЫ УПРАВЛЕНИЯ
    QObject::connect(simThread, &QThread::started, sim, &ConferenceSimulation::process);

    // Когда симуляция сигналит об окончании — поток должен остановиться
    QObject::connect(sim, &ConferenceSimulation::simulationEnded, simThread, &QThread::quit);

    // 5. Создаем окно и передаем ему симуляцию
    MainWindow w(sim);
    w.setWindowTitle("Conference Party - SUE Moscow Metro R&D");
    w.show();

    // 6. ЗАПУСК
    simThread->start();

    return a.exec();
}

