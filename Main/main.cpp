#include"Armor/Armor.h"
#include"GxCamera/GxCamera.h"
#include"AngleSolver/AngleSolver.h"
#include"General/General.h"
#include<X11/Xlib.h>

pthread_t thread1;
pthread_t thread2;

// muti-threads control variables
pthread_mutex_t Globalmutex;       // threads conflict due to image-updating
pthread_cond_t GlobalCondCV;       // threads conflict due to image-updating
bool imageReadable = false;        // threads conflict due to image-updating
Mat src = Mat::zeros(600, 800, CV_8UC3);

int main(int argc, char** argv)
{
    //For MutiTHread
    XInitThreads();
    //init mutex
    pthread_mutex_init(&Globalmutex, NULL);
    //init cond
    pthread_cond_init(&GlobalCondCV, NULL);
    //create thread 1 -- image acquisition thread
    pthread_create(&thread1, NULL, imageUpdatingThread, NULL);
    //create thread 2 -- armor Detection thread
    pthread_create(&thread2, NULL, armorDetectingThread, NULL);
    //wait for children thread
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_mutex_destroy(&Globalmutex);
    return 0;
}
