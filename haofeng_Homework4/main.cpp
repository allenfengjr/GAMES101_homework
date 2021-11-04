#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 4)
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
        << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }     
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    // TODO: Implement de Casteljau's algorithm
    cv::Point2f temp;
    std::vector<cv::Point2f> before_interpolated_control_points;
    std::vector<cv::Point2f> after_interpolated_control_points;
    before_interpolated_control_points=control_points;
    //const无法改变，复制一份
    int n=control_points.size();
    for(int i=1;i<control_points.size();i++){
        for(int j=0;j<n-1;j++){
            cv::Point2f p0,p1,p;
            p0=before_interpolated_control_points[j];
            p1=before_interpolated_control_points[j+1];
            p=(1.0f-t)*p0+t*p1;
            after_interpolated_control_points.push_back(p);
        }
        before_interpolated_control_points.clear();
        for(int j=0;j<n-1;j++){
            before_interpolated_control_points[j]=after_interpolated_control_points[j];
        }
        after_interpolated_control_points.clear();
        n-=1;
    }
    cv::Point2f ans=before_interpolated_control_points.front();
    std::cout<<"point: "<<ans.x<<" , "<<ans.y<<std::endl;
    return ans;

}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    for(double t=0.0f;t<=1.0;t+=0.0001){
        auto points=recursive_bezier(control_points,t);
        window.at<cv::Vec3b>(points.y,points.x)[1]=255;
        //反走样

        int x=points.x;int y=points.y;
        //均没有判断数组越界，不太严谨，使用周围四个像素中心点的距离平方反比确定颜色
        double d=(points.x-(x+0.5))*(points.x-(x+0.5))+(points.y-(y+0.5))*(points.y-(y+0.5));
        double d1=(points.x-(x-0.5))*(points.x-(x-0.5))+(points.y-(y+0.5))*(points.y-(y+0.5));
        double d2=(points.x-(x+0.5))*(points.x-(x+0.5))+(points.y-(y-0.5))*(points.y-(y-0.5));
        double d3=(points.x-(x-0.5))*(points.x-(x-0.5))+(points.y-(y-0.5))*(points.y-(y-0.5));
        double before=window.at<cv::Vec3b>(points.y-1,points.x-1)[1];
        window.at<cv::Vec3b>(points.y-1,points.x-1)[1]=std::max(before,255.0*(d/d3));
        before=window.at<cv::Vec3b>(points.y-1,points.x)[1];
        window.at<cv::Vec3b>(points.y-1,points.x)[1]=std::max(before,255.0*(d/d2));
        before=window.at<cv::Vec3b>(points.y,points.x-1)[1];
        window.at<cv::Vec3b>(points.y,points.x-1)[1]=std::max(before,255.0*(d/d1));

    }
}

int main() 
{
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() == 4)
        {
            //naive_bezier(control_points, window);
            bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

return 0;
}
