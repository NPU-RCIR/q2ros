#include "q2ros/Qtcp.h"
#include "q2ros/Encoder.h"

#include <sys/time.h>
#include <boost/bind/bind.hpp>

#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>
#include <mav_msgs/ExpState.h>
#include <mav_msgs/ClusterState.h>
#include "ros/ros.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <string>

using namespace Eigen;

void QServer::s_send_ctrl(const int sockfd_accepted){
    ros::MultiThreadedSpinner spinner(1);
    ros::Rate(100);
    ros::Subscriber sub_exp = this->nh->subscribe<mav_msgs::ClusterState>("/state",1,boost::bind(&QServer::s_send_exec,this,_1,sockfd_accepted));
    while(ros::ok()){
        spinner.spin();
    }
    std::cout<<"send terminated. Exiting thread."<<std::endl;
    pthread_exit(0);
}

void QServer::s_send_exec(const mav_msgs::ClusterStateConstPtr cstate_ptr, int sockfd_accepted){
    int flag = 0, cond = 0;
    // std::cout<<"s_send_exec() triggered."<<std::endl;

    //encode here
    std::string ec_data;
    Encoder::EC_cluster_state(*cstate_ptr,ec_data);
    // for(std::string::iterator it = ec_data.begin(); it < ec_data.end(); it++){
    //     std::cout << std::hex << (int)*it <<" ";
    // }
    // std::cout << std::endl;
    //retry 5 times
    flag = send(sockfd_accepted, ec_data.c_str(), ec_data.size(), MSG_DONTWAIT);
    // std::cout<<flag<<" "<<ec_data.size()<<" "<<sizeof(ec_data)<<std::endl;
    while(flag == -1 && cond <= 4){
        cond++;
        std::cout<<"ERR in send_exec(), send failed (retry "<<cond<<" )"<<std::endl;
        flag = send(sockfd_accepted, ec_data.c_str(), ec_data.size(), MSG_DONTWAIT);
        if(cond == 4){
            std::cout<<"send terminated. Exiting thread."<<std::endl;
            pthread_exit(0);
        }
        usleep(5000);
    }
}