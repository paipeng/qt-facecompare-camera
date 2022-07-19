#ifndef CPCAMERAVIEWFINDER_H
#define CPCAMERAVIEWFINDER_H

#include <QCameraViewfinder>

#include "cp-multi-camera_global.h"

class CPMULTICAMERA_EXPORT CPCameraViewfinder : public QCameraViewfinder
{
    Q_OBJECT
public:
    CPCameraViewfinder(QWidget *parent = nullptr);
    void setText(const QString &text);

protected:
    void paintEvent(QPaintEvent* event) override;
};

#endif // CPCAMERAVIEWFINDER_H
