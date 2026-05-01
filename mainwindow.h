#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QLineEdit>
#include <QPushButton>
#include <map>
#include "simulation.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(ConferenceSimulation *sim);
    ~MainWindow();

private:
    void setupScene();
    void setupButton();

public slots:
    void onInteraction(int id1, int id2, bool isMatch);

private:
    ConferenceSimulation* m_sim;
    QGraphicsScene *scene;
    QGraphicsView *view;
    std::map<int, QGraphicsEllipseItem*> items;

    QLineEdit *idInput;
    QPushButton *findButton;
    QPushButton *logButton;
    QPushButton *finalReportButton;

    QMenu *helpMenu;
    QAction *aboutAction;
};

#endif // MAINWINDOW_H

