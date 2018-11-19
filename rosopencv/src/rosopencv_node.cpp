#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <iostream>

static const std::string TOPIC_NAME = "camera/rgb/image_raw";
static const std::string DEPTH_TOPIC_NAME = "camera/depth/image_raw";

using namespace cv;
using namespace std;


void imageCallback(const sensor_msgs::ImageConstPtr& msg) {
    try {
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");
        cv::imwrite("~/Pictures/rgb.bmp", cv_ptr->image);
        cv::imshow("view", cv_bridge::toCvShare(msg, "bgr8")->image);
        cv::waitKey(30);
        Mat image = cv_bridge::toCvShare(msg, "bgr8")->image;
        Mat outputImage;
        inRange(image, Scalar(10,10,100), Scalar(100,100,255), outputImage);
        int x1;
        x1 = countNonZero(outputImage);
        if(x1>9000){
            cout << "Victim Detected, " << "Output Value : " << x1 << endl;
        } else {
            cout << "Keep Searching ..." << endl;
        }

    } catch (cv_bridge::Exception& e) {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.",
                  msg->encoding.c_str());
    }
}

void imageDepthCallback(const sensor_msgs::ImageConstPtr& msg) {
    try {
        // Save image
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "16UC1");
        cv::Mat mat = cv_ptr->image;
        cv::Mat beConvertedMat(mat.rows, mat.cols, CV_8UC4, mat.data); // provide different view of m1 data
        cv::imwrite("~/Pictures/rgbd.bmp", beConvertedMat);
        // Show image
        cv::imshow("depthview", cv_bridge::toCvShare(msg, "16UC1")->image);
        cv::waitKey(30);
    } catch (cv_bridge::Exception& e) {
        ROS_ERROR("Could not convert from '%s' to '16UC1'.",
                  msg->encoding.c_str());
    }
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "image_transport_subscriber");
    ros::NodeHandle nh;
    cv::namedWindow("view");
    cv::namedWindow("depthview");
    cv::startWindowThread();
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe(TOPIC_NAME, 1,
                                                   imageCallback);
//    image_transport::Subscriber sub_depth = it.subscribe(DEPTH_TOPIC_NAME, 1,
//                                                         imageDepthCallback);
    ros::spin();
    cv::destroyWindow("view");
    //cv::destroyWindow("depthview");
    ros::shutdown();
    return 0;
}