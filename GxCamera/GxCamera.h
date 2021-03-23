//--------------------------------------------------------------------------------
/**
\file     GxCamera.h
\brief    GxCamera class declare

\date     2021-01-17
\author   Krasjet_Yu

*/
//----------------------------------------------------------------------------------


#ifndef GXCAMERA_H
#define GXCAMERA_H

#include <opencv2/opencv.hpp>
#include <unistd.h>
#include "./include/GxIAPI.h"
#include "./include/DxImageProc.h"

#define ACQ_BUFFER_NUM          5               ///< Acquisition Buffer Qty.
#define ACQ_TRANSFER_SIZE       (64 * 1024)     ///< Size of data transfer block
#define ACQ_TRANSFER_NUMBER_URB 64              ///< Qty. of data transfer block

void GetErrorString(GX_STATUS emErrorStatus);
void *ProcGetImage(void* pAcquisitionThread);
extern pthread_mutex_t Globalmutex;
extern pthread_cond_t GlobalCondCV;
extern bool imageReadable;

//Show error message
#define GX_VERIFY(emStatus) \
    if (emStatus != GX_STATUS_SUCCESS)     \
    {                                      \
        GetErrorString(emStatus);          \
        return emStatus;                   \
    }

//Show error message, close device and lib
#define GX_VERIFY_EXIT(emStatus) \
    if (emStatus != GX_STATUS_SUCCESS)     \
    {                                      \
        GetErrorString(emStatus);          \
        GXCloseDevice(g_hDevice);          \
        g_hDevice = NULL;                  \
        GXCloseLib();                      \
        printf("<App Exit!>\n");           \
        exit(emStatus);                    \
    }


///Acquisition thread param
typedef struct AcquisitionThread
{
    cv::Mat* m_pImage;           // 图像
    GX_DEV_HANDLE m_hDevice;     // 设备句柄
    uint64_t* m_pTimeStamp;      // 时间戳
    bool* g_AcquisitionFlag;     // flag
    AcquisitionThread()
    {
        m_pImage = NULL;
        m_hDevice = NULL;
        m_pTimeStamp = NULL;
        g_AcquisitionFlag = NULL;
    }
}AcquisitionThread;

/// Roi and resolution Param
typedef struct Roi
{
    int64_t              m_i64Width;                ///< Image roi width
    int64_t              m_i64Height;               ///< Image roi height
    int64_t              m_i64OffsetX;              ///< OffsetX of roi
    int64_t              m_i64OffsetY;              ///< OffsetY of roi

    Roi()
    {
        //default roi is a 640*480 area in the center
        m_i64Width = 640;             ///< Roi width  640
        m_i64Height = 480;            ///< Roi height 480
        m_i64OffsetX = 80;            ///< OffsetX 80  和Min的差必须是16的倍数
        m_i64OffsetY = 60;            ///< OffsetY 60   和Min的差必须是2的倍数
    }
}Roi;

/// Exposure and Gain Param struct
typedef struct ExposureGain
{

    bool                 m_bAutoExposure;               ///< Exposure is auto mode or not   自动（手动）曝光
    bool                 m_bAutoGain;                   ///< Gain is auto mode or not       自动（手动）增益

    double               m_dExposureTime;               ///< Exposure Time                  曝光时间
    double               m_dAutoExposureTimeMax;        ///< Maximum exposure time when using AutoExporsureTime mode   自动曝光的最大曝光时间
    double               m_dAutoExposureTimeMin;        ///< Minimum exposure time when using AutoExporsureTime mode   自动曝光的最小曝光时间

    double               m_dGain;                       ///< Gain                                     增益量
    double               m_dAutoGainMax;                ///< Maximum gain when using AutoGain mode    自动增益最大值
    double               m_dAutoGainMin;                ///< Minimum gain when using AutoGain mode    自动增益最小值

    int64_t              m_i64GrayValue;                ///< Expected gray value                      期望灰度值

    ///Default value
    ExposureGain()
    {
        m_bAutoExposure = true;              ///< Exposure is auto mode or not
        m_bAutoGain = true;                  ///< Gain is auto mode or not

        m_dExposureTime = 2000;               ///< 2000us Exposure Time
        m_dAutoExposureTimeMax = 10000;        ///< 5000us Maximum exposure time when using AutoExporsureTime mode
        m_dAutoExposureTimeMin = 500;        ///< 1000us Minimum exposure time when using AutoExporsureTime mode

        m_dGain = 6;                          ///< Gain (Maxium 16dB)
        m_dAutoGainMax = 10;                   ///< Maximum gain when using AutoGain mode
        m_dAutoGainMin = 5;                   ///< Minimum gain when using AutoGain mode

        m_i64GrayValue = 200;                 ///< Expected gray value
    }
}ExposureGain;

