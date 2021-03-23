/*
 *  @Author: Krasjet_Yu
 *  @Data:   2021/01/10
 *  @Brief:  This header file includes the common head files and define the common structure as well as the function ect.
 *  @Mail:   krasjet.ziping@gmail.com
*/

#ifndef GENERAL_H
#define GENERAL_H

#include <opencv2/opencv.hpp>
#include <math.h>
#include <iostream>
#include <string.h>
#include <vector>

using namespace std;
using namespace cv;
using namespace ml; // SVM算法集成在ml模块下

// extern variables
extern pthread_mutex_t Globalmutex; // threads conflict due to image-updating
extern pthread_cond_t GlobalCondCV; // threads conflict due to image-updating
extern bool imageReadable;          // threads conflict due to image-updating
extern cv::Mat src;

/**
 * @brief: imageUpdating thread
 */
void* imageUpdatingThread(void* PARAM);

/**
 * @brief: armorDetecting thread
 */ 
void* armorDetectingThread(void* PARAM);

/**
 * @brief: the types of the small and big armor  
 * https://www.runoob.com/cprogramming/c-enum.html
*/
enum ArmorType
{
    SMALL_ARMOR = 0,
    BIG_ARMOR   = 1
};

/**
 * @brief: the colors of the armor, which in order B G R 
*/
enum Color
{
    BLUE  = 0,
    GREEN = 1,
    RED   = 2
};

float getPointsDistance(const Point2f& a, const Point2f& b);

#endif //GENERAL_H