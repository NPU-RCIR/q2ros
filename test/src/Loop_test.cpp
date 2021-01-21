#include "ros/ros.h"
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <geometry_msgs/PoseStamped.h>
#include <mav_msgs/ClusterState.h>

#include <iostream>

#include <boost/bind.hpp>

ros::Publisher pub_test;
void loop_callback(const geometry_msgs::PoseStampedConstPtr ps1,const geometry_msgs::PoseStampedConstPtr ps2,const geometry_msgs::PoseStampedConstPtr ps3);

int main(int argc, char** argv){
    ros::init(argc,argv,"q2ros_test_cluster_publisher");
    ros::NodeHandle nh;

    //subscribe message from server
    message_filters::Subscriber<geometry_msgs::PoseStamped>  sub_test0(nh,"/q2ros/ps0",1);
    message_filters::Subscriber<geometry_msgs::PoseStamped>  sub_test1(nh,"/q2ros/ps1",1);
    message_filters::Subscriber<geometry_msgs::PoseStamped>  sub_test2(nh,"/q2ros/ps2",1);
    typedef message_filters::sync_policies::ApproximateTime<geometry_msgs::PoseStamped, geometry_msgs::PoseStamped, geometry_msgs::PoseStamped> sync_policy;
    message_filters::Synchronizer<sync_policy> sync(sync_policy(20),sub_test0, sub_test1, sub_test2);
    sync.registerCallback(boost::bind(&loop_callback,_1,_2,_3));
    //test: pub clusterstate @100Hz
    pub_test = nh.advertise<mav_msgs::ClusterState>("/state", 100);
    ros::Rate loop_rate(100);
    ROS_INFO_STREAM("loopback test initialized. Waiting...");
    while(ros::ok()){
        ros::spinOnce();
        loop_rate.sleep();
    }
}

void loop_callback(const geometry_msgs::PoseStampedConstPtr ps1,const geometry_msgs::PoseStampedConstPtr ps2,const geometry_msgs::PoseStampedConstPtr ps3){
    mav_msgs::ClusterState msg;
        {
            msg.d1_state.position.x = ps1->pose.position.x;
            msg.d1_state.position.y = ps1->pose.position.y;
            msg.d1_state.position.z = ps1->pose.position.z;
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
            msg.d2_state.position.x = ps2->pose.position.x;
            msg.d2_state.position.y = ps2->pose.position.y;
            msg.d2_state.position.z = ps2->pose.position.z;
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
            msg.d3_state.position.x = ps3->pose.position.x;
            msg.d3_state.position.y = ps3->pose.position.y;
            msg.d3_state.position.z = ps3->pose.position.z;
            msg.d3_state.velocity.x = 0;
            msg.d3_state.velocity.y = 0;
            msg.d3_state.velocity.z = 0;
            msg.d3_state.acceleration.x = 0;
            msg.d3_state.acceleration.y = 0;
            msg.d3_state.acceleration.z = 0;
            msg.d3_state.force.x = 0;
            msg.d3_state.force.y = 0;
            msg.d3_state.force.z = 0;
        }
        pub_test.publish(msg);
        ROS_INFO_STREAM("message publish");
}