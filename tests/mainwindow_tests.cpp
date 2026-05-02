#include <gtest/gtest.h>
#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QAction>
#include <QMenuBar>
#include <QList>
#include <QTimer>
#include <QMessageBox>
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
    auto allButtons = mw->findChildren<QPushButton*>();
    
    ASSERT_FALSE(allButtons.isEmpty());

    bool foundFind = false;
    bool foundLog = false;
    bool foundReport = false;

    for (auto* btn : allButtons) {
        if (btn->text().contains("Найти")) foundFind = true;
        if (btn->text().contains("Лог событий")) foundLog = true;
        if (btn->text().contains("Итоговый отчет")) foundReport = true;
    }
    EXPECT_TRUE(foundFind);
    EXPECT_TRUE(foundLog);
    EXPECT_TRUE(foundReport);
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

//==============тесты-триггеры кнопок==============
// 1. Тест поиска участника (Имитация ввода + клик)
TEST_F(MainWindowTest, FullSearchInteractionTest)
{
    // Находим поле ввода и кнопку
    QLineEdit* input = mw->findChild<QLineEdit*>();
    QPushButton* btnFind = nullptr;

    for (auto* b : mw->findChildren<QPushButton*>())
    {
        if (b->text().contains("Найти")) btnFind = b;
    }

    ASSERT_NE(input, nullptr);
    ASSERT_NE(btnFind, nullptr);

    // Имитируем действия пользователя:
    input->setText("1"); // Вводим ID

    // Заводим "киллера" для окна результата поиска
    QTimer::singleShot(500, []() {
        for (QWidget *w : QApplication::allWidgets()) {
            if (auto *box = qobject_cast<QMessageBox *>(w)) {
                // Проверяем, что в окне РЕАЛЬНО инфа про ID 1
                EXPECT_TRUE(box->text().contains("ID: 1"));
                box->accept();
            }
        }
    });

    btnFind->click(); // Нажимаем кнопку!
    SUCCEED();
}

// 2. Тест Лога (имитация открытия диалогового окна)
TEST_F(MainWindowTest, LogDialogOpeningTest)
{
    QPushButton* btnLog = nullptr;
    for (auto* b : mw->findChildren<QPushButton*>()) {
        if (b->text().contains("Лог")) btnLog = b;
    }

    ASSERT_NE(btnLog, nullptr);

    // "Киллер" для QDialog (Журнал событий — это QDialog, а не QMessageBox)
    QTimer::singleShot(500, []() {
        for (QWidget *w : QApplication::allWidgets()) {
            // Ищем именно QDialog, но НЕ главное окно
            if (auto *dlg = qobject_cast<QDialog *>(w)) {
                if (dlg->windowTitle().contains("ЖУРНАЛ")) {
                    dlg->accept(); // Закрываем
                }
            }
        }
    });

    btnLog->click();
    SUCCEED();
}

//3. Тест хелп-меню
TEST_F(MainWindowTest, AboutBoxClickTest)
{
    // 1. Ставим "киллера" для будущего окна
    QTimer::singleShot(500, []() {
        QWidgetList allWidgets = QApplication::allWidgets();
        for (QWidget *w : allWidgets)
        {
            if (auto *box = qobject_cast<QMessageBox *>(w))
            {
                box->accept(); // Закрываем месседж-бокс
            }
        }
    });

    // 2. Находим Action и имитируем клик
    QAction* aboutAct = mw->findChild<QAction*>("aboutAction");
    ASSERT_NE(aboutAct, nullptr);
    aboutAct->trigger(); // Это вызовет QMessageBox и заблокирует поток

    // Сюда код дойдет только ПОСЛЕ того, как таймер закроет окно
    SUCCEED();
}
