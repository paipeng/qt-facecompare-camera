#pragma once

#include "ArcFaceEngine.h"
#include <string>
#include <mutex>

using namespace std;

#define NSCALE 32
#define FACENUM 5


#define FACE_FEATURE_SIZE 1032


IplImage* QImage2IplImage(const QImage *qimg) {
    IplImage *imgHeader = cvCreateImageHeader( cvSize(qimg->width(), qimg->height()), IPL_DEPTH_8U, 3);
    imgHeader->imageData = (char*) qimg->bits();

    uchar* newdata = (uchar*) malloc(sizeof(uchar) * qimg->width() * qimg->height() * 3);
    //memcpy(newdata, qimg->bits(), sizeof(uchar) * qimg->width() * qimg->height() * 3);
    for (int i = 0; i < qimg->height(); i++) {
        for (int j = 0; j < qimg->width(); j++) {
            QRgb rgb = qimg->pixel(j, i);
            newdata[i*qimg->width()*3 + j*3 + 2] = (rgb >> 16) & 0xFF;
            newdata[i*qimg->width()*3 + j*3 + 1] = (rgb >> 8) & 0xFF;
            newdata[i*qimg->width()*3 + j*3 + 0] = (rgb >> 0) & 0xFF;
        }
    }

    imgHeader->imageData = (char*) newdata;
    //cvClo
    return imgHeader;
}




void PicCutOut(IplImage* src, IplImage* dst, int x, int y) {
    if (!src || !dst) {
        return;
    }

    CvSize size = cvSize(dst->width, dst->height);//区域大小
    cvSetImageROI(src, cvRect(x, y, size.width, size.height));//设置源图像ROI
    cvCopy(src, dst); //复制图像
    cvResetImageROI(src);//源图像用完后，清空ROI
}

//颜色空间转换
int ColorSpaceConversion(IplImage* image, MInt32 format, ASVLOFFSCREEN& offscreen) {
    switch (format) {
        case ASVL_PAF_RGB24_B8G8R8:
            offscreen.u32PixelArrayFormat = (unsigned int)format;
            offscreen.i32Width = image->width;
            offscreen.i32Height = image->height;
            offscreen.pi32Pitch[0] = image->widthStep;
            offscreen.ppu8Plane[0] = (MUInt8*)image->imageData;
            break;
        case ASVL_PAF_GRAY:
            offscreen.u32PixelArrayFormat = (unsigned int)format;
            offscreen.i32Width = image->width;
            offscreen.i32Height = image->height;
            offscreen.pi32Pitch[0] = image->widthStep;
            offscreen.ppu8Plane[0] = (MUInt8*)image->imageData;
            break;
        default:
            return 0;
    }
    return 1;
}


ArcFaceEngine::ArcFaceEngine():m_stop(false), decoding(false) {

}

ArcFaceEngine::~ArcFaceEngine() {
    requestInterruption();
    m_stop = true;
    //exit(0);
    quit();
    wait();

    if (registeredFaceData.faceFeature.feature != NULL) {
        free(registeredFaceData.faceFeature.feature);
    }
}

MRESULT ArcFaceEngine::ActiveSDK(char* appID,char* sdkKey, char* activeKey)
{
#ifdef PRO
	MRESULT res = ASFOnlineActivation(appID, sdkKey, activeKey);
#else 
	MRESULT res = ASFOnlineActivation(appID, sdkKey);
#endif
	if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res)
		return res;
	return MOK;
}

MRESULT ArcFaceEngine::GetActiveFileInfo(ASF_ActiveFileInfo& activeFileInfo)
{
	MRESULT res = ASFGetActiveFileInfo(&activeFileInfo);
	return res;
}

MRESULT ArcFaceEngine::InitEngine(ASF_DetectMode detectMode)
{
	m_hEngine = NULL;
	MInt32 mask = 0;

	if (ASF_DETECT_MODE_IMAGE == detectMode)
	{
		mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS | ASF_IR_LIVENESS;
	}
	else
	{
		mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_LIVENESS | ASF_IR_LIVENESS;
	}

	MRESULT res = ASFInitEngine(detectMode, ASF_OP_0_ONLY, NSCALE, FACENUM, mask, &m_hEngine);
	return res;
}


