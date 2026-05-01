#ifndef SIMULATION_H
#define SIMULATION_H

#include <QObject>
#include <QString>
#include <QVector> // В Qt лучше использовать QVector или std::vector
#include <vector>
#include <random>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <QDateTime>

enum class Interest
{
    EPIDEMIOLOGY,
    STATISTICS,
    CLINICAL_TRIALS,
    HEALTH_POLICY
};

struct Person {
    int id;
    Interest interest;
    QString room = "A";

    QString getInterestStr() const {
        static const QString names[] = { "Эпидемиология",
                                         "Статистика",
                                         "Клинические исследования",
                                         "Политика здравоохранения"  };
        return names[static_cast<int>(interest)];
    }
};

class ConferenceSimulation : public QObject {
    Q_OBJECT
public:
    explicit ConferenceSimulation(int n, int intervalMs);
    ~ConferenceSimulation();

    // Геттеры для UI
    const std::vector<Person>& getRoomA() const { return roomA; }
    const std::vector<Person>& getRoomB() const { return roomB; }
    QString findPersonById(int id) const;
    QString getHistoryLog() const;
    QString getFinalStateReport() const;

public slots:
    void process();
    void stop();

signals:
    void interactionOccurred(int id1, int id2, bool isMatch);
    void simulationEnded();
    void finished(); // Сигнал для завершения потока в main.cpp

private:
    void nextStep();
    void buildLogString(QString curLog);
    void display(const Person& p) const;
    QString getCurrentTimestamp() const;

    std::vector<Person> roomA;
    std::vector<Person> roomB;

    std::mt19937 gen{std::random_device{}()};
    int m_interval;
    int iterationCount = 0;
    std::atomic<bool> isRun; // Атомарный флаг для безопасности
    QString logString;
    mutable QMutex dataMutex;
};

#endif // SIMULATION_H

