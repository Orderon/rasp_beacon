/// ideas:
/// - increase saturation value until only 1 line found


////////////////////////////////// Auto version!

#define _USE_MATH_DEFINES
//#include <QCoreApplication>
//#include <QDebug>
//#include <QElapsedTimer>
#include <math.h>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include <raspicam/raspicam_cv.h>

bool getBeaconAngles(float angles[4],bool valid[4]);

using namespace cv;
using namespace std;


int hueValue = 0; // red

const Point2d imgcenter(665,565);
int minSaturation = 50; // to test..
int minValue = 140; // to test...
VideoCapture cam;
raspicam::RaspiCam_Cv Camera;
int thresh1;
int thresh2;

int main()//int argc, char *argv[])
{
 //   QCoreApplication a(argc, argv);

    /****************************************************/
 //   namedWindow( "desired color", CV_WINDOW_AUTOSIZE );
      /// Create a Trackbar for user to enter threshold
 //   createTrackbar( "hueValue:", "desired color", &hueValue, 255, NULL );
 //   createTrackbar( "minSaturation:", "desired color", &minSaturation, 255, NULL );
 //   createTrackbar( "minValue:", "desired color", &minValue, 255, NULL );
 

    //set camera params
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );

    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
	Mat image;
	Camera.grab();
	Camera.retrieve(image);
	imshow("prev",image);
	waitKey(0);
    float angles[4];
    bool valid[4];
    while(getBeaconAngles(angles,valid)){
    cout << "angle red:" << angles[0] << valid[0];
    }

    Camera.release();
    return 1;
      /****************************************************/
//    return a.exec();
}

/*  Ring mask */
Mat ringMask(Mat* image){
    static int rInner = 150;
    static int rOuter = 220;//118;
    Mat mask = Mat::zeros(image->rows,image->cols,CV_8UC1);
    Mat element = getStructuringElement( 2, Size( 2*0 + 1, 2*0+1 ), Point( 0, 0 ) );


    circle(mask,imgcenter,rOuter, 255,-1);
    circle(mask,imgcenter,rInner, 0,-1);
//    imshow("mask",mask);
    return mask;
//    Mat maskedImg;
//    image->copyTo(maskedImg,mask);
/*    imshow("masked img",maskedImg);
    Mat canny;
    Canny(maskedImg, canny, thresh1, thresh2, 3);
    morphologyEx( canny, canny, MORPH_CLOSE, element );
    imshow("org canny",canny);
    floodFill(canny, cv::Point(0,0), Scalar(255));
    imshow("canny",canny);
*/

//return maskedImg;
}


/* Tries to find the line corresponding best to the given HSV values.
 * Input: hue Value
 *        minimal Saturation value: minSat
 *        minimal  Value value: minValue
 *        maximal hue range: maxRange
 *
 * Output: resulting line: lines
 * Returns true if a line has been found
 * */
