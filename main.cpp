#include <QApplication>
#include "mainwindow.h"
#include "simulation.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 1. Создаем симуляцию на 20 человек
    ConferenceSimulation *sim = new ConferenceSimulation(20, 1000);
    QObject::connect(sim, &ConferenceSimulation::finished, sim, &QObject::deleteLater);
    // 2. Создаем окно и передаем ему симуляцию для отрисовки
    MainWindow w(sim);
    w.setWindowTitle("Conference Party - Команда №5");
    w.show();

    // 3. Запускаем внутренний таймер симуляции (1 секунда из ТЗ)
    sim->start();

    return a.exec();
}
