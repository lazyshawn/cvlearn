/***********************************
 * 函数名称：main.cpp
 * 备    注：.h文件为C头文件; .hpp文件为C++头文件
 ***********************************/

// Realsense
#include <librealsense2/rs.hpp>
#include<librealsense2/rsutil.h>
#include <librealsense2/hpp/rs_processing.hpp>
#include <librealsense2/hpp/rs_types.hpp>
#include <librealsense2/hpp/rs_sensor.hpp>

// 会减慢编译速度, 推荐逐个添加必须的库
#include <opencv2/opencv.hpp>        // 包含OpenCV各个模块的头文件

// Rectangle recognition
#include <opencv2/highgui/highgui.hpp>    // 显示窗口,滑动条,鼠标操作以输入输出相关
#include <opencv2/imgproc/imgproc.hpp>    // 图像处理函数
#include <opencv2/core/core.hpp>    // 新式C++风格的结构以及数学运算

#include <stdio.h>
#include <iostream>

// 文件读写
#include <fstream>
#include <iomanip>

int main (int argc, char** argv) {
 
  cv::Mat src, cny, edge, img;
  std::ofstream fline;

  rs2::pipeline pipe;    // 声明Realsense管道
  rs2::frameset frames;  // 创建一个rs2::frameset对象, 包含一组帧和访问它们的接口
  rs2::colorizer color_map;    // 声明彩色图


  fline.open("./data.line");
  if(fline.is_open()) {
    fline << std::left
      << std::setw(12) << "X(0)"
      << std::setw(12) << "Y(0)"
      << std::setw(12) << "X(1)"
      << std::setw(12) << "Y(1)"
      << std::setw(12) << "AveY"
      << std::endl;
  }
  
/* **************************************************
 * 从Realsense获取图片
 * **************************************************/
  //Create a configuration for configuring the pipeline with a non default profile
  // 配置数据流信息
	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);

  // 启动设备的管道配置文件, 开始传送数据流
	rs2::pipeline_profile selection = pipe.start(cfg);

  while (true)
	{
		frames = pipe.wait_for_frames();    // 等待下一帧

		// Get each frame
    rs2::frame color_frame = frames.get_color_frame();  // 获取彩色图

		// 创建Opencv中的Mat类,并传入数据
    cv::Mat src(cv::Size(640, 480), CV_8UC3, (void*)color_frame.get_data(), 
        cv::Mat::AUTO_STEP);

    cv::imshow("Display color Image", src);
    cv::imwrite("./pic1.jpg", src);

    // 等待n ms;如果有键盘输入则退出循环; 否则n ms后跳过并执行下一个循环
    // 在退出的时候,所有数据的内存空间将会被自动释放
    if(cv::waitKey(10) >= 0) break;
	}
  cv::destroyWindow("Display color Image");


/* **************************************************
 * 图像处理
 * **************************************************/
  // Load pics
  src = cv::imread("./pic1.jpg", 1);
  if(src.empty()) {
    std::cout << "Cannot load pic. " << std::endl;
  }
  cv::imshow("Origin", src);
  cv::waitKey(0);

  // Processing
  // Canny只接受单通道8位图像, 边缘检测前先将图像转换为灰度图
  cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);         // 灰度图
  cv::GaussianBlur(img, img, cv::Size(5,5), 0, 0);    // 高斯滤波
  // Canny参数影响轮廓轮廓识别
  cv::Canny(img, img, 40, 110);
  cv::imshow("Canny", img);
  cv::waitKey(0);
  // cv::imwrite("./Canny.jpg", img);

  // 储存直线信息
  std::vector<cv::Vec4i> tline;
  std::vector<double> theta;    // 弧度
	double delta_x,delta_y;

  // 霍夫变换提取直线
	HoughLinesP(img, tline,1,CV_PI/90,14,6,16);

  // 获取直线信息
	for (size_t i = 0; i < tline.size(); i++) {
		std::cout << "tline" << tline[i] << std::endl;
    cv::line(src, cv::Point(tline[i][0],tline[i][1]), 
        cv::Point(tline[i][2],tline[i][3]), 
        cv::Scalar(0, 0, 255),2,8);

		delta_x = tline[i][2] - tline[i][0];
		delta_y = tline[i][3] - tline[i][1];      
		if(delta_y == 0) {
			theta.push_back(1.57);
			std::cout << "theta"<<i<<" = " << theta[i] << std::endl;
		}
		else {
			theta.push_back(atan2(delta_x,delta_y));
			std::cout << "theta"<<i<<" = " << theta[i] << std::endl;
		}   
    cv::imshow("LineImage", src);
    cv::waitKey(0);

    if (theta[i] > 1.5) {
      fline << std::left
        << std::setw(12) << tline[i][0]
        << std::setw(12) << tline[i][1]
        << std::setw(12) << tline[i][2]
        << std::setw(12) << tline[i][3]
        << std::setw(12) << (tline[i][1] + tline[i][3])/2
        << std::endl;
    }
	}

  return 0;
}

