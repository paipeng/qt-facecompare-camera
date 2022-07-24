#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "licensedialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), camera(0, this)
{
    ui->setupUi(this);
    detectedFaceData.faceFeature.featureSize = 0;
    detectedFaceData.faceFeature.feature = NULL;

    registeredImageLabeList.append(ui->registeredFace1Label);
    registeredImageLabeList.append(ui->registeredFace2Label);
    registeredImageLabeList.append(ui->registeredFace3Label);
    registeredImageLabeList.append(ui->registeredFace4Label);
    registeredImageLabeList.append(ui->registeredFace5Label);
    registeredImageLabeList.append(ui->registeredFace6Label);

    ui->statusbar->showMessage(tr("app_info"));
    initMenu();
    initCameras();

    QObject::connect(ui->registeredFaceLabel, SIGNAL(clicked()), this, SLOT(registeredFaceLabelClicked()));

    displayCapturedImage(0);


    QObject::connect(&arcFaceEngine, &ArcFaceEngine::updateFaceDecodeResult, this, &MainWindow::updateFaceDecodeResult);
    arcFaceEngine.start();
    arcFaceEngine.setPriority(QThread::LowestPriority);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initMenu() {

    QMenu* fileMenu;
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addSeparator();
    QAction *exitAct;
    exitAct = new QAction(tr("&Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("exit"));
    connect(exitAct, &QAction::triggered, this, &MainWindow::menuExit);
    fileMenu->addAction(exitAct);

    //QMenu* preferencesMenu = menuBar()->addMenu(tr("&Preferences"));
    QMenu* arcsoftSdkMenu = menuBar()->addMenu(tr("arcsoft_sdk"));
    QAction *settingAct = new QAction(tr("&Settings"), this);
    settingAct->setShortcuts(QKeySequence::Preferences);
    settingAct->setStatusTip(tr("settings"));
    connect(settingAct, &QAction::triggered, this, &MainWindow::menuSettings);
    arcsoftSdkMenu->addAction(settingAct);
    arcsoftSdkMenu->addSeparator();
    QAction *loadAct = new QAction(tr("load_registered_face"), this);
    loadAct->setStatusTip(tr("load_registered_face"));
    connect(loadAct, &QAction::triggered, this, &MainWindow::menuLoad);
    arcsoftSdkMenu->addAction(loadAct);
    QAction *saveAct = new QAction(tr("save_registered_face"), this);
    saveAct->setStatusTip(tr("save_registered_face"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::menuSave);
    arcsoftSdkMenu->addAction(saveAct);


    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *helpAct = new QAction(tr("&Help"), this);
    helpAct->setShortcuts(QKeySequence::HelpContents);
    helpAct->setStatusTip(tr("help"));
    connect(helpAct, &QAction::triggered, this, &MainWindow::menuHelp);
    helpMenu->addAction(helpAct);
    helpMenu->addSeparator();
    QAction *aboutAct = new QAction(tr("&About"), this);
    aboutAct->setShortcuts(QKeySequence::WhatsThis);
    aboutAct->setStatusTip(tr("about"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::menuAbout);
    helpMenu->addAction(aboutAct);
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

    activateArcSoftSDK();
}

void MainWindow::startCamera() {
    qDebug() << "startCamera";
    camera.setAutoCapture(true);
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
    //qDebug() << "cameraReadyForCapture: " << cameraId << " state: " << ready;

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


void MainWindow::updateFaceDecodeResult(int decodeState, float score, FaceData* faceData) {
    Q_UNUSED(decodeState);
    Q_UNUSED(score);
    qint64 t = timer.elapsed();

    qDebug() << "updateFaceDecodeResult: " << decodeState << " score: " << score << " elapsed time: " << t;
    //FaceData *faceData2 = &(arcFaceEngine.faceData);//ui->cameraViewfinder->getFaceData();
    qDebug() << "faceData->faceFeature.featureSize: " << faceData->faceFeature.featureSize;

    ui->cameraViewfinder->updateData(decodeState, score, &(arcFaceEngine.faceData), t==0?0:1000.0f/t, previewImageSize);
    if (decodeState == 0) {
#if 1
        QString showStr = QString(tr("face_result")).arg(
                    QString::number(faceData->ageInfo.ageArray[0]),
                (faceData->genderInfo.genderArray[0]==0?tr("male"):tr("female")),
                QString::number(faceData->liveNessInfo.isLive[0]),
                QString::number(score));
        ui->cameraLabel->setText(showStr);
#endif
    } else {
        ui->cameraLabel->setText(QString("not found -> error"));
    }
    if (faceData != NULL) {
        int w = ui->detectedFaceLabel->width();
        int h = ui->detectedFaceLabel->height();

        QPixmap pixmap = QPixmap::fromImage(faceData->image);
        ui->detectedFaceLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));

        qDebug() << "faceData->faceFeature.featureSize: " << faceData->faceFeature.featureSize;
        if (faceData->faceFeature.featureSize > 0) {
            detectedFaceData.image = faceData->image;
            if (detectedFaceData.faceFeature.feature == NULL) {
                qDebug() << "malloc feature ";
                detectedFaceData.faceFeature.feature = (MByte*) malloc(sizeof(char) * faceData->faceFeature.featureSize);
            }
            detectedFaceData.faceFeature.featureSize = faceData->faceFeature.featureSize;
            //memcpy(detectedFaceData.faceFeature.feature, faceData->faceFeature.feature, sizeof(char) * faceData->faceFeature.featureSize);

            for (int i = 0; i < faceData->faceFeature.featureSize; i++) {
                detectedFaceData.faceFeature.feature[i] = faceData->faceFeature.feature[i];
            }
        }

    }

    camera.takeImage();
}

void MainWindow::registeredFaceLabelClicked() {
    qDebug() << "registeredFaceLabelClicked";
    QString passPhoto = QFileDialog::getOpenFileName(this,
        tr("open_image"), "/Users/paipeng/Documents", tr("image_file_format"));
    qDebug() << "selected file: " << passPhoto;

    setPhotoImage(passPhoto);
}

void MainWindow::setPhotoImage(const QString & filePath) {
    int w = ui->registeredFaceLabel->width();
    int h = ui->registeredFaceLabel->height();
    QImage passPhotoImage = QImage(filePath);

    if (!passPhotoImage.isNull()) {
        QPixmap pixmap = QPixmap::fromImage(passPhotoImage);
        ui->registeredFaceLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
    } else {
        QImage passphoto = QImage(":/resources/images/FacialDetection.png");
        QPixmap pixmap = QPixmap::fromImage(passphoto);
        ui->registeredFaceLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
    }
}

void MainWindow::registerFaceImage() {
    qDebug() << "registerFaceImage";
    const QPixmap* pixmap = ui->detectedFaceLabel->pixmap();
    if (pixmap) {
        if (arcFaceEngine.registeredFaceDataList.size() >= 6) {
            QMessageBox::warning(this, tr("arcsoft_sdk"), tr("arcsoft_sdk_max_registered_face_exceeded"), QMessageBox::Ok);

            return;
        }
        bool bRet = false;
        QString placeholder = "";
        QString info = QInputDialog::getText(this, tr("register_face_image_title"), tr("please_input_face_info"), QLineEdit::Normal, placeholder, &bRet);
        if (bRet) {
            //QImage image(pixmap->toImage());
            //image.save("detectedFace.jpeg");
            //detectedFaceData.info = info;
            qDebug() << "registerFaceImage: " << info;
            const char *p = info.toLocal8Bit().data();
            qDebug() << "info len: " << strlen(p);
            memset(detectedFaceData.info, 0, sizeof(char)* 64);
            memcpy(detectedFaceData.info, p, sizeof(char) * strlen(p));
            //detectedFaceData.image.save("detectedFaceImage.bmp");
            arcFaceEngine.registerFaceData(&detectedFaceData);
            ui->statusbar->showMessage(tr("face_registered"));
            int i = 0;
            foreach(FaceData faceData, arcFaceEngine.registeredFaceDataList) {
                int w = registeredImageLabeList.at(i)->width();
                int h = registeredImageLabeList.at(i)->height();
                QPixmap pixmap = QPixmap::fromImage(faceData.image);
                registeredImageLabeList.at(i)->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
                QString info2 = QString::fromLocal8Bit(faceData.info);
                registeredImageLabeList.at(i)->setInfo(info2);
                i++;

            }

        }
    }

}

void MainWindow::clearRegisteredFaces() {
    qDebug() << "clearRegisteredFaces";
    arcFaceEngine.registeredFaceDataList.clear();
    int i = 0;
    QImage green;
    green.load("./images/FacialDetection.png");
    QPixmap pixmap = QPixmap::fromImage(green);
    foreach(FaceLabel* registeredImageLabel, registeredImageLabeList) {
        int w = registeredImageLabeList.at(i)->width();
        int h = registeredImageLabeList.at(i)->height();
        registeredImageLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
        registeredImageLabel->setInfo("");
        i++;
    }
}

void MainWindow::activateArcSoftSDK() {    
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    path.append("/.cp-camera-face");
    if (!QDir(path).exists()) {
        QDir().mkdir(path);
    }
    path.append("/setting.ini");
    QFile::copy("setting.ini", path);

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
        //QMessageBox::information(this, tr("arcsoft_sdk"), tr("arcsoft_sdk_activate_success"), QMessageBox::Ok);

        //获取激活文件信息
        ASF_ActiveFileInfo activeFileInfo = { 0 };
        MRESULT afResult = arcFaceEngine.GetActiveFileInfo(activeFileInfo);
        qDebug() << "GetActiveFileInfo ret: " << afResult;
        if (afResult == MOK) {
            uint second = atoi(activeFileInfo.endTime);
            QString endTime = QDateTime::fromTime_t(second).toString(tr("format_date")); //(activeFileInfo.endTime);
            QString sdkVersion(activeFileInfo.sdkVersion);
            QString sdkInfo = QString(tr("sdk_info")).arg(endTime, sdkVersion);
            ui->statusbar->showMessage(sdkInfo);
        }
        if (faceRes == MOK) {
            faceRes = arcFaceEngine.InitEngine(ASF_DETECT_MODE_IMAGE);//Image
            qDebug() << "IMAGE模式下初始化结果: " << faceRes;

            //faceRes = arcFaceEngine->InitEngine(ASF_DETECT_MODE_VIDEO);//Video
            //qDebug() << "VIDEO模式下初始化结果: " << faceRes;
        }

        arcFaceEngine.SetLivenessThreshold(0.8f, 0.0f);
    } else if (faceRes == 0x16001) {
        QMessageBox::critical(this, tr("arcsoft_sdk"), tr("please_run_in_admin_mode"), QMessageBox::Ok);
    }  else if (faceRes == 28673) {
        QMessageBox::critical(this, tr("arcsoft_sdk"), tr("please set valide appid/sdk-key"), QMessageBox::Ok);
    } else {
        QMessageBox::critical(this, tr("arcsoft_sdk"), tr("please set valide appid/sdk-key"), QMessageBox::Ok);
    }
}

void MainWindow::menuExit() {
    qDebug() << "menuExit";
    QCoreApplication::quit();

}

void MainWindow::menuHelp() {
    qDebug() << "menuHelp";

}
void MainWindow::menuAbout() {
    qDebug() << "menuAbout";
    QString version(APP_VERSION);
    QString content =  tr("app_about_content");
    content.append("\n\nVersion: ");
    content.append(version);

    ASF_ActiveFileInfo activeFileInfo = { 0 };
    MRESULT afResult = arcFaceEngine.GetActiveFileInfo(activeFileInfo);
    qDebug() << "GetActiveFileInfo ret: " << afResult;
    if (afResult == MOK) {
        content.append("\n\nArcSoft SDK: \n");
        uint second = atoi(activeFileInfo.endTime);
        QString endTime = QDateTime::fromTime_t(second).toString(tr("format_date")); //(activeFileInfo.endTime);

        second = atoi(activeFileInfo.startTime);
        QString startTime = QDateTime::fromTime_t(second).toString(tr("format_date")); //(activeFileInfo.endTime);

        QString sdkVersion(activeFileInfo.sdkVersion);

        QString sdkType(activeFileInfo.sdkType);
        QString sdkPlatform(activeFileInfo.platform);
        QString appId(activeFileInfo.appId);

        QString sdkInfo = QString(tr("sdk_info2")).arg(startTime, endTime, sdkVersion, sdkType, sdkPlatform, appId);
        content.append(sdkInfo);
    }

    QMessageBox::about(this, tr("menu_about"), content);

}

void MainWindow::menuSettings() {
    qDebug() << "menuSettings";
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    path.append("/.cp-camera-face/setting.ini");
    qDebug() << "setting path: " << path;
    QSettings settings(path, QSettings::IniFormat);
    QString appId = settings.value("x64_free/APPID", "XXXXXXXX").toString(); // settings.value() returns QVariant
    qDebug() << "appId: " << appId;

    QString sdkKey = settings.value("x64_free/SDKKEY", "YYYYYYYY").toString(); // settings.value() returns QVariant
    qDebug() << "sdkKey: " << sdkKey;

    bool ok;
    QStringList list = LicenseDialog::getStrings(this, appId, sdkKey, &ok);
    if (ok) {
        // use list
        foreach(QString str, list) {
            qDebug() << str;
        }
        settings.setValue("x64_free/APPID", list.at(0));
        settings.setValue("x64_free/SDKKEY", list.at(1));

        activateArcSoftSDK();
    }
}

void MainWindow::menuLoad() {
    qDebug() << "menuLoad";

    QJsonArray jsonArray = commonUtil.readJson("registeredImages.json");
    qDebug() << "jsonArray: " << jsonArray.size();
    if (jsonArray.size() > 0) {
        if (arcFaceEngine.registeredFaceDataList.size() > 0) {
            clearRegisteredFaces();
        }
        arcFaceEngine.registeredFaceDataList = commonUtil.loadRegisteredImage(jsonArray);
        qDebug() << "arcFaceEngine.registeredFaceDataList size: " << arcFaceEngine.registeredFaceDataList.size();
        int i = 0;
        foreach(FaceData faceData, arcFaceEngine.registeredFaceDataList) {
            int w = registeredImageLabeList.at(i)->width();
            int h = registeredImageLabeList.at(i)->height();
            QPixmap pixmap = QPixmap::fromImage(faceData.image);
            registeredImageLabeList.at(i)->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
            QString info2 = QString::fromLocal8Bit(faceData.info);
            registeredImageLabeList.at(i)->setInfo(info2);
            i++;

        }
    }
}

void MainWindow::menuSave() {
    qDebug() << "menuSave";
    if (arcFaceEngine.registeredFaceDataList.length() > 0) {
        QJsonArray jsonArray = commonUtil.convertRegisteredImageToJson(arcFaceEngine.registeredFaceDataList);
        commonUtil.writeJson("registeredImages.json", jsonArray);
    } else {
        QMessageBox::warning(this, tr("arcsoft_sdk"), tr("no_registered_image"), QMessageBox::Ok);

    }
}
