//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
#include <algorithm>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    Eigen::Vector3f getColorBilinear(float u, float v) {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto u_img_max=std::min(int(u_img)+1,width);
        auto v_img_max=std::min(int(v_img)+1,height);
        auto u_img_min=std::max(int(u_img),0);
        auto v_img_min=std::max(int(v_img),0);
        auto s=(u_img-u_img_min);
        auto t=(v_img-v_img_min);
        auto u_img_inp=u_img_min+s*(u_img_max-u_img_min);
        auto color_0=image_data.at<cv::Vec3b>(v_img_max,u_img_inp);
        auto color_1=image_data.at<cv::Vec3b>(v_img_min,u_img_inp);
        auto color=color_1+t*(color_0-color_1);
        return Eigen::Vector3f(color[0],color[1],color[2]);
    }
    };
#endif //RASTERIZER_TEXTURE_H
