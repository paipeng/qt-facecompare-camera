#pragma once

#include "merror.h"
#include <opencv2\opencv.hpp>
#include <vector>


#if QT_VERSION>=0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QImage>

//#define PRO

#ifdef PRO
#include "pro/arcsoft_face_sdk.h"
#else
#include "free/arcsoft_face_sdk.h"
#endif

typedef struct _FaceData {
    int32_t width;
    int32_t height;
    ASF_SingleFaceInfo faceInfo;
    ASF_AgeInfo ageInfo;
    ASF_GenderInfo genderInfo;
    ASF_Face3DAngle angleInfo;
    ASF_LivenessInfo liveNessInfo;
    ASF_FaceFeature faceFeature;
} FaceData;

class ArcFaceEngine  : public QThread
{
    Q_OBJECT
public:
	ArcFaceEngine();
	~ArcFaceEngine();

	//激活
	MRESULT ActiveSDK(char* appID, char* sdkKey, char* activeKey);
	//获取激活文件信息（可以获取到有效期）
	MRESULT GetActiveFileInfo(ASF_ActiveFileInfo& activeFileInfo);
	//初始化引擎
	MRESULT InitEngine(ASF_DetectMode mode);	
	//释放引擎
	MRESULT UnInitEngine();	
	//人脸检测
	MRESULT PreDetectFace(IplImage* image, ASF_SingleFaceInfo& faceRect, bool isRGB);
	//人脸特征提取
	MRESULT PreExtractFeature(IplImage* image, ASF_FaceFeature& feature, ASF_SingleFaceInfo& faceRect);
	//人脸比对
	MRESULT FacePairMatching(MFloat &confidenceLevel, ASF_FaceFeature feature1, ASF_FaceFeature feature2, 
		ASF_CompareModel compareModel = ASF_LIFE_PHOTO);
	//设置活体阈值
	MRESULT SetLivenessThreshold(MFloat	rgbLiveThreshold, MFloat irLiveThreshold);
	//RGB图像人脸属性检测
	MRESULT FaceASFProcess(ASF_MultiFaceInfo detectedFaces, IplImage *img, ASF_AgeInfo &ageInfo,
		ASF_GenderInfo &genderInfo, ASF_Face3DAngle &angleInfo, ASF_LivenessInfo& liveNessInfo);
	//IR活体检测
	MRESULT FaceASFProcess_IR(ASF_MultiFaceInfo detectedFaces, IplImage *img, ASF_LivenessInfo& irLiveNessInfo);
	//获取版本信息
	const ASF_VERSION GetVersion();

    void registerFace(const QImage& image);
	
public slots:
    void stop();
    int faceDetect(const QImage &image, FaceData *faceData);
    void faceCompare(const QImage& image);
    void setImage(const QImage &image);

signals:
    Q_SIGNAL void updateFaceDecodeResult(int decodeState, float score);

private:
	MHandle m_hEngine;
    QMutex m_mutex;
    bool m_stop;
    bool decoding;

    void run();
    QImage faceImage;
    QImage registeredFaceImage;
    FaceData registeredFaceData;
public:
    FaceData faceData;
};
