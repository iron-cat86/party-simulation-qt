#include "simulation.h"
#include <iostream>
#include <QThread>
#include <QDateTime>
#include <QDebug>
#include <map>

ConferenceSimulation::ConferenceSimulation(int n, int intervalMs) :
    m_interval(intervalMs), isRun(false)
{
    std::uniform_int_distribution<> dist(0, 3);
    for (int i = 1; i <= n; ++i)
    {
        roomA.push_back({i, static_cast<Interest>(dist(gen)), "A"});
    }
}

ConferenceSimulation::~ConferenceSimulation()
{}

void ConferenceSimulation::stop()
{
    isRun = false;
}

void ConferenceSimulation::process()
{
    isRun = true;

    while (isRun)
    {
        nextStep();
        QThread::msleep(m_interval);
        ++iterationCount;
    }
    emit finished();
}

void ConferenceSimulation::nextStep()
{
    QMutexLocker locker(&dataMutex);

    if (roomA.size() < 2)
    {
        buildLogString(QString("[Итерация %1]: Симуляция окончена (недостаточно людей)!").arg(iterationCount));
        isRun = false;
        emit simulationEnded();
        return;
    }

    if(roomA.size() <= 4)
    {
        bool hasPotentialMatch = false;
        for (size_t i = 0; i < roomA.size(); ++i)
        {
            for (size_t j = i + 1; j < roomA.size(); ++j)
            {
                if (roomA[i].interest == roomA[j].interest)
                {
                    hasPotentialMatch = true;
                    break;
                }
            }
        }

        if(!hasPotentialMatch)
        {
            buildLogString(QString("[Итерация %1]: Симуляция окончена (пар больше нет)").arg(iterationCount));
            isRun = false;
            emit simulationEnded();
            return;
        }
    }

    std::uniform_int_distribution<> select(0, static_cast<int>(roomA.size() - 1));
    int idx1 = select(gen);
    int idx2;
    do
    {
        idx2 = select(gen);
    }
    while (idx1 == idx2);

    Person& p1 = roomA[idx1];
    Person& p2 = roomA[idx2];

    QString log = QString("[Итерация %1]: Участник %2 с интересом %3 встретил %4 с интересом %5\n")
        .arg(iterationCount)
        .arg(p1.id).arg(p1.getInterestStr())
        .arg(p2.id).arg(p2.getInterestStr());

    bool isMatch = (p1.interest == p2.interest);
    emit interactionOccurred(p1.id, p2.id, isMatch);

    if (isMatch)
    {
        p1.room = "B";
        p2.room = "B";
        roomB.push_back(p1);
        roomB.push_back(p2);

        if (idx1 < idx2) std::swap(idx1, idx2);
        roomA.erase(roomA.begin() + idx1);
        roomA.erase(roomA.begin() + idx2);
        log += "Интересы совпали! Уходят в зал Б.\n";
    }
    else
    {
        log += "Разные интересы.\n";
    }

    log += QString("Зал А: %1, Зал Б: %2\n").arg(roomA.size()).arg(roomB.size());

    qDebug().noquote() << log;
    buildLogString(log);
}

QString ConferenceSimulation::findPersonById(int id) const
{
    QMutexLocker locker(&dataMutex);

    auto itA = std::find_if(roomA.begin(), roomA.end(), [id](const Person& p) {
        return p.id == id;
    });

    if (itA != roomA.end()) {
        return QString("ID: %1\nИнтерес: %2\nЛокация: Зал А")
                   .arg(itA->id).arg(itA->getInterestStr());
    }

    auto itB = std::find_if(roomB.begin(), roomB.end(), [id](const Person& p) {
        return p.id == id;
    });

    if (itB != roomB.end()) {
        return QString("ID: %1\nИнтерес: %2\nЛокация: Зал Б")
                   .arg(itB->id).arg(itB->getInterestStr());
    }
    return "Участник не найден";
}

void ConferenceSimulation::display(const Person& p) const
{
   qDebug().noquote() << "ID:" << p.id << "| Interest:" << p.getInterestStr() << "| Room:" << p.room;
}

void ConferenceSimulation::buildLogString(QString curLog)
{
    logString += App::Data::getCurrentTimestamp() + ": " + curLog;
}

QString ConferenceSimulation::getHistoryLog() const
{
    QMutexLocker locker(&dataMutex);
    return logString.isEmpty() ? "Лог пуст" : logString;
}

QString ConferenceSimulation::getFinalStateReport() const
{
    QMutexLocker locker(&dataMutex);

    auto countInterests = [](const std::vector<Person>& people) {
        std::map<Interest, int> counts;
        for (const auto& p : people) counts[p.interest]++;
        return QString("- Эпидемиология: %1\n- Статистика: %2\n- Клин. иссл.: %3\n- Политика: %4\n")
            .arg(counts[Interest::EPIDEMIOLOGY])
            .arg(counts[Interest::STATISTICS])
            .arg(counts[Interest::CLINICAL_TRIALS])
            .arg(counts[Interest::HEALTH_POLICY]);
    };

    QString report = "===== ИТОГОВЫЙ ОТЧЕТ =====\n\n";
    report += QString("ЗАЛ А (Остались): %1 чел.\n").arg(roomA.size()) + countInterests(roomA);
    report += QString("\nЗАЛ Б (всего людей): %1 чел. (Пары): %2\n").arg(roomB.size()).arg(roomB.size() / 2) + countInterests(roomB);
    report += QString("Всего итераций %1\n").arg(iterationCount);
    return report;
}
