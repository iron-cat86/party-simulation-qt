#include "mainwindow.h"
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QMessageBox>
#include <QVariantAnimation>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QDialog>
#include <QDebug>
#include <QMenuBar>
#include <QTimer>
#include <random>

MainWindow::MainWindow(ConferenceSimulation *sim)
    : m_sim(sim)
{
    resize(800, 650);
    setHelpMenu();
    setGraphics();
    setupButtons();
    setConnects();
}

MainWindow::~MainWindow()
{
    if (m_sim)
    {
        m_sim->stop();
        delete m_sim;
    }
}

void MainWindow::setHelpMenu()
{
    helpMenu = menuBar()->addMenu("Help");

    aboutAction = new QAction("About Application", this);
    aboutAction->setObjectName("aboutAction");
    helpMenu->addAction(aboutAction);

    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox aboutBox(this);
        aboutBox.setWindowTitle("О программе");
        QString info = "<b>Conference Party Simulator</b><br><br>"
                       "<b>Автор:</b> Анна Белова (Lead Systems Programmer)<br>"
                       "<b>Назначение:</b> Моделирование социальных взаимодействий на базе Qt/C++.<br><br>"
                       "<b>Что симулирует:</b> Участники с разными интересами пытаются найти пару. "
                       "Если интересы совпадают (Epidemiology, Statistics и др.), они уходят в Lounge (Зал Б).<br><br>"
                       "<b>Использование:</b> Наблюдайте за анимацией, пользуйтесь поиском по ID, журналом событий "
                       "и не забывайте проверять итоговый отчет в любое время. Вечеринка должна продолжаться!";

        aboutBox.setTextFormat(Qt::RichText);
        aboutBox.setText(info);
        aboutBox.setStandardButtons(QMessageBox::Ok);
        aboutBox.setIcon(QMessageBox::Information);

        aboutBox.exec();
    });
}

void MainWindow::setConnects()
{
    connect(m_sim, &ConferenceSimulation::simulationEnded, this, [this]() {
            QMessageBox::information(this, "Конец симуляции", "Подходящих пар в зале А больше нет!");
    }, Qt::QueuedConnection);

    connect(m_sim, &ConferenceSimulation::interactionOccurred,
            simWidget, &SimulationWidget::onInteraction, Qt::QueuedConnection);
    connect(m_sim, &ConferenceSimulation::simulationEnded, this, [this]() {
        QTimer::singleShot(500, this, [this]() {
            for (Person* p : m_sim->getRoomA())
            {
                 if (p->room == "B")
                 {
                     simWidget->forceMoveToB(p->id);
                 }
            }

            for (Person* p : m_sim->getRoomB())
            {
                 simWidget->forceMoveToB(p->id);
            }

            QMessageBox::information(this, "Итог", "Симуляция завершена. Графика синхронизирована!");
        });
    });
}

void MainWindow::setGraphics()
{
    simWidget = new SimulationWidget(m_sim, this);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QWidget *controls = new QWidget(this);
    mainLayout->addWidget(controls);
    mainLayout->addWidget(simWidget);

    QWidget *central = new QWidget(this);
    central->setLayout(mainLayout);
    setCentralWidget(central);
}

void MainWindow::setupButtons()
{
    QWidget *controls = new QWidget(this);
    hLayout = new QHBoxLayout(controls);

    setFindEdit();
    setFindButton();
    setLogButton();
    setFinalReportButton();

    hLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(controls);
    mainLayout->addWidget(simWidget);

    QWidget *central = new QWidget(this);
    central->setLayout(mainLayout);
    setCentralWidget(central);
}

void MainWindow::setFindEdit()
{
    idInput = new QLineEdit(this);
    idInput->setPlaceholderText("Введите ID...");
    idInput->setFixedWidth(100);
    hLayout->addWidget(idInput);
}

void MainWindow::setFindButton()
{
    findButton = new QPushButton("Найти участника", this);
    connect(findButton, &QPushButton::clicked, this, [this]() {
        bool ok;
        int id = idInput->text().toInt(&ok);
        if (!ok)
        {
            QMessageBox::warning(this, "Ошибка", "Введите корректное числовое ID!");
            return;
        }
        QString info = m_sim->findPersonById(id);
        QMessageBox::information(this, "Результат поиска", info);
    });
    hLayout->addWidget(findButton);
}

void MainWindow::setLogButton()
{
    logButton = new QPushButton("Лог событий", this);
    connect(logButton, &QPushButton::clicked, this, [this]() {
        QDialog *logDialog = new QDialog(this);
        logDialog->setWindowTitle("ЖУРНАЛ СОБЫТИЙ");
        logDialog->resize(600, 400);

        QVBoxLayout *l = new QVBoxLayout(logDialog);
        QTextEdit *textEdit = new QTextEdit(logDialog);
        textEdit->setPlainText(m_sim->getHistoryLog());
        textEdit->setReadOnly(true);

        QPushButton *closeBtn = new QPushButton("Закрыть", logDialog);
        connect(closeBtn, &QPushButton::clicked, logDialog, &QDialog::accept);

        l->addWidget(textEdit);
        l->addWidget(closeBtn);
        logDialog->exec();
    });
    hLayout->addWidget(logButton);
}

void MainWindow::setFinalReportButton()
{
    finalReportButton = new QPushButton("Итоговый отчет", this);
    connect(finalReportButton, &QPushButton::clicked, this, [this]() {
        QString finalReport = m_sim->getFinalStateReport();
        QMessageBox::information(this, "Итоги", finalReport);
    });
    hLayout->addWidget(finalReportButton);
}
