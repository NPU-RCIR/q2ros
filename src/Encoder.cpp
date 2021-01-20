#include "q2ros/Encoder.h"

#include <iostream>
#include <iomanip>
#include <cmath>

using namespace Eigen;

void Encoder::EC_double(const double num, std::string& ec_data){
    ec_data.clear();
    ec_data = "00000000";
    const int it_e = 8;

    //convert double to hex string, _t means translated
    std::string ec_data_t;
    if(abs(num) < 1e-6){
        for(int it = 0; it < it_e; it++)
            ec_data[it] = 0;
    }
    else{
        const convert_d_u64 d2uint(num);
        std::stringstream ss;
        ss << std::setiosflags(std::ios::uppercase) << std::hex << d2uint.u64_;
        ec_data_t = ss.str();
        //combine 2 char to 1 uint8
        for(int it = 0; it < it_e; it++){
            std::string str;
            str = ec_data_t[2*it];
            // str[1] = ec_data_t[2*it + 1];
            str += ec_data_t[2*it + 1];
            ec_data[it] = (uint8_t)stoi(str, 0, 16);
        }
    }
}

void Encoder::EC_vector3(const geometry_msgs::Vector3 gvec, std::string& ec_data){
    ec_data.clear();
    Vector3d vec(gvec.x, gvec.y, gvec.z);
    for(int it = 0; it < 3; it++){
        std::string tmp_data;
        EC_double(vec[it],tmp_data);
        ec_data += tmp_data;
    }
}

void Encoder::EC_singal_state(const mav_msgs::ExpState& state, std::string& ec_data){
    //CAUTION: Encoder does NOT encode std_msgs::Header
    ec_data.clear();
    std::string tmp_data;
    EC_vector3(state.position, tmp_data);
    ec_data += tmp_data;
    EC_vector3(state.velocity, tmp_data);
    ec_data += tmp_data;
    EC_vector3(state.acceleration, tmp_data);
    ec_data += tmp_data;
    EC_vector3(state.force, tmp_data);
    ec_data += tmp_data;
}

void Encoder::EC_cluster_state(const mav_msgs::ClusterState& cstate, std::string& ec_data){
    ec_data.clear();
    std::string tmp_data;
    EC_singal_state(cstate.d1_state, tmp_data);
    ec_data += tmp_data;
    EC_singal_state(cstate.d2_state, tmp_data);
    ec_data += tmp_data;
    EC_singal_state(cstate.d3_state, tmp_data);
    ec_data += tmp_data;
}
