/**
 * @Author: Krasjet_Yu
 * @Data:   2021/01/11
 * @Brief:  This cpp file define the function 'int findLights()' which is used to find lights
 * @Mail:   krasjet.ziping@gmail.com
 */ 

#include "Armor.h"

/**
 * @brief: find all the possible lights of armor
 */ 
void ArmorDetector::findLights()
{
    vector<vector<Point>> lightContours;  //candidate contours of lights roiImg
    Mat contourImg;  //image for the usage of findContours avoiding the unexpected change of itself
    srcImg_binary.copyTo(contourImg); // a copy of roiImg, contourImg
    findContours(contourImg, lightContours, 0, 2); //CV_RETR_EXTERNAL = 0, CV_CHAIN_APPROX_SIMPLE = 2
    RotatedRect lightRect;  //RotatedRect for fitElipse
    LightBar light;  //template light
    for (const auto& lightContour : lightContours) {
        if (lightContour.size() < 6) continue; //if contour's size is less than 6, then it cna not used to fitEllipse
        if (contourArea(lightContour) < armorParam.min_area) continue; //minarea of lightContour to filter some small blobs

        lightRect = fitEllipse(lightContour); //lightContour fits into a RotatedRect
        light = LightBar(lightRect); //construct to a lightBar

        if (abs(light.angle) > armorParam.max_angle) continue; //angle filter

        lights.emplace_back(light);
    }
    if (lights.size() < 2) {
        state = LIGHTS_NOT_FOUND; //if lights is less than 2, then set state not found lights
        return;
    }

    // sort the lightBars from left to right
    sort(lights.begin(), lights.end(),
        [](LightBar& a1, LightBar& a2) {
        return a1.center.x < a2.center.x; });
    state = LIGHTS_FOUND;
    return;
}
