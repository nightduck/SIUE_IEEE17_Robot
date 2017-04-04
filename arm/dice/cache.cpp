#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

int cannyThreshold, accumulatorThreshold;
Mat cache, gray_cache;
int x,y;

int detectCircles(Mat& input, Mat& display, int cannyThreshold, int accThreshold);

int main(int argc, const char** argv)
{
  //read the file
  const char* filename = argv[1];
  cache = imread(filename,CV_LOAD_IMAGE_COLOR);
  if(!cache.data) {
    printf("Image not found\n");
    return -1;
  }

  // display input image
  //imshow("Input",cache);
  //waitKey();

  //convert image to grayscale
  if(cache.channels() > 1){
    cvtColor(cache,gray_cache,CV_BGR2GRAY);
  }
  else {
    gray_cache = cache;
  }
  
  // blur to reduce noise
  GaussianBlur(gray_cache,gray_cache,Size(9,9),2,2);
  //imshow("Blurred",gray_cache);
  //waitKey(0);
 
  cannyThreshold = 120;
  accumulatorThreshold = 30;
  
  int center;
  center = detectCircles(gray_cache,cache,cannyThreshold,accumulatorThreshold);

  return center;
}

int detectCircles(Mat& input, Mat& src, int cannyThreshold, int accThreshold){ 
  vector<Vec3f> lid;
 
  Mat display = src.clone();

  //HoughCircles(input, output,detection_method,dp,min_dist,upper_threshold,lower_threshold,min_radius,max_radius)
  HoughCircles(input,lid,CV_HOUGH_GRADIENT,1,input.rows/8,cannyThreshold,accThreshold,0,0);

  printf("Number of circles found: %lu\n\n",lid.size());

  int maxRadius = 0;
  int coordx = 0;
  int coordy = 0;
  
  for(int i = 0; i < lid.size(); i++){
    maxRadius = max(maxRadius,cvRound(lid[i][2]));
    if(maxRadius = max(maxRadius, cvRound(lid[i][2]))){
        coordx = cvRound(lid[i][0]);
        coordy = cvRound(lid[i][1]);
    }
  }

  if(maxRadius >= 70 && maxRadius <= 100){
    x = coordx;
    y = coordy;

    // color the center green
    Point centroid(x,y);
    //circle(src,center,radius,color,thickness,lineType,shift)
    circle(display,centroid,3,Scalar(0,255,0),-1,8,0);

    //color the outline red
    circle(display,centroid,maxRadius,Scalar(0,0,255),3,8,0);
 
  } 
  else {
     for(int i = 0; i < lid.size(); i++){
       Point centroid(cvRound(lid[i][0]),cvRound(lid[i][1]));
       int radius = cvRound(lid[i][2]);
   
       if (radius >= 70 && radius <= 100){
       	  x = centroid.x;
          y = centroid.y;
          break;
       }
   
       // color the center green
       //circle(src,center,radius,color,thickness,lineType,shift)
       circle(display,centroid,3,Scalar(0,255,0),-1,8,0);
   
       //color the outline red
       circle(display,centroid,radius,Scalar(0,0,255),3,8,0);
       printf("Circle at: x = %d, y = %d, radius of %d\n",centroid.x,centroid.y,radius);
     }
  }
  
  printf("\n\nDesired Circle found at: x = %d, y = %d, radius of  %d \n\n",x,y,maxRadius);
  //imshow(windowName,display);
  //waitKey();
  imwrite("botthink.jpeg",display);
  printf("Return val: 0x%x\n",(x<<10)+y);
  
  ofstream out;
  out.open("arm/dice/coordinates.txt");
  out << x << endl << y << endl;
  out.close();
  
  return ((x << 10) + y);
}
