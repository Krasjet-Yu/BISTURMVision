/*
 * @Author: Krasjet_Yu
 * @Date:   2021/01/10
 * @Brief:  This header file declares all the classes and params used to detect/recognize enemy armors.
 * @Mail:   krasjet.ziping@gmail.com
 */
#ifndef ARMOR
#define ARMOR

#include "General/General.h"

enum DetectorState
{
    LIGHTS_NOT_FOUND = 0,
    LIGHTS_FOUND = 1,
    ARMOR_NOT_FOUND = 2,
    ARMOR_FOUND = 3
};

/**
 * @brief: params used in armor detection
 */ 
struct ArmorParam{
    int color_threshold;   //color threshold for colorImg from substract channels
    int bright_threshold;  //color threshold for brightImg 

    float min_area;        //min area of light bar
    float max_angle;       //max angle of light bar

    float max_angle_diff;  //max angle difference between two light bars
    float max_lengthDiff_ratio; //max length ratio difference between two light bars
    float max_deviation_angle;  //max deviation angle

    float max_y_diff_ratio;    //max y
    float max_x_diff_ratio;    //max x

    //default values
    ArmorParam(){
        color_threshold = 100-20;
        bright_threshold = 60;

        min_area = 50;
        max_angle = 40;

        max_angle_diff = 6;
        max_lengthDiff_ratio = 0.5;
        max_deviation_angle = 50;
        
        max_y_diff_ratio = 0.5;
        max_x_diff_ratio = 4.5;
    }
};
extern ArmorParam armorParam;

/**
 * @brief: information of lightBars
 */ 
class LightBar
{
public:
    LightBar();
    /**
     * @brief: parametrical constructor of lightBar
     * @param: RotatedRect created by fitellipse
     */ 
    LightBar(const RotatedRect& light);
    ~LightBar();
public:
    RotatedRect lightRect;  // rotation rect of light
    float length;           // length of light bar
    Point2f center;         // center of light bar
    float angle;            // angle of light bar(between height direction and vertical or between width direction and x axis, left 0~90 right 0~90)
};

/**
 * @brief: information of Armor
 */ 
class ArmorBox
{
public:
    ArmorBox();
    /**
     * @brief: Parametrical constructor of armorBox
     * @param: two LightBar
     */ 
    ArmorBox(const LightBar& l_light, const LightBar& r_light);
    ~ArmorBox();

    // angle difference: the angle difference of left and right lights
    float getAngleDiff() const;

    // deviation angle: the horizon angle of line of centers of lights
    float getDeviationAngle() const;

    // dislocation judge x: r-l light center distance ration on the x-axis. 两灯条中心在x方向上的差距比值
    float getDislocationX() const;

    // disloaction judge y: r-l light center distance ration on the Y-axis. 两灯条中心在y方向上的差距比值
    float getDislocationY() const;

    // length difference ration: the length difference ration r-l lights
    float getLengthRation() const;

    // an integrative function to judge whether this armor is suitable or not
    bool isSuitableArmor() const;

public:
    LightBar l_light, r_light; //the left and right lightbar of this armor 装甲板的左右灯条
	int l_index, r_index; //the index of left and right light 左右灯条的下标(默认为-1，仅作为ArmorDetector类成员时生效) 
	int armorNum;  //number on armor(recognized by SVM) 装甲板上的数字（用SVM识别得到）
	vector<Point2f> armorVertices;  // bl->tl->tr->br     左下 左上 右上 右下
	ArmorType type; //the type of armor
	Point2f center;	// center point(crossPoint) of armor 装甲板中心
	Rect armorRect;  //armorRect for roi 装甲板的矩形获取roi用
	float armorAngle;//armor angle(mean of lightBars) 装甲板角度(灯条角度的平均值)
	Mat armorImg;	//image of armor set by getArmorImg() from ArmorNumClassifier() 装甲板的图片（透射变换获得）
};

/**
 * @brief: use warpPerspective to get armorImg and SVM to recognize armorImg 
 */ 
class ArmorNumClassifier
{
public:
    ArmorNumClassifier();
    ~ArmorNumClassifier();

    /**
     * @brief: load the SVM model used to recognize armorNum
     * @param: the path of xml_file, the size of the training dataset ImgSize
     */ 
    void loadSvmModel(const char *model_path, Size armorImgSize = Size(40, 40));

