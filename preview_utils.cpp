#include "preview_utils.h"

#include <QFile>

QByteArray Utils::readStringFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return QByteArray();

    const QByteArray b = file.readAll();
    file.close();
    return b;
}
