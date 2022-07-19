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

	//����
	MRESULT ActiveSDK(char* appID, char* sdkKey, char* activeKey);
	//��ȡ�����ļ���Ϣ�����Ի�ȡ����Ч�ڣ�
	MRESULT GetActiveFileInfo(ASF_ActiveFileInfo& activeFileInfo);
	//��ʼ������
	MRESULT InitEngine(ASF_DetectMode mode);	
	//�ͷ�����
	MRESULT UnInitEngine();	
	//�������
	MRESULT PreDetectFace(IplImage* image, ASF_SingleFaceInfo& faceRect, bool isRGB);
	//����������ȡ
	MRESULT PreExtractFeature(IplImage* image, ASF_FaceFeature& feature, ASF_SingleFaceInfo& faceRect);
	//�����ȶ�
	MRESULT FacePairMatching(MFloat &confidenceLevel, ASF_FaceFeature feature1, ASF_FaceFeature feature2, 
		ASF_CompareModel compareModel = ASF_LIFE_PHOTO);
	//���û�����ֵ
	MRESULT SetLivenessThreshold(MFloat	rgbLiveThreshold, MFloat irLiveThreshold);
	//RGBͼ���������Լ��
	MRESULT FaceASFProcess(ASF_MultiFaceInfo detectedFaces, IplImage *img, ASF_AgeInfo &ageInfo,
		ASF_GenderInfo &genderInfo, ASF_Face3DAngle &angleInfo, ASF_LivenessInfo& liveNessInfo);
	//IR������
	MRESULT FaceASFProcess_IR(ASF_MultiFaceInfo detectedFaces, IplImage *img, ASF_LivenessInfo& irLiveNessInfo);
	//��ȡ�汾��Ϣ
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