MRESULT ArcFaceEngine::FacePairMatching(MFloat &confidenceLevel, ASF_FaceFeature feature1, 
	ASF_FaceFeature feature2, ASF_CompareModel compareModel)
{
	int res = ASFFaceFeatureCompare(m_hEngine, &feature1, &feature2, &confidenceLevel, compareModel);
	return res;
}

MRESULT ArcFaceEngine::SetLivenessThreshold(MFloat	rgbLiveThreshold, MFloat irLiveThreshold)
{
	ASF_LivenessThreshold threshold = { 0 };

	threshold.thresholdmodel_BGR = rgbLiveThreshold;
	threshold.thresholdmodel_IR = irLiveThreshold;

	int res = ASFSetLivenessParam(m_hEngine, &threshold);
	return res;
}

MRESULT ArcFaceEngine::FaceASFProcess(ASF_MultiFaceInfo detectedFaces, IplImage *img, ASF_AgeInfo &ageInfo,
	ASF_GenderInfo &genderInfo, ASF_Face3DAngle &angleInfo, ASF_LivenessInfo& liveNessInfo)
{
	if (!img)
	{
		return -1;
	}

	MInt32 lastMask = ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS;
	IplImage* cutImg = cvCreateImage(cvSize(img->width - (img->width % 4), img->height), IPL_DEPTH_8U, img->nChannels);
	PicCutOut(img, cutImg, 0, 0);

	ASVLOFFSCREEN offscreen = { 0 };
	ColorSpaceConversion(cutImg, ASVL_PAF_RGB24_B8G8R8, offscreen);
	if (!cutImg)
	{
		cvReleaseImage(&cutImg);
		return -1;
	}

	int res = ASFProcessEx(m_hEngine, &offscreen, &detectedFaces, lastMask);
	res = ASFGetAge(m_hEngine, &ageInfo);
	res = ASFGetGender(m_hEngine, &genderInfo);
	res = ASFGetFace3DAngle(m_hEngine, &angleInfo);
	res = ASFGetLivenessScore(m_hEngine, &liveNessInfo);
	cvReleaseImage(&cutImg);
	return res;
}

MRESULT ArcFaceEngine::FaceASFProcess_IR(ASF_MultiFaceInfo detectedFaces, IplImage *img, ASF_LivenessInfo& irLiveNessInfo)
{
	if (!img)
	{
		return -1;
	}

	cv::Mat grayMat;
	cv::Mat matImg(img, false);						//IplImage转Mat 设为ture为深拷贝
	cv::cvtColor(matImg, grayMat, CV_BGR2GRAY);
    IplImage* iplGrayMat = NULL;// &IplImage(grayMat);		//mat 转 IplImage 浅拷贝

	MInt32 lastMask = ASF_IR_LIVENESS;
	IplImage* cutGrayImg = cvCreateImage(cvSize(iplGrayMat->width - (iplGrayMat->width % 4), iplGrayMat->height), 
		IPL_DEPTH_8U, iplGrayMat->nChannels);
	PicCutOut(iplGrayMat, cutGrayImg, 0, 0);

	ASVLOFFSCREEN offscreen = { 0 };
	ColorSpaceConversion(cutGrayImg, ASVL_PAF_GRAY, offscreen);
	if (!cutGrayImg)
	{
		cvReleaseImage(&cutGrayImg);
		return -1;
	}

	int res = ASFProcessEx_IR(m_hEngine, &offscreen, &detectedFaces, lastMask);
	res = ASFGetLivenessScore_IR(m_hEngine, &irLiveNessInfo);
	cvReleaseImage(&cutGrayImg);
	return res;
}

const ASF_VERSION ArcFaceEngine::GetVersion()
{
	const ASF_VERSION pVersionInfo = ASFGetVersion();
	return pVersionInfo;
}

