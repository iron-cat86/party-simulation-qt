#include "simulation.h"
#include <iostream>

ConferenceSimulation::ConferenceSimulation(int n, int intervalMs):
    m_interval(intervalMs)
{
    this->setParent(nullptr);
    std::uniform_int_distribution<> dist(0, 3);
    for (int i = 1; i <= n; ++i) {
        roomA.push_back({i, static_cast<Interest>(dist(gen)), "A"});
    }
}

ConferenceSimulation::~ConferenceSimulation()
{
    stop();
    quit();
    wait();
}

void ConferenceSimulation::stop()
{
    isRun = false;
}

void ConferenceSimulation::run() 
{
    isRun = true;
    while (isRun)
    {
        nextStep();
        msleep(m_interval);
    }
}

void ConferenceSimulation::nextStep() 
{
    // 1. Проверка: есть ли кого выбирать
    if (roomA.size() < 2) {
        buildLogString("Симуляция окончена!");
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
            buildLogString("Симуляция окончена!");
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
    QString log = QString("Участник %1 подходит к участнику %2\n").arg(idx1).arg(idx2);

    // Достаем ссылки для сравнения
    Person& p1 = roomA[idx1];
    display(p1);
    Person& p2 = roomA[idx2];
    display(p2);
    log += QString("Интересы: %1 и %2 соответственно\n").arg(p1.getInterestStr())
                                                        .arg(p2.getInterestStr());

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
        log += "Интересы совпали, уходят в гостинную\n";
    }
    else
        log += "Интересы не совпали\n";
    log += QString("В комнате А %1 участников, в комнате Б %2 участников\n").arg(roomA.size()).arg(roomB.size());
    printState(log);
    buildLogString(log);
    // Если не совпали — ничего не делаем, они просто остаются в roomA
}


void ConferenceSimulation::printState(QString log) const
{
    QMutexLocker locker(&dataMutex);
    qDebug()<<log;
}

QString ConferenceSimulation::findPersonById(int id) const
{
    QMutexLocker locker(&dataMutex);
    // Ищем в комнате А
    for (const auto& p : roomA) {
        if (p.id == id) {
            return QString("ID: %1\nИнтерес: %2\nЛокация: Зал А")
                .arg(p.id).arg(p.getInterestStr());
        }
    }
    // Ищем в комнате Б
    for (const auto& p : roomB) {
        if (p.id == id) {
            return QString("ID: %1\nИнтерес: %2\nЛокация: Зал Б (Уже в паре!)")
                .arg(p.id).arg(p.getInterestStr());
        }
    }
    return QString("Не нашли");
}

void ConferenceSimulation::display(const Person& p) const 
{
    QMutexLocker locker(&dataMutex);
    qDebug() << ">>> Результат поиска: ID " << p.id << " | Интерес: "
             << p.getInterestStr() << " | Локация: Комната " << p.room;
}

QString ConferenceSimulation::getCurrentTimestamp() const
{
    // Формат: День.Месяц.Год Часы:Минуты:Секунды
    return QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");
}

void ConferenceSimulation::buildLogString(QString curLog)
{
    QMutexLocker locker(&dataMutex);
    logString += getCurrentTimestamp();
    logString += ": ";
    logString += curLog;
    logString += "\n";
}

QString ConferenceSimulation::getFinalStateReport() const {
    // Лямбда для подсчета интересов в конкретном списке
    auto countInterests = [](const std::vector<Person>& people) {
        std::map<Interest, int> counts;
        for (const auto& p : people) {
            counts[p.interest]++;
        }

        QString detail;
        detail += QString("- Эпидемиология: %1\n").arg(counts[Interest::EPIDEMIOLOGY]);
        detail += QString("- Статистика: %1\n").arg(counts[Interest::STATISTICS]);
        detail += QString("- Клин. исследования: %1\n").arg(counts[Interest::CLINICAL_TRIALS]);
        detail += QString("- Политика здравоохр.: %1\n").arg(counts[Interest::HEALTH_POLICY]);
        return detail;
    };

    QString report = "===== ИТОГОВЫЙ ОТЧЕТ СИМУЛЯЦИИ =====\n\n";

    report += QString("ЗАЛ А (Остались без пары: %1 чел.)\n").arg(roomA.size());
    report += countInterests(roomA);

    report += "\n------------------------------------\n";

    report += QString("ЗАЛ Б (Сформировано пар: %1)\n").arg(roomB.size() / 2);
    report += countInterests(roomB);

    report += "\n====================================";

    return report;
}
