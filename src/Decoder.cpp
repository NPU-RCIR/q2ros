#include "q2ros/Decoder.h"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace Eigen;

double Decoder::DC_double(uint8_t *raw_data){
    //combine 8 bit uint8 to 8 bit hex string
    const int it_e=8;
    std::string str;
    for(int it=it_e-1;it>=0;it--){
        char ctr[3];
        sprintf(ctr,"%X",(int)raw_data[it]);
        if(ctr[1]==0){
            ctr[1]='0';
        }
        str+=ctr[0];
        str+=ctr[1];
    }
    str+='\0';
    
    //convert hex string to uint64_t
    convert uint2d;
    std::stringstream ss(str);
    ss >> std::hex >> uint2d.u64_;
    const double dc_result = uint2d.d_;
    return dc_result;
}

Vector3d Decoder::DC_pos(uint8_t *raw_data){
    //client should use BIG ENDIAN
    /* CAUTION: decoder will NOT judge the length of data */
    const Vector3d dc_pos(DC_double(raw_data),DC_double(raw_data+8),DC_double(raw_data+16));
    //std::cout<<"decode position: x/y/z: "<<dc_pos[0]<<"\t"<<dc_pos[1]<<"\t"<<dc_pos[2]<<std::endl;
    return dc_pos;
}

Quaterniond Decoder::DC_ori(uint8_t *raw_data){
    //client should use BIG ENDIAN
    /* CAUTION: decoder will NOT judge the length of data */
    const Quaterniond dc_ori(DC_double(raw_data),DC_double(raw_data+8),DC_double(raw_data+16),DC_double(raw_data+24));
    //std::cout<<"decode orientation: x/y/z/w: "<<dc_ori.x()<<"\t"<<dc_ori.y()<<"\t"<<dc_ori.z()<<"\t"<<dc_ori.w()<<std::endl;
    return dc_ori;
}

// cv::Mat Decoder::DC_image(int height,int width,uint8_t *raw_data){
//     std::vector<unsigned char> image;
//     unsigned error = lodepng::decode(image, width, height, static_cast<unsigned char *>(raw_data), height*width, LCT_GREY);
//     //if there's an error, display it
//     if(error) std::cerr << "decode error " << error << ": " << lodepng_error_text(error) << std::endl;
//     return cv::Mat(height,width,CV_8UC1,image.data());
// }