MRESULT ArcFaceEngine::PreDetectFace(IplImage* image, ASF_SingleFaceInfo& faceRect, bool isRGB)
{
	if (!image)
	{
		return -1;
	}

	IplImage* cutImg = NULL;
	MRESULT res = MOK;
	ASF_MultiFaceInfo detectedFaces = { 0 };//人脸检测
	
	if (isRGB)
	{
		cutImg = cvCreateImage(cvSize(image->width - (image->width % 4), image->height),
			IPL_DEPTH_8U, image->nChannels);

		PicCutOut(image, cutImg, 0, 0);

		ASVLOFFSCREEN offscreen = { 0 };
		ColorSpaceConversion(cutImg, ASVL_PAF_RGB24_B8G8R8, offscreen);

		res = ASFDetectFacesEx(m_hEngine, &offscreen, &detectedFaces);
	}
	else  //IR图像
	{
		cv::Mat grayMat;
		cv::Mat matImg(image, false);						//IplImage转Mat 设为ture为深拷贝
		cv::cvtColor(matImg, grayMat, CV_BGR2GRAY);
        IplImage* iplGrayMat;// = &IplImage(grayMat);		//mat 转 IplImage 浅拷贝

		cutImg = cvCreateImage(cvSize(iplGrayMat->width - (iplGrayMat->width % 4), iplGrayMat->height),
			IPL_DEPTH_8U, iplGrayMat->nChannels);

		PicCutOut(iplGrayMat, cutImg, 0, 0);

		ASVLOFFSCREEN offscreen = { 0 };
		ColorSpaceConversion(cutImg, ASVL_PAF_GRAY, offscreen);

		res = ASFDetectFacesEx(m_hEngine, &offscreen, &detectedFaces);
	}

	if (res != MOK || detectedFaces.faceNum < 1)
	{
		cvReleaseImage(&cutImg);
        return -2;
	}

	int max = 0;
	int maxArea = 0;

	for (int i = 0; i < detectedFaces.faceNum; i++)
	{
		if (detectedFaces.faceRect[i].left < 0)
			detectedFaces.faceRect[i].left = 10;
		if (detectedFaces.faceRect[i].top < 0)
			detectedFaces.faceRect[i].top = 10;
		if (detectedFaces.faceRect[i].right < 0 || detectedFaces.faceRect[i].right > cutImg->width)
			detectedFaces.faceRect[i].right = cutImg->width - 10;
		if (detectedFaces.faceRect[i].bottom < 0 || detectedFaces.faceRect[i].bottom > cutImg->height)
			detectedFaces.faceRect[i].bottom = cutImg->height - 10;

		if ((detectedFaces.faceRect[i].right - detectedFaces.faceRect[i].left)*
			(detectedFaces.faceRect[i].bottom - detectedFaces.faceRect[i].top) > maxArea)
		{
			max = i;
			maxArea = (detectedFaces.faceRect[i].right - detectedFaces.faceRect[i].left)*
				(detectedFaces.faceRect[i].bottom - detectedFaces.faceRect[i].top);
		}
	}

	faceRect.faceRect.left = detectedFaces.faceRect[max].left;
	faceRect.faceRect.top = detectedFaces.faceRect[max].top;
	faceRect.faceRect.right = detectedFaces.faceRect[max].right;
	faceRect.faceRect.bottom = detectedFaces.faceRect[max].bottom;
	faceRect.faceOrient = detectedFaces.faceOrient[max];
	cvReleaseImage(&cutImg);

	return res;
}


// 预先提取人脸特征
MRESULT ArcFaceEngine::PreExtractFeature(IplImage* image, ASF_FaceFeature& feature, ASF_SingleFaceInfo& faceRect)
{
	if (!image || image->imageData == NULL)
	{
		return -1;
	}
	IplImage* cutImg = cvCreateImage(cvSize(image->width - (image->width % 4), image->height),
		IPL_DEPTH_8U, image->nChannels);
	PicCutOut(image, cutImg, 0, 0);
	if (!cutImg)
	{
		cvReleaseImage(&cutImg);
		return -1;
	}

	MRESULT res = MOK;
	ASF_FaceFeature detectFaceFeature = { 0 };//特征值
	ASVLOFFSCREEN offscreen = { 0 };
	ColorSpaceConversion(cutImg, ASVL_PAF_RGB24_B8G8R8, offscreen);
	res = ASFFaceFeatureExtractEx(m_hEngine, &offscreen, &faceRect, &detectFaceFeature);

	if (MOK != res)
	{
		cvReleaseImage(&cutImg);
		return res;
	}

	if (!feature.feature)
	{
		return -1;
	}
	memset(feature.feature, 0, detectFaceFeature.featureSize);
	memcpy(feature.feature, detectFaceFeature.feature, detectFaceFeature.featureSize);
	cvReleaseImage(&cutImg);

	return res;
}

MRESULT ArcFaceEngine::UnInitEngine()
{
	//销毁引擎
	return ASFUninitEngine(m_hEngine);
}

