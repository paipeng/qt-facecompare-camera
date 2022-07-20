#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cpcamera.h"
#include "ArcFaceEngine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, CPCameraInterface
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void initCameras();
    void startCamera();
    void cameraChanged(int index);
    const QCameraInfo getSelectedCameraInfo(int source);
    void updateFaceDecodeResult(int decodeState, float score);

    void registeredFaceLabelClicked();
    void registerFaceImage();

private:
    void cameraState(int cameraId, int state) override;
    void processCapturedImage(int cameraId, const QImage& img) override;
    void cameraReadyForCapture(int cameraId, bool ready) override;

private:
    void displayViewfinder(int cameraId);
    void displayCapturedImage(int cameraId);
    void setPhotoImage(const QString & filePath);
private:
    Ui::MainWindow *ui;
    CPCamera camera;
    ArcFaceEngine arcFaceEngine;
    QElapsedTimer timer;
    QSize previewImageSize;
};
#endif // MAINWINDOW_H
