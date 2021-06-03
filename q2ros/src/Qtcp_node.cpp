#include "q2ros/Qtcp.h"
#include <string>
#include <iostream>
#include "ros/ros.h"

int main(int argc,char **argv){
    //argv: port

    //image server
    // QServer server(640*480, atoi(argv[0]));
    //optitrack server, buffer size = 1 (which drone) + 24 (x/y/z double data) + 32 (x/y/z/w double data) = 57
    if(argc!=2){
        std::cerr<<"wrong arg. Usage: ./QTCP port"<<std::endl;
        return -1;
    }
    ros::init(argc,argv,"q2ros");
    ros::NodeHandle nh("~");

    QServer server(PoseStamped, atoi(argv[1]), 500, &nh);
    server.exec();
    return 0;
    
}
