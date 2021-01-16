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
    printf("listening...\n");

    socklen_t socklen=sizeof(struct sockaddr_in);
    while(ros::ok()){
        //accpet
        // pthread_txrx pid_new;
        sockfd_acc = accept(sockfd, (struct sockaddr *)&c_addr_in, &socklen);
        std::cout<<"Establishing connection from: "<<inet_ntoa(c_addr_in.sin_addr)<<" port: "<<c_addr_in.sin_port<<std::endl;

        //make warpper
        pid.sockfd_accpeted = sockfd_acc;
        sock_param_warpper warpper(&pid,this);
        pthread_create(&pid.recv, NULL, QServer::s_recv_entry, (void *)&warpper);
        // if(!pthread_create(&pid_new.recv, NULL, QServer::s_recv_entry, (void *)&warpper)){//success
        //     pid.push_back(pid_new);
        // }
    }
    //close(sockfd_acc);
    close(sockfd);
    return 0;
}

void *QServer::s_recv_entry(void *arg) {
    //TODO: switch server type case ...
    QServer *q_server = static_cast<QServer*>(((sock_param_warpper *)arg)->objaddr);
    switch ((((sock_param_warpper *)arg)->objaddr)->mode){
        case PoseStamped: q_server->s_recv_ps(((sock_param_warpper *)arg)->sock_param);
        // case image: q_server->s_recv_im(((sock_param_warpper *)arg)->sock_param);
    }
    return nullptr;
}

void QServer::s_recv_ps(const pthread_txrx *sock_param){
    uint8_t recv_buf[buffer_len];
    //first byte of data differs drones
    int bias = 1;

    //determine topic & frame name
    recv(sock_param->sockfd_accpeted, recv_buf, sizeof(recv_buf), 0);
    std::string pub_topic="/q2ros/ps";
    std::string fid = "drone";
    pub_topic = pub_topic + (char)recv_buf[0];
    fid = fid + (char)recv_buf[0];

    //set ros publisher
    ros::Publisher pub_ps = nh->advertise<geometry_msgs::PoseStamped>(pub_topic,10);
    std::cout<<"Start publishing ROS [PoseStamped] topic: "<<pub_topic<<std::endl;
    memset(recv_buf,0,sizeof(recv_buf));

    struct timeval tv, tv_old;
    gettimeofday(&tv_old,NULL);
    while(1){
        //Non-blocking recv()
        auto len = recv(sock_param->sockfd_accpeted, recv_buf, sizeof(recv_buf), MSG_DONTWAIT);
        if(len<0 && !(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)){
            //error occured
            std::cerr<<"ERR in recv(), errno = "<<errno<<" @Socket = "<<sock_param->sockfd_accpeted<<std::endl;
            break;
        }
        else if(len>0){
            gettimeofday(&tv,NULL);
        
            //decode here
            Vector3d pos;
            Quaterniond ori;
            pos = Decoder::DC_pos(recv_buf + bias);        
            ori = Decoder::DC_ori(recv_buf + 24 + bias);

            //generate message
            geometry_msgs::PoseStamped ps;
            ps.header.frame_id = fid;
            ps.header.stamp = ros::Time::now();
            ps.pose.position.x = pos[0];
            ps.pose.position.y = pos[1];
            ps.pose.position.z = pos[2];
            ps.pose.orientation.x = ori.x();
            ps.pose.orientation.y = ori.y();
            ps.pose.orientation.z = ori.z();
            ps.pose.orientation.w = ori.w();

            //ros publish
            pub_ps.publish(ps);
        
            //clean
            memset(recv_buf,0,sizeof(recv_buf));

            //exit: if Ctrl-C triggered or connection timed out
            gettimeofday(&tv,NULL);
            if(!ros::ok() || tv.tv_sec-tv_old.tv_sec>=5){
                break;
            }
            tv_old = tv;
        }
        /*TODO: thread will not exit if client stop first
        find a way to determine if to close socket*/
    }
    close(sock_param->sockfd_accpeted);
    std::cout<<"recv terminated @Socket = "<<sock_param->sockfd_accpeted<<". Exiting thread."<<std::endl;
    pthread_exit(0);
}

// void QServer::s_recv_im(const pthread_txrx *sock_param){
//     uint8_t recv_buf[buffer_len];
//     //first byte of data differs drones
//     int bias = 1;

//     //determine topic & frame name
//     recv(sock_param->sockfd_accpeted, recv_buf, sizeof(recv_buf), 0);
//     std::string pub_topic="/q2ros/im";
//     std::string fid = "drone";
//     pub_topic = pub_topic + (char)recv_buf[0];
//     fid = fid + (char)recv_buf[0];

//     //set ros publisher
//     image_transport::ImageTransport nh_im(*nh);
//     image_transport::Publisher pub_im = nh_im.advertise(pub_topic, 1);
//     std::cout<<"Start publishing ROS [image] topic: "<<pub_topic<<std::endl;
//     memset(recv_buf,0,sizeof(recv_buf));

//     struct timeval tv, tv_old;
//     gettimeofday(&tv_old,NULL);
//     while(1){
//         //Non-blocking recv()
//         auto len = recv(sock_param->sockfd_accpeted, recv_buf, sizeof(recv_buf), MSG_DONTWAIT);
//         if(len<0 && !(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)){
//             //error occured
//             std::cerr<<"ERR in recv(), errno = "<<errno<<" @Socket = "<<sock_param->sockfd_accpeted<<std::endl;
//             break;
//         }
//         else if(len>0){
//             gettimeofday(&tv,NULL);
        
//             //decode here
//             cv::Mat image = Decoder::DC_image(480,640,recv_buf);

//             //generate message
//             sensor_msgs::ImagePtr im_msg = cv_bridge::CvImage(std_msgs::Header(), "mono8", image).toImageMsg();

//             //ros publish
//             pub_im.publish(im_msg);
        
//             //clean
//             memset(recv_buf,0,sizeof(recv_buf));

//             //exit: if Ctrl-C triggered or connection timed out
//             gettimeofday(&tv,NULL);
//             if(!ros::ok() || tv.tv_sec-tv_old.tv_sec>=5){
//                 break;
//             }
//             tv_old = tv;
//         }
//         /*TODO: thread will not exit if client stop first
//         find a way to determine if to close socket*/
//     }
//     close(sock_param->sockfd_accpeted);
//     std::cout<<"recv terminated @Socket = "<<sock_param->sockfd_accpeted<<". Exiting thread."<<std::endl;
//     pthread_exit(0);
// }

// void QServer::s_send(const pthread_txrx *sock_param){
//     uint8_t 
//     send(sock_param->sockfd_accpeted, sendbuf, sizeof(sendbuf), 0);
//     usleep(200); OR ros::Rate(),ros::Spin()
// }