void ArcFaceEngine::run() {
    qDebug()<<"From worker thread: "<<currentThreadId();
    while (1) {
        {
        QMutexLocker locker(&m_mutex);
        if (m_stop)
            break;
        }
        if (decoding) {
            faceCompare(faceImage);
            decoding = false;
        }
        //msleep(500);
    }
    qDebug()<<"From worker thread ended: "<<currentThreadId();
}


void ArcFaceEngine::setImage(const QImage &image)
{
    this->faceImage = image;
    this->decoding = true;
}

void ArcFaceEngine::stop()
{
    qDebug()<<"Thread::stop called from main thread: "<<currentThreadId();
    QMutexLocker locker(&m_mutex);
    m_stop=true;
}

int ArcFaceEngine::faceDetect(const QImage &image, FaceData *faceData) {
    qDebug()<<"ArcFaceEngine::faceDetect worker thread: "<<currentThreadId();
    int ret = 0;
    IplImage *originImage = QImage2IplImage(&image);
    faceData->width = originImage->width;
    faceData->height = originImage->height;

    //FD
    faceData->faceInfo = { 0 };
    MRESULT detectRes = PreDetectFace(originImage, faceData->faceInfo, true);
    qDebug() << "PreDetectFace: " << detectRes;
    if (MOK == detectRes) {
        qDebug() << "PreDetectFace OK";
        //age gender
        ASF_MultiFaceInfo multiFaceInfo = { 0 };
        multiFaceInfo.faceOrient = (MInt32*)malloc(sizeof(MInt32));
        multiFaceInfo.faceRect = (MRECT*)malloc(sizeof(MRECT));

        multiFaceInfo.faceNum = 1;
        multiFaceInfo.faceOrient[0] = faceData->faceInfo.faceOrient;
        multiFaceInfo.faceRect[0] = faceData->faceInfo.faceRect;

        faceData->ageInfo = { 0 };
        faceData->genderInfo = { 0 };
        faceData->angleInfo = { 0 };
        faceData->liveNessInfo = { 0 };

        //age 、gender 、3d angle 信息
        detectRes = FaceASFProcess(multiFaceInfo, originImage, faceData->ageInfo, faceData->genderInfo, faceData->angleInfo, faceData->liveNessInfo);

        if (MOK == detectRes) {
            QString showStr = QString("年龄:%1,性别:%2,活体:%3").arg(
                        QString::number(faceData->ageInfo.ageArray[0]), QString::number(faceData->genderInfo.genderArray[0]),
                    QString::number(faceData->liveNessInfo.isLive[0]));
            qDebug() << "age/gender: " << showStr;
            //FR used for face compare 1032 bytes
            faceData->faceFeature = { 0 };
            faceData->faceFeature.featureSize = FACE_FEATURE_SIZE;
            faceData->faceFeature.feature = (MByte *)malloc(faceData->faceFeature.featureSize * sizeof(MByte));
            detectRes = PreExtractFeature(originImage, faceData->faceFeature, faceData->faceInfo);

            if (MOK == detectRes) {
                qDebug() << "PreExtractFeature OK " << faceData->faceFeature.featureSize;
            } else {
                ret = -3;
            }
        } else {
            ret = -2;
        }

        free(multiFaceInfo.faceRect);
        free(multiFaceInfo.faceOrient);

    } else {
        ret = -1;
    }
    cvReleaseImage(&originImage);
    return ret;
}

void ArcFaceEngine::faceCompare(const QImage& image) {
    qDebug() << "faceCompare";
    //FaceData faceData;
    memset(&faceData, 0, sizeof(FaceData));

    int ret = faceDetect(image, &faceData);
    qDebug() << "faceDetect:" << ret;

    MFloat confidenceLevel = 0;
    if (MOK == ret) {
        // 可以选择比对模型，人证模型推荐阈值：0.82 生活照模型推荐阈值：0.80
        ret = FacePairMatching(confidenceLevel, faceData.faceFeature, registeredFaceData.faceFeature);
        if (MOK == ret) {
            qDebug() << "FacePairMatching: " << confidenceLevel;
        }

        if (faceData.faceFeature.feature != NULL) {
            free(faceData.faceFeature.feature);
        }
        qDebug() << "emit slot -> updateFaceDecodeResult";
    }
    emit updateFaceDecodeResult(ret, confidenceLevel);
}

void ArcFaceEngine::registerFace(const QImage& image) {
    // convert to opencv image IplImage
    int ret = faceDetect(image, &registeredFaceData);
    qDebug() << "registerFace:" << ret;
}