/// WhiteBalance
typedef struct WhiteBalance
{
    bool m_bWhiteBalance;               ///< Auto WhiteBalance is applied ?    自动（手动）白平衡
    GX_AWB_LAMP_HOUSE_ENTRY lightSource;   ///< The lamp type of environment   环境灯的类型
    WhiteBalance()
    {
        m_bWhiteBalance = false;                 ///< WhiteBalance is applied defaultly
        lightSource = GX_AWB_LAMP_HOUSE_ADAPTIVE;  ///< Auto adaptive mode
    }
}WhiteBalance;

class GxCamera
{
    //friend void *ProcGetImage(void* pMatImage);

public:
    GxCamera();
    ~GxCamera();

    /// Initialize libary
    GX_STATUS initLib();

    /// Open device and display device information
    GX_STATUS openDevice(const char* CameraSN);

    /// Main function to start the acquisition thread
    GX_STATUS acquisitionStart(cv::Mat* targetMatImg);

    /// Set camera exposure and gain
    void setExposureGainParam( bool AutoExposure,
                               bool AutoGain,
                               double ExposureTime,
                               double AutoExposureTimeMin,
                               double AutoExposureTimeMax,
                               double Gain,
                               double AutoGainMin,
                               double AutoGainMax,
                               int64_t GrayValue);

    /// Set camera roi param
    void setRoiParam( int64_t Width,
                      int64_t Height,
                      int64_t OffsetX,
                      int64_t OffsetY);

    /// Set camera WhiteBalance
    void setWhiteBalanceParam( bool WhiteBalanceOn,
                               GX_AWB_LAMP_HOUSE_ENTRY lightSource);

private:

    /// Set camera roi param
    GX_STATUS setRoi();

    /// Set camera exposure and gain
    GX_STATUS setExposureGain();

    /// Set camera WhiteBalance
    GX_STATUS setWhiteBalance();

private:
    GX_DEV_HANDLE g_hDevice;                    ///< Device handle                     设备句柄
    char* g_pDeviceSN;                          ///< Device SN number                  设备序列号
    int64_t g_i64ColorFilter;                   ///< Color filter of device            设备滤色器
    int64_t g_nPayloadSize = 0;                 ///< Payload size                      有效载荷大小
    pthread_t g_nAcquisitonThreadID;            ///< Thread ID of Acquisition thread   获取图像线程的ID

    bool g_bColorFilter;                        ///< Color filter support flag         是否使用滤色器
    bool g_bExposure;                           ///< Exposure supprot flag             是否支持曝光
    bool g_bGain;                               ///< Gain supprot flag                 是否支持增益
    bool g_bImgImprovement;                     ///< Image improvement supprot flag    是否支持图像增强
    bool g_bRoi;                                ///< Roi support flag                  是否支持ROI提取
    bool g_bWhiteBalance;                       ///< WhiteBalance support flag         是否支持白平衡
    bool g_bAcquisitionFlag;                    ///< Thread running flag               多线程运行flag

    AcquisitionThread threadParam;
    //pthread_mutex_t mutex;
    ExposureGain exposure_gain;                 ///< Camera exposure and gain param    曝光和增益的参数（结构体）
    Roi roi;                                    ///< Camera roi and resolution param   ROI
    WhiteBalance white_balance;                 ///< Camera whitebalance param         白平衡参数（结构体）
};



#endif // GXCAMERA_H
