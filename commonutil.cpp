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
        if (faceData.ageInfo.num > 0) {
            json["age"] = faceData.ageInfo.ageArray[0];
        }
        if (faceData.genderInfo.num > 0) {
            json["gender"] = faceData.genderInfo.genderArray[0];
        }
        if (faceData.angleInfo.num > 0) {
            json["angle_pitch"] = faceData.angleInfo.pitch[0];
            json["angle_roll"] = faceData.angleInfo.roll[0];
            json["angle_status"] = faceData.angleInfo.status[0];
            json["angle_yaw"] = faceData.angleInfo.yaw[0];
        }
        json["faceOrient"] = faceData.faceInfo.faceOrient;
        json["faceRectLeft"] = faceData.faceInfo.faceRect.left;
        json["faceRectTop"] = faceData.faceInfo.faceRect.top;
        json["faceRectRight"] = faceData.faceInfo.faceRect.right;
        json["faceRectBottom"] = faceData.faceInfo.faceRect.bottom;
        if (faceData.liveNessInfo.num > 0) {
            json["liveNessInfo"] = faceData.liveNessInfo.isLive[0];
        }
        jsonArray.append(json);
    }
    return jsonArray;
}

int CommonUtil::writeJson(const QString& fileName, QJsonArray jsonArray) {
    QJsonDocument document;
    document.setArray(jsonArray);
    QByteArray bytes = document.toJson( QJsonDocument::Indented );

    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    path.append("/.cp-camera-face");
    if (!QDir(path).exists()) {
        QDir().mkdir(path);
    }
    path.append("/");
    path.append(fileName);
    QFile file( path );
    if( file.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate ) )
    {
        QTextStream iStream( &file );
        iStream.setCodec( "utf-8" );
        iStream << bytes;
        file.close();
        return 0;

    } else {
        return -1;
    }
}

QList<FaceData> CommonUtil::loadRegisteredImage(QJsonArray jsonArray) {
    QList<FaceData> registeredFaceDataList;
    foreach(const QJsonValue & value, jsonArray) {
        QJsonObject jsonObject = value.toObject();
        FaceData faceData;
        const char *p = jsonObject["info"].toString().toLocal8Bit().data();
        memset(faceData.info, 0, sizeof(char)* 64);
        memcpy(faceData.info, p, sizeof(char) * strlen(p));

        QByteArray feature;
        feature.append(jsonObject["facefeature"].toString());
        QByteArray f = QByteArray::fromBase64(feature);
        qDebug() << "facefeature size: " << f.size();
        faceData.faceFeature.feature = (MByte*) malloc(sizeof(char) * f.size());
        memcpy(faceData.faceFeature.feature, f.data(), sizeof(char)*f.size());
        faceData.faceFeature.featureSize = f.size();
#if 1
        QByteArray image;
        image.append(jsonObject["image"].toString());
        QByteArray i = QByteArray::fromBase64(image);
        qDebug() << "image size: " << i.size();

        QPixmap pix;
        pix.loadFromData(i, "JPEG");
        faceData.image = pix.toImage();
#endif
        registeredFaceDataList.append(faceData);
    }
    return registeredFaceDataList;
}

QJsonArray CommonUtil::readJson(const QString& fileName) {
    QJsonArray jsonArray;
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    path.append("/.cp-camera-face");
    if (!QDir(path).exists()) {
        QDir().mkdir(path);
    }
    path.append("/");
    path.append(fileName);
    qDebug() << "readJosn: " << path;
    QFile file( path );

    if(file.open(QIODevice::ReadOnly)) {
        QByteArray bytes = file.readAll();
        file.close();

        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson( bytes, &jsonError );
        if( jsonError.error != QJsonParseError::NoError ) {
            qDebug() << "fromJson failed: " << jsonError.errorString() << endl;
            return jsonArray;
        } else {
            if( document.isArray() ) {
                qDebug() << "json is array";
                jsonArray = document.array();
                return jsonArray;
            } else {
                qDebug() << "json is not array";
                return jsonArray;
            }
        }
    } else {
        qDebug() << "file can't be opened";
        return jsonArray;
    }
}
