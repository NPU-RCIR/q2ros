#ifndef QTCP_DECODER_H
#define QTCP_DECODER_H

// #include "lodepng/lodepng.h"

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <vector>
// #include <opencv2/highgui/highgui.hpp>

#include <geometry_msgs/PoseStamped.h>

using namespace Eigen;

union convert_u64_d{
    uint64_t u64_;
    double d_;
};

class Decoder{
    public:
    static geometry_msgs::PoseStamped DC_pose_stamped(uint8_t *raw_data);
    static Vector3d DC_pos(uint8_t *raw_data);
    static Quaterniond DC_ori(uint8_t *raw_data);
    static double DC_double(uint8_t *raw_data);
    // static cv::Mat DC_image(int height,int width,uint8_t *raw_data);
};
#endif