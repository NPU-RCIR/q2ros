#ifndef QTCP_ENCODER_H
#define QTCP_ENCODER_H

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <string>
#include <geometry_msgs/Vector3.h>
#include <mav_msgs/ExpState.h>
#include <mav_msgs/ClusterState.h>

using namespace Eigen;

union convert_d_u64{
    uint64_t u64_;
    double d_;
    convert_d_u64(double d):d_(d){};
    convert_d_u64(){};
};

class Encoder{
    public:
    static void EC_cluster_state(const mav_msgs::ClusterState& cstate, std::string& ec_data);
    static void EC_singal_state(const mav_msgs::ExpState& state, std::string& ec_data);
    static void EC_vector3(const geometry_msgs::Vector3 vec, std::string& ec_data);
    static void EC_double(const double num, std::string& ec_data);
};
#endif