#ifndef COMMONUTIL_H
#define COMMONUTIL_H

#include <QJsonArray>
#include "ArcFaceEngine.h"

class CommonUtil
{
public:
    CommonUtil();
    QJsonArray convertRegisteredImageToJson(QList<FaceData> registeredFaceDataList);
};

#endif // COMMONUTIL_H