    /**
     * @brief: load the current roiImage from ArmorDetector
     * @param: the path of xml_file
     */ 
    void loadImg(Mat& srcImg);

    /**
     * @brief: use warpPerspective to get armorImg
     * @param: the path of xml_file
     */ 
    void getWarpPerspectiveImg(ArmorBox& armor);

    /**
     * @brief: use SVM to recognize the number of each Armor
     */ 
    void getArmorNum(ArmorBox& armor);

private:
    Ptr<SVM>svm;  //svm model svm模型
	Mat p;		//preRecoginze matrix for svm 载入到SVM中识别的矩阵
	Size armorImgSize; //svm model training dataset size SVM模型的识别图片大小（训练集的图片大小）

	Mat warpPerspective_src; //warpPerspective srcImage  透射变换的原图
	Mat warpPerspective_dst; //warpPerspective dstImage   透射变换生成的目标图
	Mat warpPerspective_mat; //warpPerspective transform matrix 透射变换的变换矩阵
	Point2f srcPoints[4];   //warpPerspective srcPoints		透射变换的原图上的目标点 tl->tr->br->bl  左上 右上 右下 左下
	Point2f dstPoints[4];	//warpPerspective dstPoints     透射变换的目标图中的点   tl->tr->br->bl  左上 右上 右下 左下
};

/**
 * @brief:  Detector function to detect lights from srcImage, match light to armors,
 *          recognize ArmorNumber and select target to shot. 
 */
class ArmorDetector
{
public:
    ArmorDetector(); // 构造函数
    ~ArmorDetector(); // 析构函数

    /**
     * @brief: set the color of the opponent robots
     */
    void setEnemyColor(Color enemyColor);

    /**
     * @brief: for client, set the target armor number 
     */ 
    void setTargetNum(const int& targetNum);

    /**
     * @brief: load svm model for client
     * @param: the model file path of svm and the size of image
     */
    void loadSVM(const char* model_path, Size armorImgSize = Size(40, 40));

    /**
     * @brief copy source image to detector' srcImg
     * @param Mat& src
     */
    void setImg(Mat& src);

    /**
     * @brief: load source image and set roi if roiMode is open and found target in last frame.
     */
    void PreprocessImg();

    /**
    * @brief: find all the possible lights of armor(get lights) 
    */ 
    void findLights();

    /**
     * @brief: match lights into armors (get armors) 
     */
    void matchArmors();

    /**
     * @brief: set the privious targetArmor as lastArmor and the choose the most valuable armor from current armors as targetArmor (set target)
     */ 
    void setTargetArmor();

    /**
     * @brief: an integrative function to run the Detector
     */ 
    void run();

    /**
     * @brief:  return the Detector status
     * @return: FOUND(1) NOT_FOUND(0)
     */
    bool isFoundArmor();

    /**
     * @brief: reset the ArmorDetector(delete the privious lights and armors) to start next frame detection
     */
    void resetDetector();

    /**
	 *@brief: show all the informations of this frame detection  显示所有信息
	 */
	void showDebugInfo(bool showSrcImg_ON, bool showSrcBinary_ON, bool showLights_ON, bool showArmors_ON, bool textLights_ON, bool textArmors_ON, bool textScores_ON);

	/**
	 *@brief: get the vertices and type of target Armor for angle solver 将detector的结果输出
	 */
    void getTargetInfo(vector<Point2f> &armorVertices, Point2f &centerPoint, ArmorType &type);


private:
    Mat srcImg;  //source image (current frame acquired from camera) 从相机采集的当前的图像帧
	Mat srcImg_binary; //binary image of srcImg 源图像的二值图
	Color enemyColor;  //the color of enemy 敌方颜色
	int targetNum; //number of client's target armor 操作手设定的目标装甲板数字
	vector<LightBar> lights; //all the lightBars find in roiIng 找到的灯条
	vector<ArmorBox> armors; //all the armors matched from lights 识别到的所有装甲板
	ArmorBox targetArmor; //current target for current frame 当前图像帧对应的目标装甲板
	ArmorBox lastArmor;  //previous target for last frame 上一帧图像的目标装甲板
	ArmorNumClassifier classifier; //class used to get armorImg and classifier the armorNum 获取装甲板图像及识别装甲板数字的类
	DetectorState state; //the state of detector updating along with the program running 装甲板检测器的状态，随着装甲板进程的执行而不断更新 
};

#endif
