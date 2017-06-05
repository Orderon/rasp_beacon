
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include <raspicam/raspicam_cv.h>

using namespace cv;
using namespace std;

using namespace std;
using namespace cv;

int main(){

       //float angles[4];
       std::vector<Vec4i> lines;
       std::vector<Vec4i> temp_lines;

    raspicam::RaspiCam_Cv Camera;
    cv::Mat image;

    //set camera params
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3);
    //Camera.set ( CV_CAP_PROP_FPS, 1);

    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}


	//remove delay
for (int i=0;i<20;i++)
    Camera.grab();

	int showtimer = 0;
	cout << "entering loop\n";
	while(true){
		showtimer++;
	    	Camera.grab();
	    	Camera.retrieve (image);
	
		if(showtimer>20){
		       imshow( "output", image );
		       int key = waitKey(1);
		
		       if(key == 27)   //break when esc is pressed
		       {
		           return true;
		       }
		showtimer=0;
		}
		
	}//while
}//main