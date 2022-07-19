#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), camera(0, this)
{
    ui->setupUi(this);
    initCameras();
    ui->statusbar->showMessage(tr("app_info"));

    displayCapturedImage(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initCameras() {
    qDebug() << "initCameras";
    const QList<QCameraInfo> availableCameras = CPCamera::getAvailableCamersInfos();

    int index = 0;
    for (const QCameraInfo &cameraInfo : availableCameras) {
        ui->cameraComboBox->addItem(cameraInfo.description(), index);
        index ++;
    }
}

void MainWindow::startCamera() {
    qDebug() << "startCamera";
    camera.startCamera();
}

void MainWindow::cameraChanged(int index) {
    qDebug() << "camera1Changed: " << index;
    camera.setCamera(getSelectedCameraInfo(0), ui->cameraViewfinder);
}

const QCameraInfo MainWindow::getSelectedCameraInfo(int source) {
    qDebug() << "getSelectedCameraInfo: " << source;
    const QList<QCameraInfo> availableCameras = CPCamera::getAvailableCamersInfos();

    QComboBox* comboBox = NULL;
    if (source == 0) {
        comboBox = ui->cameraComboBox;
    } else {
        comboBox = ui->cameraComboBox;
    }

    int index = 0;
    for (const QCameraInfo &cameraInfo : availableCameras) {
        if (index== comboBox->currentIndex()) {
            qDebug() << "selected camera found: " << cameraInfo.description();
            return cameraInfo;
        }
        index++;
    }
    QCameraInfo defaultCameraInfo;
    return defaultCameraInfo;
}
void MainWindow::cameraState(int cameraId, int state) {
    qDebug() << "cameraState: " << cameraId << " state: " << state;
    if (state == 0) {
        if (cameraId == 0) {
            ui->cameraPushButton->setText(tr("start"));
        } else {
            ui->cameraPushButton->setText(tr("start"));
        }
        displayCapturedImage(cameraId);
    } else if (state == 1) {
        if (cameraId == 0) {
            ui->cameraPushButton->setText(tr("start"));

        } else {
            ui->cameraPushButton->setText(tr("start"));

        }
        displayCapturedImage(cameraId);
    } else if (state == 2) {
        if (cameraId == 0) {
            ui->cameraPushButton->setText(tr("stop"));
        } else {
            ui->cameraPushButton->setText(tr("stop"));
        }
        displayViewfinder(cameraId);
    }
}

void MainWindow::processCapturedImage(int cameraId, const QImage& img) {

}

void MainWindow::cameraReadyForCapture(int cameraId, bool ready) {
    qDebug() << "cameraReadyForCapture: " << cameraId << " state: " << ready;

    if (ready) {
        if (cameraId == 0) {
            //camera.takeImage();
        } else {
            if (camera.getAutoCapture()) {
                camera.setAutoCapture(false);
                camera.takeImage();
            }
        }
    }
}


void MainWindow::displayViewfinder(int cameraId) {
    if (cameraId == 0) {
        ui->cameraStackedWidget->setCurrentIndex(0);
    } else {
        ui->cameraStackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::displayCapturedImage(int cameraId) {
    if (cameraId == 0) {
        ui->cameraStackedWidget->setCurrentIndex(1);
    } else {
        ui->cameraStackedWidget->setCurrentIndex(1);
    }
}
