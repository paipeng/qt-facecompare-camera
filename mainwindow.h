#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cpcamera.h"
#include "ArcFaceEngine.h"
#include "facelabel.h"
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
    void initMenu();
    void initCameras();
    void startCamera();
    void cameraChanged(int index);
    const QCameraInfo getSelectedCameraInfo(int source);
    void updateFaceDecodeResult(int decodeState, float score, FaceData* faceData);

    void registeredFaceLabelClicked();
    void registerFaceImage();
    void clearRegisteredFaces();

    // menu
    void menuExit();
    void menuHelp();
    void menuAbout();
    void menuSettings();
    void menuLoad();
    void menuSave();

private:
    void cameraState(int cameraId, int state) override;
    void processCapturedImage(int cameraId, const QImage& img) override;
    void cameraReadyForCapture(int cameraId, bool ready) override;

private:
    void displayViewfinder(int cameraId);
    void displayCapturedImage(int cameraId);
    void setPhotoImage(const QString & filePath);
    void activateArcSoftSDK();
private:
    Ui::MainWindow *ui;
    CPCamera camera;
    ArcFaceEngine arcFaceEngine;
    QElapsedTimer timer;
    QSize previewImageSize;

    QList<FaceLabel*> registeredImageLabeList;
    FaceData detectedFaceData;
};
#endif // MAINWINDOW_H
