#ifndef SIMULATION_H
#define SIMULATION_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
static std::atomic<bool> isRun{false};
enum class Interest { EPIDEMIOLOGY, STATISTICS, CLINICAL_TRIALS, HEALTH_POLICY };

struct Person 
{
    int id;
    Interest interest;
    QString room = "A";

    QString getInterestStr() const {
        static const QString names[] =
        {"эпидемиология", "статистика", "клинические исследования", "политика в области здравоохранения"};
        //"Epidemiology", "Statistics", "Clinical Trials", "Health Policy" };
        return names[static_cast<int>(interest)];
    }
};

class ConferenceSimulation: public QThread 
{
    Q_OBJECT
public:
    explicit ConferenceSimulation(int n, int intervalMs);
    ~ConferenceSimulation();

    const std::vector<Person>& getRoomA() const { return roomA; }
    const std::vector<Person>& getRoomB() const { return roomB; }
    void stop();
    QString findPersonById(int id) const;
    QString getFinalStateReport() const;
    const QString getHistoryLog() const {return logString;}
private:
    void printState(QString log) const;
    void display(const Person& p) const;
    void buildLogString(QString curLog);
    QString getCurrentTimestamp() const;
protected:
    void run() override;
public slots:
    void nextStep();
signals:
    void interactionOccurred(int id1, int id2, bool isMatch);
    void simulationEnded();

private:
    mutable QMutex dataMutex;
    int m_interval;
    std::vector<Person> roomA;
    std::vector<Person> roomB;
    std::mt19937 gen{std::random_device{}()};
    QString logString;
};
#endif
