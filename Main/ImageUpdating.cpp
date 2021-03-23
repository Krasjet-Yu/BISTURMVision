#include "GxCamera/GxCamera.h"
#include <opencv2/opencv.hpp>
#include "General/General.h"

using namespace cv;

GxCamera camera;      // import Galaxy Camera
extern cv::Mat src;   // Transfering buffer

void* imageUpdatingThread(void* PARAM)
{
    //init camera lib
    camera.initLib();

    // open device   SN号
    camera.openDevice("KE0210010002");

    // Attention: (width-640%2=0; (height-64)%2=0; X%16=0; Y%2=0
    //    ROI          Width            Height        X      Y
    camera.setRoiParam( 640,             480,         80,    120);

    // ExposureGain             AutoExposure   AutoGain   ExposureTime  AutoExposureMin  AutoExposureMax  Gain(<=16)   AutoGainMin  AutoGainMax  GrayValue  
    camera.setExposureGainParam(    true,        true,        2000,           3000,            5000,          12,           5,          16,         127);

    // WhiteBalance               Applied?     light source type     
    camera.setWhiteBalanceParam(    true,    GX_AWB_LAMP_HOUSE_ADAPTIVE);
    
    // Acquisition Start!
    camera.acquisitionStart(&src);
}
