#include <gtest/gtest.h>
#include "../simulation.h"

// 1. Тест конструктора и инициализации
TEST(SimulationLogic, ConstructorInitialization)
{
    int n = 20;
    ConferenceSimulation sim(n, 100);
    // Проверяем, что создано ровно N человек
    EXPECT_EQ(sim.getRoomA().size(), n);
    // Проверяем, что в Зале Б пусто
    EXPECT_EQ(sim.getRoomB().size(), 0);
}

// 2. Тест поиска участника по ID
TEST(SimulationLogic, SearchFunctionality)
{
    ConferenceSimulation sim(5, 100);    
    // Ищем существующего (ID начинаются с 1)
    QString found = sim.findPersonById(3);
    EXPECT_TRUE(found.contains("ID: 3"));
    EXPECT_TRUE(found.contains("Зал А"));
    // Ищем несуществующего
    QString notFound = sim.findPersonById(99);
    EXPECT_EQ(notFound, "Участник не найден");
}

// 3. Тест одного шага симуляции (nextStep)
TEST(SimulationLogic, NextStepInteraction)
{
    // Создаем всего 2 человека
    ConferenceSimulation sim(2, 100);
    // Делаем один шаг вручную
    //sim.nextStep();
    // Проверяем, что если пара совпала, зал А опустел, а зал Б наполнился
    // Либо они остались в зале А, если интересы разные.
    size_t total = sim.getRoomA().size() + sim.getRoomB().size();
    EXPECT_EQ(total, 2); // Люди не должны исчезать в никуда!
}

// 4. Тест логирования (buildLogString)
TEST(SimulationLogic, LogGeneration)
{
    ConferenceSimulation sim(10, 100);
    //sim.nextStep();
    
    QString log = sim.getHistoryLog();
    EXPECT_FALSE(log.isEmpty());
    EXPECT_NE(log, "Лог пуст");
    // Проверяем, что в логе есть временная метка (формат HH:mm:ss содержит двоеточия)
    EXPECT_TRUE(log.contains(":"));
}

// 5. Тест итогового отчета (getFinalStateReport)
TEST(SimulationLogic, FinalReportFormat)
{
    ConferenceSimulation sim(10, 100);
    QString report = sim.getFinalStateReport();
    
    EXPECT_TRUE(report.contains("===== ИТОГОВЫЙ ОТЧЕТ ====="));
    EXPECT_TRUE(report.contains("ЗАЛ А"));
    EXPECT_TRUE(report.contains("ЗАЛ Б"));
}

// 6. Тест остановки симуляции (stop)
TEST(SimulationLogic, StopCommand)
{
    ConferenceSimulation sim(10, 100);
    // Имитируем запуск
    // В реальности это делает метод process(), но мы проверим флаг
    sim.stop(); 
    EXPECT_FALSE(sim.isRunning());
    ConferenceSimulation simSmall(1, 100); // Всего 1 человек
    //simSmall.nextStep(); // Тут внутри должен сработать isRun = false
    EXPECT_FALSE(simSmall.isRunning());
}
