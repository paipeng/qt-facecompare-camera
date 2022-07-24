#include "commonutil.h"

CommonUtil::CommonUtil()
{

}

QJsonArray CommonUtil::convertRegisteredImageToJson(QList<FaceData> registeredFaceDataList) {
    QJsonArray jsonArray;

    foreach(FaceData faceData, registeredFaceDataList ) {
        QJsonObject json;
        json["info"] = QString::fromLocal8Bit((char*)faceData.info);
        QByteArray ba; // buffer uses ba as its storage
        QBuffer buf(&ba); // we'll use buf as QIODevice
        buf.open(QIODevice::WriteOnly);
        faceData.image.save(&buf, "JPEG");
        json["image"] = QString(ba.toBase64());;
        QByteArray feature((const char*)faceData.faceFeature.feature, faceData.faceFeature.featureSize);
        json["facefeature"] = QString(feature.toBase64());
        qDebug() << "json: " << json;
        jsonArray.append(json);
    }
    return jsonArray;
}
