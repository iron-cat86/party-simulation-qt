#ifndef UTILS_H
#define UTILS_H

#include <QSplashScreen>
#include <QIcon>
#include <QString>
#include <QDate>

namespace App 
{
    namespace UI 
    {
        QSplashScreen* makeSplashScreen(QString path);
        QIcon makeIcon(QString path);
    }
    namespace Data 
    {
        QString getCurrentTimestamp();
    }
}
#endif
