#include <gtest/gtest.h>
#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QTimer>
#include "../simulationwidget.h"
#include "../simulation.h"

class GraphicsTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        static int argc = 1;
        static char* argv[] = {(char*)"test"};
        if (!qApp) new QApplication(argc, argv);
    }

    void SetUp() override
    {
        sim = new ConferenceSimulation(10, 100); // Создаем 10 человек
        sw = new SimulationWidget(sim);
    }

    void TearDown() override
    {
        delete sw;
        delete sim;
    }

    ConferenceSimulation* sim;
    SimulationWidget* sw;
};

// 1. Тест: создались ли фоновые прямоугольники (залы)
TEST_F(GraphicsTest, SceneBackgroundRects)
{
    auto items = sw->scene()->items();
    int rectCount = 0;

    for (auto* item : items)
    {
        if (item->type() == QGraphicsRectItem::Type) rectCount++;
    }
    // У нас 2 зала (прямоугольника)
    EXPECT_GE(rectCount, 2);
}

// 2. Тест: соответствие количества кружочков количеству людей
TEST_F(GraphicsTest, ParticipantsCountOnScene)
{
    auto allItems = sw->scene()->items();
    int ellipseCount = 0;

    for (auto* item : allItems)
    {
        if (item->type() == QGraphicsEllipseItem::Type) ellipseCount++;
    }
    EXPECT_EQ(ellipseCount, 10); // 10 участников = 10 эллипсов
}

// 3. Тест: корректность ID на кружочках
TEST_F(GraphicsTest, IdTextLabels)
{
    auto allItems = sw->scene()->items();
    bool foundId1 = false;

    for (auto* item : allItems)
    {
        if (item->type() == QGraphicsSimpleTextItem::Type || item->type() == 8)
        { // 8 - QGraphicsTextItem
            auto* textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item);
            if (textItem && textItem->toPlainText() == "1") foundId1 = true;
        }
    }
    EXPECT_TRUE(foundId1);
}

// 4. Тест: перемещение в Зал Б (имитация onInteraction)
TEST_F(GraphicsTest, MoveToRoomBLogic)
{
    // 1. Находим кружочки для участников 1 и 2 в "Зале А" (x < 350)
    auto allItems = sw->scene()->items();
    QGraphicsEllipseItem* item1 = nullptr;
    QGraphicsEllipseItem* item2 = nullptr;

    // Нам нужны именно эллипсы, привязанные к ID 1 и 2
    // В SimulationWidget они хранятся в std::map<int, QGraphicsEllipseItem*> items;
    for (auto* item : allItems)
    {
        if (auto* ellipse = qgraphicsitem_cast<QGraphicsEllipseItem*>(item))
        {
            for (auto* child : ellipse->childItems())
            {
                if (auto* txt = qgraphicsitem_cast<QGraphicsTextItem*>(child))
                {
                    if (txt->toPlainText() == "1") item1 = ellipse;
                    if (txt->toPlainText() == "2") item2 = ellipse;
                }
            }
        }
    }
    ASSERT_NE(item1, nullptr);
    ASSERT_NE(item2, nullptr);
    // Проверяем, что изначально они в левой части (Зал А)
    EXPECT_LT(item1->pos().x(), 350);
    // 2. Имитируем "мэтч" (взаимодействие)
    sw->onInteraction(1, 2, true);
    // 3. Используем QTimer, чтобы дождаться конца анимации (она длится 500мс)
    // В юнит-тесте нам нужно прокрутить цикл событий, чтобы таймер сработал
    bool animationFinished = false;

    QTimer::singleShot(700, [&]() {
        // ПРОВЕРКА: После мэтча они должны оказаться в Зале Б (x > 390)
        EXPECT_GT(item1->pos().x(), 390);
        EXPECT_GT(item2->pos().x(), 390);
        // Проверяем смену цвета на мадженту (как в коде)
        EXPECT_EQ(item1->pen().color(), Qt::magenta);
        animationFinished = true;
        });
    // "Крутим" цикл событий Qt, пока таймер не сработает
    QTime dieTime = QTime::currentTime().addMSecs(1000);

    while (!animationFinished && QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    EXPECT_TRUE(animationFinished) << "Анимация не завершилась или зависла!";
}

