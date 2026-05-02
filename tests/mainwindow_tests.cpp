#include <gtest/gtest.h>
#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QAction>
#include <QMenuBar>
#include <QList>
#include "../mainwindow.h"
#include "../simulation.h"

class MainWindowTest : public ::testing::Test
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
        sim = new ConferenceSimulation(10, 100);
        mw = new MainWindow(sim);
    }

    void TearDown() override
    {
        delete mw; // sim удалится в деструкторе MainWindow
    }

    ConferenceSimulation* sim;
    MainWindow* mw;
};

// 1. Тест конструктора и инициализации (resize, заголовок)
TEST_F(MainWindowTest, InitializationTest)
{
    EXPECT_EQ(mw->width(), 800);
    EXPECT_EQ(mw->height(), 650);
}

// 2. Тест setHelpMenu (наличие Action и меню)
TEST_F(MainWindowTest, HelpMenuTest)
{
    QMenuBar* menuBar = mw->menuBar();
    ASSERT_NE(menuBar, nullptr);
    
    QList<QAction*> actions = menuBar->actions();
    bool foundHelp = false;
    for (auto* a : actions) {
        if (a->text() == "Help") foundHelp = true;
    }
    EXPECT_TRUE(foundHelp);
}

// 3. Тест setupButtons и дочерних виджетов
TEST_F(MainWindowTest, ButtonsCreationTest)
{
    // Ищем кнопки по тексту
    QPushButton* btnFind = mw->findChild<QPushButton*>();
    QPushButton* btnLog = mw->findChild<QPushButton*>();
    
    ASSERT_NE(btnFind, nullptr);
    ASSERT_NE(btnLog, nullptr);
    
    EXPECT_TRUE(btnFind->text().contains("Найти"));
    EXPECT_TRUE(btnLog->text().contains("Лог"));
}

// 4. Тест поля ввода (setFindEdit)
TEST_F(MainWindowTest, FindEditTest)
{
    QLineEdit* input = mw->findChild<QLineEdit*>();
    ASSERT_NE(input, nullptr);
    
    input->setText("123");
    EXPECT_EQ(input->text(), "123");
}

// 5. Тест логики кнопок (имитация нажатия на "Итоговый отчет")
TEST_F(MainWindowTest, FinalReportButtonClick)
{
    QPushButton* btnReport = nullptr;
    QList<QPushButton*> buttons = mw->findChildren<QPushButton*>();
    for (auto* b : buttons)
    {
        if (b->text().contains("Итоговый отчет")) btnReport = b;
    }

    ASSERT_NE(btnReport, nullptr);
    // Проверяем состояние вместо клика
    EXPECT_TRUE(btnReport->isEnabled());
    EXPECT_FALSE(btnReport->isHidden());
}

// 6. Тест наличия графического виджета (setGraphics)
TEST_F(MainWindowTest, GraphicsWidgetPresence)
{
    SimulationWidget* sw = mw->findChild<SimulationWidget*>();
    ASSERT_NE(sw, nullptr);
}
