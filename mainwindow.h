#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <map>
#include "simulation.h"
#include "simulationwidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(ConferenceSimulation *sim);
    ~MainWindow();

private:
    void setupButtons();
    void setHelpMenu();
    void setGraphics();
    void setConnects();
    void setFindEdit();
    void setFindButton();
    void setLogButton();
    void setFinalReportButton();

private:
    ConferenceSimulation* m_sim;
    std::map<int, QGraphicsEllipseItem*> items;

    QHBoxLayout *hLayout;
    QLineEdit *idInput;
    QPushButton *findButton;
    QPushButton *logButton;
    QPushButton *finalReportButton;

    QMenu *helpMenu;
    QAction *aboutAction;
    SimulationWidget *simWidget;
};

#endif // MAINWINDOW_H

