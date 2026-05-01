#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <map>
#include "simulation.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(ConferenceSimulation *sim);
    ~MainWindow();
private:
    void setupScene();
    void setupButton();
public slots:
    // Тот самый слот, который будет двигать кружочки
    void onInteraction(int id1, int id2, bool isMatch);

private:
    ConferenceSimulation* m_sim;
    QGraphicsScene *scene;
    QGraphicsView *view;
    std::map<int, QGraphicsEllipseItem*> items; // Быстрый доступ к кружку по ID
    QLineEdit *idInput;
    QPushButton *findButton;
    QPushButton *logButton;
    QPushButton *finalReportButton;
};
