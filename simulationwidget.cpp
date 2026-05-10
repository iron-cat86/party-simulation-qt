#include "simulationwidget.h"
#include <QVariantAnimation>
#include <QPen>
#include <QBrush>
#include <QThread>
#include <QApplication> // Для проверки потока

SimulationWidget::SimulationWidget(ConferenceSimulation *sim, QWidget *parent)
    : QGraphicsView(parent), m_sim(sim)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setupScene();
}

void SimulationWidget::setupScene()
{
    m_scene->setSceneRect(0, 0, 750, 550);
    m_scene->addRect(10, 10, 350, 530, QPen(Qt::black), QBrush(QColor(240, 240, 240)));
    m_scene->addRect(390, 10, 350, 530, QPen(Qt::black), QBrush(QColor(220, 255, 220)));

    auto textA = m_scene->addText("Зал А (Вечеринка)");
    textA->setPos(120, 15);
    auto textB = m_scene->addText("Зал Б (Коктейли)");
    textB->setPos(500, 15);

    std::map<Interest, QColor> colors = {
        {Interest::EPIDEMIOLOGY, Qt::red},
        {Interest::STATISTICS, Qt::blue},
        {Interest::CLINICAL_TRIALS, Qt::green},
        {Interest::HEALTH_POLICY, Qt::yellow}
    };

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> posX(30, 300);
    std::uniform_int_distribution<> posY(50, 480);

    // ВАЖНО: Теперь работаем с указателями на Person*, как в новом Simulation
    for (Person* p : m_sim->getRoomA())
    {
        auto* circle = m_scene->addEllipse(0, 0, 30, 30, QPen(Qt::black), QBrush(colors[p->interest]));
        circle->setPos(posX(gen), posY(gen));
        auto* idText = m_scene->addText(QString::number(p->id));
        idText->setParentItem(circle);
        idText->setPos(5, 2);
        items[p->id] = circle;
    }
}

void SimulationWidget::onInteraction(int id1, int id2, bool isMatch)
{
    // 1. Берем айтемы
    auto* item1 = items[id1];
    auto* item2 = items[id2];
    if (!item1 || !item2) return;

    // 2. Если они уже "в домике" (Зал Б), не трогаем
    if (item1->data(0) == "in_b" || item2->data(0) == "in_b") return;

    QPointF start1 = item1->pos();
    QPointF start2 = item2->pos();
    QPointF meetingPoint = (start1 + start2) / 2.0;

    auto *anim = new QVariantAnimation(this);
    anim->setDuration(500);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);

    connect(anim, &QVariantAnimation::valueChanged, [item1, item2, start1, start2, meetingPoint](const QVariant &value) {
        qreal t = value.toReal();
        item1->setPos(start1 + (meetingPoint - start1) * t);
        item2->setPos(start2 + (meetingPoint + QPointF(15,0) - start2) * t);
    });

    if (isMatch)
    {
        // Помечаем сразу, чтобы другие анимации не подхватили
        item1->setData(0, "in_b");
        item2->setData(0, "in_b");

        connect(anim, &QVariantAnimation::finished, [item1, item2]() {
            // ВОТ ТУТ ФИКС: Жесткие координаты внутри прямоугольника Зала Б
            // (Зал Б у тебя начинается с 390, ширина 350 -> до 740)
            static std::mt19937 gen(std::random_device{}());
            std::uniform_int_distribution<> posX(410, 680);
            std::uniform_int_distribution<> posY(50, 480);

            QPointF endB(posX(gen), posY(gen));

            // Принудительно ставим их в Зал Б и "красим"
            item1->setPos(endB);
            item2->setPos(endB + QPointF(20, 20));
            item1->setPen(QPen(Qt::magenta, 3));
            item2->setPen(QPen(Qt::magenta, 3));
            item1->setZValue(1);
            item2->setZValue(1);
        });
    }

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void SimulationWidget::forceMoveToB(int id) {
    auto* item = items[id];
    if (item && item->data(0) != "in_b") {
        item->setData(0, "in_b");
        std::mt19937 gen(std::random_device{}());
        item->setPos(410 + (gen() % 280), 50 + (gen() % 400));
        item->setPen(QPen(Qt::magenta, 3));
    }
}
