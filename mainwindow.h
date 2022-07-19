#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cpcamera.h"
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

private:
    void cameraState(int cameraId, int state) override;
    void processCapturedImage(int cameraId, const QImage& img) override;
    void cameraReadyForCapture(int cameraId, bool ready) override;

private:
    void displayViewfinder(int cameraId);
    void displayCapturedImage(int cameraId);

private:
    Ui::MainWindow *ui;
    CPCamera camera;
};
#endif // MAINWINDOW_H
