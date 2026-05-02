#include "app.h"
#include <QPixmap>
#include <QDebug>

QIcon App::UI::makeIcon(QString iconPath)
{
    QPixmap originalPixmap(iconPath);

    if (originalPixmap.isNull())
    {
        qDebug() << "ERROR: Failed to load original image for icon!";
    }

    QPixmap scaledIcon = originalPixmap.scaled(
        256, 256,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );

    QIcon icon(scaledIcon);

    if(icon.availableSizes().isEmpty())
    {
        qDebug()<<"ERROR: Fauld to load icon!";
    }
    return icon;
}

QSplashScreen* App::UI::makeSplashScreen(QString imagePath)
{
    QPixmap pixmap(imagePath);

    if (pixmap.isNull())
    {
        qDebug() << "ERROR: Failed to load splash image!";
    }

    int desiredWidth = 800;
    int desiredHeight = 800;

    QPixmap scaledPixmap = pixmap.scaled(
        desiredWidth,
        desiredHeight,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );
    QSplashScreen *splash = new QSplashScreen(scaledPixmap);
    splash->show();
    splash->repaint();
    splash->raise();
    return splash;
}

QString App::Data::getCurrentTimestamp()
{
    return QDateTime::currentDateTime().toString("HH:mm:ss");
}
