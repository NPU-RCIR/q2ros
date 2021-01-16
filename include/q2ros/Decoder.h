#ifndef QTCP_DECODER_H
#define QTCP_DECODER_H

// #include "lodepng/lodepng.h"

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <vector>
// #include <opencv2/highgui/highgui.hpp>

using namespace Eigen;

union convert{
    uint64_t u64_;
    double d_;
};

class Decoder{
    public:
    static Vector3d DC_pos(uint8_t *raw_data);
    static Quaterniond DC_ori(uint8_t *raw_data);
    static double DC_double(uint8_t *raw_data);
    // static cv::Mat DC_image(int height,int width,uint8_t *raw_data);
};
#endif