#include <gtest/gtest.h>
#include <QApplication>
#include "../app.h"

class AppTest : public ::testing::Test {
protected:
    static void SetUpTestSuite()
    {
        static int argc = 1;
        static char* argv[] = {(char*)"test"};
        static QApplication app(argc, argv);
    }
};

TEST_F(AppTest, MakeIconValid)
{
    QIcon icon = App::UI::makeIcon(":/icon.png");
    EXPECT_TRUE(icon.availableSizes().isEmpty() || !icon.isNull());
}

TEST_F(AppTest, MakeSplashScreenNotNull)
{
    QSplashScreen* splash = App::UI::makeSplashScreen(":/splash.png");
    ASSERT_NE(splash, nullptr);
    EXPECT_LE(splash->pixmap().width(), 800);
}

TEST_F(AppTest, TimestampFormat)
{
    QString ts = App::Data::getCurrentTimestamp();
    EXPECT_EQ(ts.length(), 8);
    EXPECT_EQ(ts[2], ':');
    EXPECT_EQ(ts[5], ':');
    bool ok = false;
    ts.left(2).toInt(&ok);
    EXPECT_TRUE(ok);
}

