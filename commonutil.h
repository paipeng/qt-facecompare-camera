#ifndef COMMONUTIL_H
#define COMMONUTIL_H

#include <QJsonArray>
#include "ArcFaceEngine.h"

class CommonUtil
{
public:
    CommonUtil();
    QJsonArray convertRegisteredImageToJson(QList<FaceData> registeredFaceDataList);
    QList<FaceData> loadRegisteredImage(QJsonArray jsonArray);

    int writeJson(const QString& fileName, QJsonArray jsonArray);
    QJsonArray readJson(const QString& fileName);
};

#endif // COMMONUTIL_H
