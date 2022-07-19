#include "facecameraviewfinder.h"

FaceCameraViewfinder::FaceCameraViewfinder(QWidget *parent) : QCameraViewfinder(parent)
{

}
void FaceCameraViewfinder::updateData(int decodeState, float score, FaceData *faceData, float fps) {
    this->decodeState = decodeState;
    this->score = score;
    this->fps = fps;
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
        float scale = this->width()*1.0f/faceData.width;

        QRect rect(faceData.faceInfo.faceRect.left * scale,
                   faceData.faceInfo.faceRect.top * scale,
                   (faceData.faceInfo.faceRect.right - faceData.faceInfo.faceRect.left) * scale,
                   (faceData.faceInfo.faceRect.bottom - faceData.faceInfo.faceRect.top) * scale);


        painter.drawRect(rect);
    }
}

void FaceCameraViewfinder::setText(const QString &text) {

}
