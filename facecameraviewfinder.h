#ifndef FACECAMERAVIEWFINDER_H
#define FACECAMERAVIEWFINDER_H

#include <QCameraViewfinder>
#include <QPainter>
#include "ArcFaceEngine.h"

class FaceCameraViewfinder : public QCameraViewfinder
{
    Q_OBJECT
public:
    FaceCameraViewfinder(QWidget *parent = nullptr);
    void updateData(int decodeState, float score, FaceData *faceData, float fps);
    FaceData* getFaceData();
    void setText(const QString &text);
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    FaceData faceData;
    int decodeState;
    float score;
    float fps;
};

#endif // FACECAMERAVIEWFINDER_H
