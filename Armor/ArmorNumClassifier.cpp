/*
 * @Author: Krasjet_Yu
 * @Data:   2021/01/11
 * @Brief:  This cpp file define the ArmorNumClassifier class, realize some function used
 * @Mail:   krasjet.ziping@gmail.com
 */ 

#include "Armor.h"

ArmorNumClassifier::ArmorNumClassifier()
{

}

ArmorNumClassifier::~ArmorNumClassifier(){}

void ArmorNumClassifier::loadSvmModel(const char* model_path, Size armorImgSize)
{
    svm = StatModel::load<SVM>(model_path);
    if(svm.empty())
    {
        cout << "SVM load error! Please check the path!" << endl;
        exit(0);
    }
    this->armorImgSize = armorImgSize;
    //set dstPoints (the same to armorImgSize, as it can avoid resize armorImg)
	dstPoints[0] = Point2f(0, 0);
    dstPoints[1] = Point2f(armorImgSize.width, 0);
    dstPoints[2] = Point2f(armorImgSize.width, armorImgSize.height);
    dstPoints[3] = Point2f(0, armorImgSize.height);
}

void ArmorNumClassifier::loadImg(Mat& srcImg)
{
    //copy srcImg as warpPerspective_src
    (srcImg).copyTo(warpPerspective_src);

    //preprocess srcImg for the goal of acceleration
    cvtColor(warpPerspective_src, warpPerspective_src, 6);  //CV_BGR2GRAT=6
    threshold(warpPerspective_src, warpPerspective_src, 10, 255, THRESH_BINARY);
}

void ArmorNumClassifier::getWarpPerspectiveImg(ArmorBox& armor)
{
    //set the armor vertex as srcPoints
    for (int i = 0; i < 4; ++i)
    {
        srcPoints[i] = armor.armorVertices[i];
    }

    //get the armor image using warpPerspective
    warpPerspective_mat = getPerspectiveTransform(srcPoints, dstPoints); // get perspective transform matrix  透射变换矩阵
    warpPerspective(warpPerspective_src, warpPerspective_dst, warpPerspective_mat, armorImgSize, INTER_NEAREST, BORDER_CONSTANT, Scalar(0));
    warpPerspective_dst.copyTo(armor.armorImg);
}

void ArmorNumClassifier::getArmorNum(ArmorBox& armor)
{
    // adapt armorImg to the SVM model sample-size requirement
    p = armor.armorImg.reshape(1, 1);
    p.convertTo(p, CV_32FC1);

    //set armor number according to the result of SVM
    armor.armorNum = (int)svm->predict(p);
}
