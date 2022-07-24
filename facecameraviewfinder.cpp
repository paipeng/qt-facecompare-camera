#include "facecameraviewfinder.h"

FaceCameraViewfinder::FaceCameraViewfinder(QWidget *parent) : QCameraViewfinder(parent)
{

}
void FaceCameraViewfinder::updateData(int decodeState, float score, FaceData *faceData, float fps, QSize previewImageSize) {
    this->decodeState = decodeState;
    this->score = score;
    this->fps = fps;
    this->previewImageSize = previewImageSize;
    memcpy(&(this->faceData), faceData, sizeof(FaceData));
    this->repaint();
}

FaceData* FaceCameraViewfinder::getFaceData() {
    return &(this->faceData);
}

void FaceCameraViewfinder::paintEvent(QPaintEvent* event) {
    //qDebug() << "paintEvent";
    // Default rendered -> call base class
    QCameraViewfinder::paintEvent(event);
    // draw some text
    QPainter painter(this);
    if (score > 0.8) {
        painter.setPen(QPen(Qt::green, 2));
    } else {
        painter.setPen(QPen(Qt::red, 3));
    }
    //QBrush brush(Qt::green, Qt::SolidPattern);
    //painter.setBrush(brush);
    QString fpsStr;
    QFont font = QGuiApplication::font();
    font.setPointSize(10);
    painter.setFont(font);
    fpsStr.sprintf("FPS: %2.02f", fps);
    painter.drawText(20,20, fpsStr);
    if (decodeState == 0) {
        // rescale camera-preview-image coordination -> viewfinder cooridnation
        float scale = this->height()*1.0f/this->previewImageSize.height();
        //qDebug() << "preview image width: " << this->previewImageSize.width() << " view width: " << this->width() << " scale: " << scale;

        //float s =  *1.0 / this->width();
        QRect rect(faceData.faceInfo.faceRect.left * scale,
                   faceData.faceInfo.faceRect.top * scale,
                   (faceData.faceInfo.faceRect.right - faceData.faceInfo.faceRect.left) * scale,
                   (faceData.faceInfo.faceRect.bottom - faceData.faceInfo.faceRect.top) * scale);


        painter.drawRect(rect);

        QString scoreStr;
        scoreStr.sprintf("Score: %.02f ", this->score);

        if (strlen(this->faceData.info) > 0) {
            QString info = QString::fromLocal8Bit((char*)this->faceData.info);
            scoreStr.append(info);
        }
        painter.drawText(faceData.faceInfo.faceRect.left * scale,
                         faceData.faceInfo.faceRect.top * scale - 10, scoreStr);
    }
}

void FaceCameraViewfinder::setText(const QString &text) {

}
