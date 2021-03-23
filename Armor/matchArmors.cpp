/**
 * @Author: Krasjet_Yu
 * @Date:   2020/01/11
 * @Brief:  This cpp file define the function 'int matchArmors()' which is used to match lights into armors
 * @Mail:   krasjet.ziping@gmail.com
 */ 

#include "Armor.h"

void eraseErrorRepeatArmor(vector<ArmorBox>& armors);

bool armorCompare(const ArmorBox& a_armor, const ArmorBox& b_armor, const ArmorBox& lastArmor, const int& targetNum);

/**
 * @brief: match lights into armors
 */ 
void ArmorDetector::matchArmors()
{
    for (int i = 0; i < lights.size() - 1; ++i)
    {
        for (int j = i + 1; j < lights.size(); ++j) //just ensure every two lights be matched once
        {
            ArmorBox armor = ArmorBox(lights[i], lights[j]); //construct an armor using the matchable lights
            if (armor.isSuitableArmor())  //when the armor we constructed just now is a suitable one, extra infomation of armor
            {
                armor.l_index = i; //set index of left light
                armor.r_index = j; //set index of right light
                classifier.getWarpPerspectiveImg(armor); //set armor image
                classifier.getArmorNum(armor);  //set armor number
                armors.emplace_back(armor);   //push into armors
            }
        }
        eraseErrorRepeatArmor(armors); //delete the error armor caused by error light
    }
    if (armors.empty()) 
    {
        state = ARMOR_NOT_FOUND; //if armors is empty then set state ARMOR_NOT_FOUND
        return; //exit function
    }
    else
    {
        state = ARMOR_FOUND; //else set state ARMOR_FOUND
        return; //exit function
    }
}

/**
 * @brief: set the privious targetArmor as lastArmor and then choose the most valuable armor from current armors as targetArmor
 */ 
void ArmorDetector::setTargetArmor()
{
    if (state == ARMOR_NOT_FOUND) targetArmor = ArmorBox(); //not found armor the set a default aomor as lastArmor
    else if (state == ARMOR_FOUND) {
        ArmorBox mva  =armors[0]; //mva most valuable armor
        for (int i = 1; i < armors.size(); ++i) //for circle to select the mva
        {
            if (armorCompare(armors[i], mva, lastArmor, targetNum)) mva = armors[i];
        }
        targetArmor = mva; //set the mva as the targetArmor of this frame
    }
    lastArmor = targetArmor; //first set the targetArmor(of last frame) as lastArmor
}

/**
 * @brief: detect and delete error armor which is caused by the single lightBar
 */ 
void eraseErrorRepeatArmor(vector<ArmorBox>& armors)
{
    int length = armors.size();
    vector<ArmorBox>::iterator it = armors.begin();
    for (size_t i = 0; i < length; ++i)
        for (size_t j = i + 1; j < length; ++j)
        {
            if (armors[i].l_index == armors[j].l_index ||
                armors[i].l_index == armors[j].r_index ||
                armors[i].r_index == armors[j].l_index ||
				armors[i].r_index == armors[j].r_index)
            {
                armors[i].getDeviationAngle() > armors[j].getDeviationAngle() ? armors.erase(it + i) : armors.erase(it + j);
            } 
        }
}

/**
 *@brief: get the distance of two points(a and b)
 */
float getPointsDistance(const Point2f& a, const Point2f& b) {
    float delta_x = a.x - b.x;
    float delta_y = a.y - b.y;
    //return sqrtf(delta_x * delta_x + delta_y * delta_y);
    return sqrt(delta_x * delta_x + delta_y * delta_y);
}

/**
 * @brief: get the distance of two points(a and b)
 */ 
void setNumScore(const int& armorNum, const int& targetNum, float& armorScore)
{
    if (targetNum == 0)
    {
        if (armorNum == 1) armorScore += 1000;
        else if (armorNum == 2) armorScore += 2000;
        else if (armorNum == 3) armorScore += 3000;
        else if (armorNum == 4) armorScore += 4000;
        else if (armorNum == 5) armorScore += 5000;
        else if (armorNum == 6) armorScore += 6000;
    }
    if (armorNum == targetNum) armorScore += 100000;
}

/**
 * @brief: compare a_armor to b_armor according to their distance to lastArmor(if exit, not a default armor) and their area and armorNum
 */ 
bool armorCompare(const ArmorBox& a_armor, const ArmorBox& b_armor, const ArmorBox& lastArmor, const int& targetNum)
{
    float a_score = 0; //shooting value of a_armor
    float b_score = 0; //shooting value of b_armor
    a_score += a_armor.armorRect.area(); //area value of a a_armor
    b_score += b_armor.armorRect.area(); //area value of b b_armor

    //number(robot type) priorty
    setNumScore(a_armor.armorNum, targetNum, a_score);
    setNumScore(b_armor.armorNum, targetNum, b_score);

    if (lastArmor.armorNum != 0) //if lastArmor.armorRect is not a default armor means there is a true taegetArmor in the last frame
    {
        float a_distance = getPointsDistance(a_armor.center, lastArmor.center);
        float b_distance = getPointsDistance(b_armor.center, lastArmor.center);
        a_score -= a_distance * 2;
        b_score -= b_distance * 2;
    }
    return a_score > b_score; //judge whether a is more calueable according their score or not
}
