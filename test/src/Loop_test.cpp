#include "ros/ros.h"
#include <geometry_msgs/PoseStamped.h>
#include <mav_msgs/ClusterState.h>
#include <iostream>

ros::Publisher pub_test;
void loop_callback(const geometry_msgs::PoseStampedConstPtr ps);

int main(int argc, char** argv){
    ros::init(argc,argv,"q2ros_test_cluster_publisher");
    ros::NodeHandle nh;

    //subscribe message from server
    ros::Subscriber sub_test1 = nh.subscribe<geometry_msgs::PoseStamped>("/q2ros/ps0",1,loop_callback);

    //test: pub clusterstate @100Hz
    pub_test = nh.advertise<mav_msgs::ClusterState>("/state", 100);
    ros::Rate loop_rate(100);
    while(ros::ok()){
        ros::spinOnce();
        loop_rate.sleep();
    }
}

void loop_callback(const geometry_msgs::PoseStampedConstPtr ps){
    mav_msgs::ClusterState msg;
        {
            msg.d1_state.position.x = ps->pose.position.x;
            msg.d1_state.position.y = ps->pose.position.y;
            msg.d1_state.position.z = ps->pose.position.z;
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
}