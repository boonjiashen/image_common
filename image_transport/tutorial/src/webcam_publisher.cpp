#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sstream> // for converting the command line parameter to integer

int main(int argc, char** argv)
{
  //// Check if video source has been passed as a parameter
  //if(argv[1] == NULL) return 1;

  ros::init(argc, argv, "image_publisher");
  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub = it.advertise("camera/image_raw", 1);

  //// Convert the passed as command line parameter index for the video device to an integer
  //std::istringstream video_sourceCmd(argv[1]);
  //int video_source;
  //// Check if it is indeed a number
  //if(!(video_sourceCmd >> video_source)) {
      //std::cout << "Exiting because " << argv[1] << " is not a int\n";
      //return -1;
  //}

  int device = 0;  // 0 for one camera connected
  int imageResolutionHeight = 480;
  cv::VideoCapture cap(device);
  // Check if video device can be opened with the given index
  if(!cap.isOpened()) {
      std::cout << "Exiting because camera video source " << device << " cannot be opened\n";
      return -1;
  }
  if (!cap.set(CV_CAP_PROP_FRAME_HEIGHT, imageResolutionHeight)) {
      std:: cout << "Failed to set image resolution height to " << imageResolutionHeight << std::endl;
  }
  else {
      std:: cout << "Set image resolution height to " << imageResolutionHeight << std::endl;
  }
  cv::Mat frame;
  sensor_msgs::ImagePtr msg;

  ros::Rate loop_rate(5);

  // Display frame resolution
  cap >> frame;
  if (!frame.empty()) {
      std::cout << "Frame resolution " << frame.size() << std::endl;
  }

  while (nh.ok()) {
    cap >> frame;
    // Check if grabbed frame is actually full with some content
    if(!frame.empty()) {
      msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
      pub.publish(msg);
      cv::waitKey(1);
    }

    ros::spinOnce();
    loop_rate.sleep();
  }
}
