#ifndef SIMULATIONWIDGET_H
#define SIMULATIONWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include "simulation.h"

class SimulationWidget : public QGraphicsView {
    Q_OBJECT
public:
    explicit SimulationWidget(ConferenceSimulation *sim, QWidget *parent = nullptr);

public slots:
    void onInteraction(int id1, int id2, bool isMatch);

private:
    void setupScene();
    ConferenceSimulation *m_sim;
    QGraphicsScene *m_scene;
    std::map<int, QGraphicsEllipseItem*> items;
};

#endif
