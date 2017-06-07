#include <iostream>
#include <raspicam/raspicam_cv.h>
using namespace std; 
 
int main ( int argc,char **argv ) {
   
    raspicam::RaspiCam_Cv Camera;
    cv::Mat image;
    int nCount=100;
    //set camera params
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );
    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    //Start capture
    cout<<"Capturing "<<nCount<<" frames ...."<<endl;

for(int j=0;j<20;j++){

        Camera.grab();
        Camera.retrieve ( image); 

    	//save image 
	std::ostringstream oss;
	oss << "img" << j << ".jpg";
	std::string name = oss.str();
    	cv::imwrite(name,image);
    	cout<<name<<endl;

}
   cout<<"Stop camera..."<<endl;
    Camera.release();
}
