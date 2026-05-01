#ifndef SIMULATION_H
#define SIMULATION_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QString>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>

enum class Interest { EPIDEMIOLOGY, STATISTICS, CLINICAL_TRIALS, HEALTH_POLICY };

struct Person 
{
    int id;
    Interest interest;
    std::string room = "A";

    std::string getInterestStr() const {
        static const std::string names[] = { "Epidemiology", "Statistics", "Clinical Trials", "Health Policy" };
        return names[static_cast<int>(interest)];
    }
};

class ConferenceSimulation: public QThread 
{
    Q_OBJECT
public:
    explicit ConferenceSimulation(int n, int intervalMs);
    ~ConferenceSimulation();
    
    void printState() const;
    QString findPersonById(int id) const;
    void display(const Person& p) const;
    void stop(){isRunning = false;}
    const std::vector<Person>& getRoomA() const { return roomA; }
    const std::vector<Person>& getRoomB() const { return roomB; }
protected:
    void run() override;
public slots:
    void nextStep();
signals:
    void interactionOccurred(int id1, int id2, bool isMatch);
    void simulationEnded();

private:
    int m_interval;
    std::vector<Person> roomA;
    std::vector<Person> roomB;
    std::mt19937 gen{std::random_device{}()};
    bool isRunning=true;
};
#endif