bool detectOneColor(Mat image,int hueValue, int minSat,int minValue,int maxRange,Point2i* position){
    Mat hsvimage;
    cvtColor(image, hsvimage, CV_BGR2HSV);
    std::vector<Mat> hsvChannels;
    cv::split(hsvimage, hsvChannels);
    Mat hueImage = hsvChannels[0]; // [hue, saturation, value]
    int hueRange=maxRange;

    //todo: remove
    Mat result,sresult;
  //  Size size(800,600);

    // filter out all the pixels where saturation and value do not fit the limits:
    cv::Mat saturationMask = hsvChannels[1] > minSat;
    cv::Mat valueMask = hsvChannels[2] > minValue;

    // is the color within the lower hue range?
    Mat hueMask;
    inRange(hueImage, hueValue - hueRange, hueValue + hueRange, hueMask);

    // check for overflow
    if (hueValue - hueRange < 0 || hueValue + hueRange > 180)
    {
        Mat hueMaskUpper;
        int upperHueValue = hueValue + 180; //180 cause of opencv, normally its 360
        inRange(hueImage, upperHueValue - hueRange, upperHueValue + hueRange, hueMaskUpper);

        // add this mask to the other one
        hueMask = hueMask | hueMaskUpper;
    }


    //cominbe masks
    hueMask = (hueMask & saturationMask) & valueMask;
    //imshow("mask",hueMask);
    threshold(hueMask,hueMask,1,1,THRESH_BINARY);

    //find largest color blob
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( hueMask, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

    // Find the contours in the image
    Mat drawing = Mat::zeros( hueMask.size(), CV_8UC3 );


    // Find largest contour
    int index=0;
    double area= 0;
    double temp=0;
    if(contours.size()>0){
     for(int i = 0; i< contours.size();i++)
        {
         temp = contourArea(contours[i]);
         if(temp>area){
            area=temp;
            index = i;
         }
        }

     // Find center of that contour
     Moments m = moments(contours[index],false);
    // Point2i center;
     position->x = m.m10 / m.m00;
     position->y = m.m01 / m.m00;


    // No erros occured, return true
    return true;
    }
    //otherwise return false, ie. value is not valid
    else
        return false;
}

/* Searches the beacon angles for all four beacons
 * Input: -
 * Output: float angles[4] in radian of the found beacons in RGBY order
 *         bool valid[4] (true/false) of angles in RGBY order
 * Returns true if no unhandled error occures
 */
bool getBeaconAngles(float angles[4],bool valid[4]){
       static int displaycounter=0;
       Mat image,rawimage;
       //float angles[4];
       std::vector<Point2i> points;
       Point2i tempPoint;

       /******************************************** INPUT **************************************************/
       //from image
       rawimage = imread("C:/images/beacons/img0.jpg", CV_LOAD_IMAGE_COLOR);
      // ringMask(image);

       //from camera
        Camera.grab();
        Camera.retrieve(rawimage);
        


       /************************************Circular masks****************************************************/

       static Mat mask = ringMask(&rawimage);
        rawimage.copyTo(image,mask);
       /************************************Line detection****************************************************/
       /* uses detect one color to obtain one Point per color
        * if no suitable point has been found
        * declare beacon as unvalid
        * */

       // detect red /// 5 75 175 5
       if(detectOneColor(image,0,70,100,5,&tempPoint)){//0,120,130,30
       points.push_back(tempPoint);
       valid[0]=true;
       }else{
           points.push_back(Point2i{0,0});
           valid[0]=false;
       }

       //detect green 80 80 130 20
       if(detectOneColor(image,80,80,130,20,&tempPoint)){//70,50,110,30
       points.push_back(tempPoint);
       valid[1]=true;
       }else{
           points.push_back(Point2i{0,0});
           valid[1]=false;
       }

       // detect blue 110 100 115 20
       if(detectOneColor(image,110,100,115,20,&tempPoint)){//115,90,55,30
       points.push_back(tempPoint);
       valid[2]=true;
       }else{
           points.push_back(Point2i{0,0});
           valid[2]=false;
       }
       // detect yellow 30 8 200 5
       //detectOneColor(image,21,30,80,30,&temp_lines);
       if(detectOneColor(image,30,20,125,5,&tempPoint)){
       points.push_back(tempPoint);
       valid[3]=true;
       }else{
           points.push_back(Point2i{0,0});
           valid[3]=false;
       }

       /********************************************** Caculation of Angles ****************************/
       for (unsigned int i = 0; i < points.size(); ++i)
        {
            //only do something if the line is valid
           if(valid[i]){
                angles[i] = atan2(points[i].x-imgcenter.x,points[i].y-imgcenter.y);
                std::cout << "angle found for i " << i << " " << angles[i]*180/3.1415 << "\n";
                if(displaycounter>10)
                cv::circle(image,points[i],5,cv::Scalar(0, 255, 0), 5);
            }
       }
       // angles and validations have been found
       // return true
       ///TODO: remove this whole rest
       //cv::circle(image,imgcenter,40,cv::Scalar(0, 255, 0), 2);

       if(displaycounter>10){
           imshow( "output", image );
           int key = waitKey(20);
	static int j=0;
    	//save image 
	std::ostringstream oss;
	oss << "pic/img" << j << ".jpg";
	std::string name = oss.str();
    	cv::imwrite(name,rawimage);
	j++;

           if(key == 27)   //break when esc is pressed
           {
               return false;
           }
           displaycounter=0;
       }
       displaycounter++;
    return true;
}
