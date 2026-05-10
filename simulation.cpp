#include "simulation.h"
#include <iostream>
#include <QThread>
#include "simulation.h"
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>

ConferenceSimulation::ConferenceSimulation(int n, int intervalMs) :
    m_interval(intervalMs), isRun(false)
{
    std::uniform_int_distribution<> dist(0, 3);
    for (int i = 1; i <= n; ++i)
    {
        // Используем указатели, так как QMutex нельзя копировать в векторе
        roomA.push_back(new Person{i, static_cast<Interest>(dist(gen)), "A"});
    }
}

void ConferenceSimulation::process()
{
    isRun = true;

    QThreadPool::globalInstance()->setMaxThreadCount(roomA.size() + 2);
    // Запускаем каждого участника в пуле потоков
    for (Person* p : roomA) {
        QtConcurrent::run([this, p]() {
            this->participantLifeCycle(p);
        });
    }
}

void ConferenceSimulation::checkOnFinish()
{
    if (roomA.size() <= 4)
    {
        bool hasPotentialMatch = false;

        for (size_t i = 0; i < roomA.size(); ++i)
        {
            for (size_t j = i + 1; j < roomA.size(); ++j)
            {
                if (roomA[i]->interest == roomA[j]->interest)
                {
                    hasPotentialMatch = true;
                    break;
                }
            }

            if (hasPotentialMatch) break;
        }

        if (!hasPotentialMatch)
        {
            isRun = false; // Гасим все потоки
            buildLogString("Симуляция окончена: подходящих пар больше нет.");
            emit simulationEnded();
        }
    }
}

// Жизненный цикл отдельного участника (потока)
void ConferenceSimulation::participantLifeCycle(Person* self)
{
    while (isRun)
    {
        QThread::msleep(m_interval);
        if(self->id == 1) checkOnFinish();

        if(isRun && self->room == "A" && !self->isBusy)
        {
            Person* partner = nullptr;

            {
                QMutexLocker locker(&dataMutex); // Блокируем общие списки для поиска

                std::uniform_int_distribution<> select(0, roomA.size()-1);
                partner = roomA[select(gen)];

                if(partner == nullptr || partner->isBusy || partner->id == self->id)
                {
                    continue;
                }
                else
                {
                    self->isBusy = true;
                    partner->isBusy = true;
                }
            }

            Person* first = (self->id < partner->id) ? self : partner;
            Person* second = (self->id < partner->id) ? partner : self;

            QMutexLocker lock1(&first->mutex);
            QMutexLocker lock2(&second->mutex);

            bool isMatch = (self->interest == partner->interest);
            emit interactionOccurred(self->id, partner->id, isMatch);

            QString logMsg; // Буфер для сообщения

            if (isRun && isMatch)
            {
                QMutexLocker locker(&dataMutex);
                self->room = "B";
                partner->room = "B";

                roomB.push_back(self);
                roomB.push_back(partner);

                roomA.erase(std::remove(roomA.begin(), roomA.end(), self), roomA.end());
                roomA.erase(std::remove(roomA.begin(), roomA.end(), partner), roomA.end());
                self->isBusy = false;
                partner->isBusy = false;

                logMsg = QString("Участники %1 и %2 нашли друг друга! интерес %3\n")
                                .arg(self->id).arg(partner->id).arg(self->getInterestStr());
            }
            else
            {
                QMutexLocker locker(&dataMutex);
                self->isBusy = false;
                partner->isBusy = false;
                logMsg = QString("Участники %1 - интерес %2, и %3 - интерес %4: не совпали!\n")
                                 .arg(self->id).arg(self->getInterestStr()).arg(partner->id).arg(partner->getInterestStr());
            }

            if (!logMsg.isEmpty()) buildLogString(logMsg);
        }
    }
}

ConferenceSimulation::~ConferenceSimulation()
{
    stop();
}

void ConferenceSimulation::stop()
{
    isRun = false;
}

QString ConferenceSimulation::findPersonById(int id) const
{
    QMutexLocker locker(&dataMutex);

    auto itA = std::find_if(roomA.begin(), roomA.end(), [id](const Person* p) {
        return p->id == id;
    });

    if (itA != roomA.end()) {
        return QString("ID: %1\nИнтерес: %2\nЛокация: Зал А")
                   .arg((*itA)->id).arg((*itA)->getInterestStr());
    }

    auto itB = std::find_if(roomB.begin(), roomB.end(), [id](const Person* p) {
        return p->id == id;
    });

    if (itB != roomB.end()) {
        return QString("ID: %1\nИнтерес: %2\nЛокация: Зал Б")
                   .arg((*itB)->id).arg((*itB)->getInterestStr());
    }
    return "Участник не найден";
}

void ConferenceSimulation::display(const Person* p) const
{
   qDebug().noquote() << "ID:" << p->id << "| Interest:" << p->getInterestStr() << "| Room:" << p->room;
}

void ConferenceSimulation::buildLogString(QString curLog)
{
    QMutexLocker locker(&dataMutex);
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

    auto countInterests = [](const std::vector<Person*>& people) {
        std::map<Interest, int> counts;
        for (const auto& p : people) counts[p->interest]++;
        return QString("- Эпидемиология: %1\n- Статистика: %2\n- Клин. иссл.: %3\n- Политика: %4\n")
            .arg(counts[Interest::EPIDEMIOLOGY])
            .arg(counts[Interest::STATISTICS])
            .arg(counts[Interest::CLINICAL_TRIALS])
            .arg(counts[Interest::HEALTH_POLICY]);
    };

    QString report = "===== ИТОГОВЫЙ ОТЧЕТ =====\n\n";
    report += App::Data::getCurrentTimestamp();
    report += QString("ЗАЛ А (Остались): %1 чел.\n").arg(roomA.size()) + countInterests(roomA);
    report += QString("\nЗАЛ Б (всего людей): %1 чел. (Пары): %2\n").arg(roomB.size()).arg(roomB.size() / 2) + countInterests(roomB);
   // report += QString("Всего итераций %1\n").arg(iterationCount);
    return report;
}
