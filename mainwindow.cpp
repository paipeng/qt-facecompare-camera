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


    QObject::connect(&arcFaceEngine, &ArcFaceEngine::updateFaceDecodeResult, this, &MainWindow::updateFaceDecodeResult);
    arcFaceEngine.start();
    arcFaceEngine.setPriority(QThread::LowestPriority);

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

    camera.setAutoCapture(true);

    qDebug() << "App path : " << qApp->applicationDirPath();
    QString path = qApp->applicationDirPath();
    path.append("/setting.ini");
    qDebug() << "setting path: " << path;
    QSettings settings(path, QSettings::IniFormat);
    QString appId = settings.value("x64_free/APPID", "XXXXXXXX").toString(); // settings.value() returns QVariant
    qDebug() << "appId: " << appId;

    QString sdkKey = settings.value("x64_free/SDKKEY", "YYYYYYYY").toString(); // settings.value() returns QVariant
    qDebug() << "sdkKey: " << sdkKey;

    //arcFaceEngine = new ArcFaceEngine();

    MRESULT faceRes = arcFaceEngine.ActiveSDK((char*)(appId.toStdString().c_str()), (char*)(sdkKey.toStdString().c_str()), NULL);
    qDebug() << "ActiveSDK: " << faceRes;

    if (faceRes == 0) {
        QMessageBox::information(this, tr("arcsoft_sdk"), tr("arcsoft_sdk_activate_success"), QMessageBox::Ok);

        //获取激活文件信息
        ASF_ActiveFileInfo activeFileInfo = { 0 };
        arcFaceEngine.GetActiveFileInfo(activeFileInfo);

        if (faceRes == MOK) {
            faceRes = arcFaceEngine.InitEngine(ASF_DETECT_MODE_IMAGE);//Image
            qDebug() << "IMAGE模式下初始化结果: " << faceRes;

            //faceRes = arcFaceEngine->InitEngine(ASF_DETECT_MODE_VIDEO);//Video
            //qDebug() << "VIDEO模式下初始化结果: " << faceRes;
        }

        arcFaceEngine.SetLivenessThreshold(0.8f, 0.0f);

        //load QImage and do register this image
        QImage registeredFaceImage;
        registeredFaceImage.load("C:/Users/paipeng/Pictures/paipeng2.jpeg");
        qDebug() << "image: " << registeredFaceImage.width() << "-" << registeredFaceImage.height() << "-" << registeredFaceImage.bitPlaneCount() << " " << registeredFaceImage.byteCount();

        arcFaceEngine.registerFace(registeredFaceImage);
    } else if (faceRes == 28673) {
        QMessageBox::critical(this, tr("arcsoft_sdk"), tr("please set valide appid/sdk-key"), QMessageBox::Ok);
    } else {
        QMessageBox::critical(this, tr("arcsoft_sdk"), tr("please set valide appid/sdk-key"), QMessageBox::Ok);

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
    previewImageSize.setWidth(img.width());
    previewImageSize.setHeight(img.height());
    timer.start();
    arcFaceEngine.setImage(img);
}

void MainWindow::cameraReadyForCapture(int cameraId, bool ready) {
    qDebug() << "cameraReadyForCapture: " << cameraId << " state: " << ready;

    if (ready) {
        if (cameraId == 0) {
            if (camera.getAutoCapture()) {
                camera.setAutoCapture(false);
                camera.takeImage();
            }
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


void MainWindow::updateFaceDecodeResult(int decodeState, float score) {
    Q_UNUSED(decodeState);
    Q_UNUSED(score);
    qint64 t = timer.elapsed();

    qDebug() << "updateFaceDecodeResult: " << decodeState << " score: " << score << " elapsed time: " << t;


    ui->cameraViewfinder->updateData(decodeState, score, &(arcFaceEngine.faceData), t==0?0:1000.0f/t, previewImageSize);
    if (decodeState == 0) {
        FaceData *faceData = ui->cameraViewfinder->getFaceData();
#if 1
        QString showStr = QString(tr("face_result")).arg(
                    QString::number(faceData->ageInfo.ageArray[0]), QString::number(faceData->genderInfo.genderArray[0]),
                QString::number(faceData->liveNessInfo.isLive[0]), QString::number(score));
        ui->cameraLabel->setText(showStr);
#endif
    } else {
        ui->cameraLabel->setText(QString("not found -> error"));
    }

    camera.takeImage();
}

