#include "mainwindow.h"
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QMessageBox>
#include <QVariantAnimation>
#include <QHBoxLayout>
#include <QTextEdit>
#include <iostream>

MainWindow::MainWindow(ConferenceSimulation *sim)
    : m_sim(sim)
{
    view = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    view->setScene(scene);
    setCentralWidget(view);
    resize(800, 600);

    setupScene();

    // Соединяем логику с графикой
    connect(m_sim, &ConferenceSimulation::interactionOccurred, this, &MainWindow::onInteraction);
    connect(m_sim, &ConferenceSimulation::simulationEnded, [this]() {
        qDebug() << "Simulation over. Thread stopping...";
        QMessageBox::information(this, "Finish", "Подходящих пар больше нет!");
        m_sim->stop();
    });
}

MainWindow::~MainWindow()
{
    if (m_sim) {
        m_sim->requestInterruption(); // Просим поток выйти из цикла
        m_sim->wait();                // ЖДЕМ (это критично!), пока он реально умрет
        delete m_sim;                 // Теперь удаляем безопасно
    }
}

void MainWindow::setupButton()
{
    QWidget *controls = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(controls);

    idInput = new QLineEdit(this);
    idInput->setPlaceholderText("Введите ID...");
    idInput->setFixedWidth(100);

    findButton = new QPushButton("Найти участника", this);

    layout->addWidget(idInput);
    layout->addWidget(findButton);
    layout->addStretch(); // Чтобы прижать кнопки влево

    // Добавляем панель в MainWindow (над сценой)
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(controls);
    mainLayout->addWidget(view);

    QWidget *central = new QWidget(this);
    central->setLayout(mainLayout);
    setCentralWidget(central);

    // Коннект для кнопки
    connect(findButton, &QPushButton::clicked, [this]() {
        bool ok;
        int id = idInput->text().toInt(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Ошибка", "Введите корректное числовое ID!");
            return;
        }

        // Вызываем поиск (нужно добавить геттер или сделать метод в Simulation)
        QString info = m_sim->findPersonById(id);

            if (!info.isEmpty()) {
                // УСПЕХ: Участник найден
                QMessageBox::information(this, "Результат поиска", info);
            } else {
                // ОШИБКА: Число валидное, но человека с таким ID нет
                QMessageBox::critical(this, "Не найден",
                    QString("Участник с ID %1 не найден в списках конференции.").arg(id));
            }
    });

    logButton = new QPushButton("Лог событий", this);
    layout->addWidget(logButton);

    connect(logButton, &QPushButton::clicked, [this]() {
        if (!m_sim) return;
        QDialog *logDialog = new QDialog(this);
            logDialog->setWindowTitle("ЖУРНАЛ СОБЫТИЙ");
            logDialog->resize(600, 400);

            QVBoxLayout *layout = new QVBoxLayout(logDialog);
            QTextEdit *textEdit = new QTextEdit(logDialog);

            // Получаем текст ДО того, как диалог начнет жить своей жизнью
            QString history = m_sim->getHistoryLog();
            textEdit->setPlainText(history);
            textEdit->setReadOnly(true);

            QPushButton *closeBtn = new QPushButton("Закрыть", logDialog);
            connect(closeBtn, &QPushButton::clicked, logDialog, &QDialog::accept);

            layout->addWidget(textEdit);
            layout->addWidget(closeBtn);

            logDialog->exec();
            // НЕ ИСПОЛЬЗУЙ deleteLater() здесь, если есть сомнения в потоках
            delete logDialog;
    });

    finalReportButton = new QPushButton("Итоговый отчет", this);
    layout->addWidget(finalReportButton);

    connect(finalReportButton, &QPushButton::clicked, [this]() {
        // Вызываем нашу новую функцию статистики
        m_sim->blockSignals(true);
        QString finalReport = m_sim->getFinalStateReport();
         m_sim->blockSignals(false);
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Результаты конференции");
        msgBox.setText("Распределение участников по залам:");
        msgBox.setInformativeText(finalReport);
        msgBox.setStandardButtons(QMessageBox::Ok);

        // Сделаем иконку информационной для солидности
        msgBox.setIcon(QMessageBox::Information);

        msgBox.exec();
    });
}
void MainWindow::setupScene()
{
    scene->setSceneRect(0, 0, 750, 550);

    // Рисуем Комнату А (слева) и Комнату Б (справа)
    scene->addRect(10, 10, 350, 530, QPen(Qt::black), QBrush(QColor(240, 240, 240)));
    scene->addRect(390, 10, 350, 530, QPen(Qt::black), QBrush(QColor(220, 255, 220)));

    auto textA = scene->addText("Зал А (Вечеринка)");
    textA->setPos(120, 15);
    auto textB = scene->addText("Зал Б (Коктейли)");
    textB->setPos(500, 15);

    // Цвета для интересов
    std::map<Interest, QColor> colors = {
        {Interest::EPIDEMIOLOGY, Qt::red},
        {Interest::STATISTICS, Qt::blue},
        {Interest::CLINICAL_TRIALS, Qt::green},
        {Interest::HEALTH_POLICY, Qt::yellow}
    };

    // Создаем кружочки для участников
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> posX(30, 300);
    std::uniform_int_distribution<> posY(50, 480);

    for (const auto& p : m_sim->getRoomA()) {
        auto* circle = scene->addEllipse(0, 0, 30, 30, QPen(Qt::black), QBrush(colors[p.interest]));
        circle->setPos(posX(gen), posY(gen));
        
        // Добавляем ID внутрь кружка
        auto* idText = scene->addText(QString::number(p.id));
        idText->setParentItem(circle);
        idText->setPos(5, 2);

        items[p.id] = circle;
    }
    setupButton();
}

void MainWindow::onInteraction(int id1, int id2, bool isMatch)
{
    auto* item1 = items[id1];
    auto* item2 = items[id2];

    if (!item1 || !item2) return;

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
        item2->setPos(start2 + (meetingPoint + QPointF(15,0) - start2) * t); // Небольшой зазор
    });

    if (isMatch) {
        connect(anim, &QVariantAnimation::finished, [this, item1, item2]() {
            std::mt19937 gen(std::random_device{}());
            std::uniform_int_distribution<> posX(400, 680);
            std::uniform_int_distribution<> posY(50, 480);

            QPointF endB(posX(gen), posY(gen));

            item1->setPos(endB);
            item2->setPos(endB + QPointF(20, 20)); // Чуть внахлест, как пара

            item1->setPen(QPen(Qt::magenta, 3));
            item2->setPen(QPen(Qt::magenta, 3));

            // Чтобы они не перекрывались другими, выводим их "на передний план"
            item1->setZValue(1);
            item2->setZValue(1);
        });
    }

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

