#include "ros/ros.h"
#include <mav_msgs/ClusterState.h>
#include <iostream>

int main(int argc, char** argv){
    ros::init(argc,argv,"q2ros_test_cluster_publisher");
    ros::NodeHandle nh;

    //test: pub clusterstate @1Hz
    ros::Publisher pub_test = nh.advertise<mav_msgs::ClusterState>("state", 1);
    ros::Rate loop_rate(1);
    while(ros::ok()){
        mav_msgs::ClusterState msg;
        {
            msg.d1_state.position.x = 1;
            msg.d1_state.position.y = 0;
            msg.d1_state.position.z = 0;
            msg.d1_state.velocity.x = 0;
            msg.d1_state.velocity.y = 0;
            msg.d1_state.velocity.z = 0;
            msg.d1_state.acceleration.x = 0;
            msg.d1_state.acceleration.y = 0;
            msg.d1_state.acceleration.z = 0;
            msg.d1_state.force.x = 0;
            msg.d1_state.force.y = 0;
            msg.d1_state.force.z = 0;
        }
        {
            msg.d2_state.position.x = 0;
            msg.d2_state.position.y = 0;
            msg.d2_state.position.z = 0;
            msg.d2_state.velocity.x = 0;
            msg.d2_state.velocity.y = 0;
            msg.d2_state.velocity.z = 0;
            msg.d2_state.acceleration.x = 0;
            msg.d2_state.acceleration.y = 0;
            msg.d2_state.acceleration.z = 0;
            msg.d2_state.force.x = 0;
            msg.d2_state.force.y = 0;
            msg.d2_state.force.z = 0;
        }
        {
            msg.d3_state.position.x = 1;
            msg.d3_state.position.y = 0;
            msg.d3_state.position.z = 0;
            msg.d3_state.velocity.x = 0;
            msg.d3_state.velocity.y = 0;
            msg.d3_state.velocity.z = 0;
            msg.d3_state.acceleration.x = 0;
            msg.d3_state.acceleration.y = 0;
            msg.d3_state.acceleration.z = 0;
            msg.d3_state.force.x = 0;
            msg.d3_state.force.y = 0;
            msg.d3_state.force.z = 1;
        }
        //TODO: RAMDOM GENERATION
        pub_test.publish(msg);
        ROS_INFO_STREAM("msg publish");
        ros::spinOnce();
        loop_rate.sleep();
    }
}