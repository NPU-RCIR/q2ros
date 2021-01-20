#include "q2ros/Qtcp.h"

#include <sys/time.h>

#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>
#include "ros/ros.h"

#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;

int QServer::exec(){
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    
    memset(&s_addr_in, 0, sizeof(s_addr_in));
    s_addr_in.sin_family = AF_INET;
    s_addr_in.sin_port = htons(port);
    s_addr_in.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd,(struct sockaddr *)&s_addr_in,sizeof(struct sockaddr));
    if(listen(sockfd,10) < -1){
        printf("listen error.\n");
    }
    socklen_t socklen=sizeof(struct sockaddr_in);
    while(ros::ok()){
        //accpet
        // pthread_txrx pid_new;
        int sockfd_acc;
        printf("listening...\n");
        sockfd_acc = accept(sockfd, (struct sockaddr *)&c_addr_in, &socklen);
        std::cout<<"Establishing connection from: "<<inet_ntoa(c_addr_in.sin_addr)<<" port: "<<c_addr_in.sin_port<<std::endl;

        //make warpper
        pid.sockfd_accepted = sockfd_acc;
        sock_param_warpper warpper(&pid,(void*)this);
        pthread_create(&pid.recv, NULL, QServer::s_recv_entry, (void *)&warpper);
        pthread_detach(pid.recv);
        pthread_create(&pid.send, NULL, QServer::s_send_entry, (void *)&warpper);
        pthread_detach(pid.send);
        // if(!pthread_create(&pid_new.recv, NULL, QServer::s_recv_entry, (void *)&warpper)){//success
        //     pid.push_back(pid_new);
        // }
    }
    // close(sockfd_acc);
    close(sockfd);
    return 0;
}

void *QServer::s_recv_entry(void *arg) {
    //TODO: switch server type case ...
    QServer *q_server = static_cast<QServer*>(((sock_param_warpper *)arg)->objaddr);
    switch (static_cast<QServer*>(((sock_param_warpper *)arg)->objaddr)->mode){
        case PoseStamped: q_server->s_recv_ps(((sock_param_warpper *)arg)->sock_param->sockfd_accepted);
        // case image: q_server->s_recv_im(((sock_param_warpper *)arg)->sock_param);
    }
    return nullptr;
}

void *QServer::s_send_entry(void *arg) {
    QServer *q_server = static_cast<QServer*>(((sock_param_warpper *)arg)->objaddr);
    q_server->s_send_ctrl(((sock_param_warpper *)arg)->sock_param->sockfd_accepted);
    return nullptr;
}

