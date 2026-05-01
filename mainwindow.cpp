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
#include <random>

MainWindow::MainWindow(ConferenceSimulation *sim)
    : m_sim(sim)
{
    view = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    view->setScene(scene);

    resize(800, 600);

    setupScene();

    connect(m_sim, &ConferenceSimulation::interactionOccurred, this, &MainWindow::onInteraction);

    connect(m_sim, &ConferenceSimulation::simulationEnded, this, [this]() {
        QMessageBox::information(this, "Конец симуляции", "Подходящих пар в зале А больше нет!");
    });
}

MainWindow::~MainWindow()
{
    if (m_sim)
    {
        m_sim->stop();
        delete m_sim;
    }
}

void MainWindow::setupButton()
{
    QWidget *controls = new QWidget(this);
    QHBoxLayout *hLayout = new QHBoxLayout(controls);

    idInput = new QLineEdit(this);
    idInput->setPlaceholderText("Введите ID...");
    idInput->setFixedWidth(100);

    findButton = new QPushButton("Найти участника", this);
    logButton = new QPushButton("Лог событий", this);
    finalReportButton = new QPushButton("Итоговый отчет", this);

    hLayout->addWidget(idInput);
    hLayout->addWidget(findButton);
    hLayout->addWidget(logButton);
    hLayout->addWidget(finalReportButton);
    hLayout->addStretch();

    // Собираем вертикальный лейаут
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(controls);
    mainLayout->addWidget(view);

    QWidget *central = new QWidget(this);
    central->setLayout(mainLayout);
    setCentralWidget(central);

    // Кнопка поиска
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

    // Кнопка ЛОГА
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

    // Кнопка ОТЧЕТА
    connect(finalReportButton, &QPushButton::clicked, this, [this]() {
        QString finalReport = m_sim->getFinalStateReport();
        QMessageBox::information(this, "Итоги", finalReport);
    });
}

void MainWindow::setupScene()
{
    scene->setSceneRect(0, 0, 750, 550);

    scene->addRect(10, 10, 350, 530, QPen(Qt::black), QBrush(QColor(240, 240, 240)));
    scene->addRect(390, 10, 350, 530, QPen(Qt::black), QBrush(QColor(220, 255, 220)));

    auto textA = scene->addText("Зал А (Вечеринка)");
    textA->setPos(120, 15);
    auto textB = scene->addText("Зал Б (Коктейли)");
    textB->setPos(500, 15);

    std::map<Interest, QColor> colors =
    {
        {Interest::EPIDEMIOLOGY, Qt::red},
        {Interest::STATISTICS, Qt::blue},
        {Interest::CLINICAL_TRIALS, Qt::green},
        {Interest::HEALTH_POLICY, Qt::yellow}
    };

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> posX(30, 300);
    std::uniform_int_distribution<> posY(50, 480);

    for (const auto& p : m_sim->getRoomA())
    {
        auto* circle = scene->addEllipse(0, 0, 30, 30, QPen(Qt::black), QBrush(colors[p.interest]));
        circle->setPos(posX(gen), posY(gen));
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

    connect(anim, &QVariantAnimation::valueChanged,
            [item1, item2, start1, start2, meetingPoint](const QVariant &value) {
        qreal t = value.toReal();
        item1->setPos(start1 + (meetingPoint - start1) * t);
        item2->setPos(start2 + (meetingPoint + QPointF(15,0) - start2) * t);
    });

    if (isMatch)
    {
        connect(anim, &QVariantAnimation::finished, [item1, item2]() {
            std::mt19937 gen(std::random_device{}());
            std::uniform_int_distribution<> posX(400, 680);
            std::uniform_int_distribution<> posY(50, 480);
            QPointF endB(posX(gen), posY(gen));
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

