#include "simulation.h"
#include <iostream>

ConferenceSimulation::ConferenceSimulation(int n, int intervalMs):
    m_interval(intervalMs) 
{
    std::uniform_int_distribution<> dist(0, 3);
    for (int i = 1; i <= n; ++i) {
        roomA.push_back({i, static_cast<Interest>(dist(gen)), "A"});
    }
}

ConferenceSimulation::~ConferenceSimulation()
{
    isRunning = false;
    wait();
}

void ConferenceSimulation::run() 
{
    isRunning = true;
    while (isRunning) {
        if (roomA.size() < 2) {
            emit simulationEnded();
            return;
        }
        nextStep();
        msleep(m_interval);
    }
}

void ConferenceSimulation::nextStep() 
{
    // 1. Проверка: есть ли кого выбирать
    if (roomA.size() < 2) {
        emit simulationEnded();
        return;
    }

    if(roomA.size() <= 4) {
        // Проверка на наличие потенциальных пар (чтобы не зациклиться)
        bool hasPotentialMatch = false;

        for (size_t i = 0; i < roomA.size(); ++i) {
            for (size_t j = i + 1; j < roomA.size(); ++j) {
                if (roomA[i].interest == roomA[j].interest) {
                    hasPotentialMatch = true;
                    break;
                }
            }
        }

        if(!hasPotentialMatch) {
            emit simulationEnded();
            return;
        }
    }
    // 2. Выбор двух случайных уникальных участников
    std::uniform_int_distribution<> select(0, roomA.size() - 1);
    int idx1 = select(gen);
    int idx2;
    do { 
        idx2 = select(gen); 
    } while (idx1 == idx2);

    // Достаем ссылки для сравнения
    Person& p1 = roomA[idx1];
    display(p1);
    Person& p2 = roomA[idx2];
    display(p2);

    bool isMatch = (p1.interest == p2.interest);

    // 3. Отправляем сигнал интерфейсу (MainWindow это подхватит для анимации)
    emit interactionOccurred(p1.id, p2.id, isMatch);

    // 4. Если интересы совпали — переводим в комнату Б
    if (isMatch) 
    {
        p1.room = "B";
        p2.room = "B";
        
        roomB.push_back(p1);
        roomB.push_back(p2);

        // Безопасное удаление из вектора А
        if (idx1 < idx2) std::swap(idx1, idx2);
        roomA.erase(roomA.begin() + idx1);
        roomA.erase(roomA.begin() + idx2);
    }
    printState();
    // Если не совпали — ничего не делаем, они просто остаются в roomA
}


void ConferenceSimulation::printState() const 
{
    std::cout << "Состояние: [Комната А: " << roomA.size() << " чел.] "
              << "[Комната Б: " << roomB.size() << " чел.]\n\n";
}

QString ConferenceSimulation::findPersonById(int id) const
{
    // Ищем в комнате А
    for (const auto& p : roomA) {
        if (p.id == id) {
            return QString("ID: %1\nИнтерес: %2\nЛокация: Зал А")
                .arg(p.id).arg(QString::fromStdString(p.getInterestStr()));
        }
    }
    // Ищем в комнате Б
    for (const auto& p : roomB) {
        if (p.id == id) {
            return QString("ID: %1\nИнтерес: %2\nЛокация: Зал Б (Уже в паре!)")
                .arg(p.id).arg(QString::fromStdString(p.getInterestStr()));
        }
    }
    return QString(""); // Не нашли
}

void ConferenceSimulation::display(const Person& p) const 
{
    std::cout << ">>> Результат поиска: ID " << p.id << " | Интерес: " 
              << p.getInterestStr() << " | Локация: Комната " << p.room << std::endl;
}
