#ifndef QTCP_QSERVER_H
#define QTCP_QSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <csignal>
#include <string>
#include <list>

#include "ros/ros.h"
// #include <image_transport/image_transport.h>
// #include <opencv2/highgui/highgui.hpp>
// #include <cv_bridge/cv_bridge.h>

#include "q2ros/Decoder.h"
#include <mav_msgs/ExpState.h>
#include <mav_msgs/ClusterState.h>

struct pthread_txrx{
    pthread_t recv;
    pthread_t send;
    int sockfd_accepted;
};

enum QServer_mode{
    PoseStamped = 0,
    image = 1
};

struct sock_param_warpper{
    pthread_txrx *sock_param;
    void *objaddr;
    sock_param_warpper(pthread_txrx *param, void *addr):sock_param(param),objaddr(addr){};
    sock_param_warpper(){};
};

// struct send_warpper{
//     const int sockfd_accepted;
//     bool isSuccess;
//     send_warpper(const int sockfd_acc=0):sockfd_accepted(sockfd_acc),isSuccess(true){};
// };

class QServer{
    public:

    QServer(QServer_mode mode_, const int port_, const int buf_len, ros::NodeHandle *nh_):mode(mode_),port(port_),buffer_len(buf_len),nh(nh_){};
    int exec();

    private:
    static void *s_recv_entry(void *arg);
    static void *s_send_entry(void *arg);
    void s_recv_ps(const int sockfd_accepted);
    // void s_recv_im(const pthread_txrx *sock_param);

    void s_send_ctrl(const int sockfd_accepted);
    // void s_send_exec(const mav_msgs::ExpState, const int& sockfd_acc, bool& isSuccess);
    void s_send_exec(const mav_msgs::ClusterStateConstPtr cstate_ptr, const int sockfd_accepted);

    struct sockaddr_in s_addr_in;
    struct sockaddr_in c_addr_in;
    const int port;
    //socket@server
    int sockfd;
    //socket accepted
    // int sockfd_acc;

    pthread_txrx pid;
    // std::list<pthread_txrx> pid;
    // int pid_it;
    
    const QServer_mode mode;
    const int buffer_len;
    //friend class Decoder;
    
    ros::NodeHandle *nh;
};

#endif
