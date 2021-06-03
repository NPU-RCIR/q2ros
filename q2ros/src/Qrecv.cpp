#include "q2ros/Qtcp.h"

#include <sys/time.h>

#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>
#include "ros/ros.h"

#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;

void QServer::s_recv_ps(const int sockfd_accepted){
    uint8_t recv_buf[buffer_len];
    memset(recv_buf,0,sizeof(recv_buf));
    //first byte of data differs drones
    int bias = 1;

    //determine topic & frame name
    recv(sockfd_accepted, recv_buf, sizeof(recv_buf), 0);
    std::vector<ros::Publisher> publishers((int)recv_buf[0]);
    for(int iterator = 0;iterator<(int)recv_buf[0];iterator++){
        std::string pub_topic="ps";        
        pub_topic = pub_topic + (char)(iterator+0x30);

        //set ros publisher
        publishers[iterator] = nh->advertise<geometry_msgs::PoseStamped>(pub_topic,10);
        std::cout<<"Start publishing ROS [PoseStamped] topic: "<<pub_topic<<std::endl;
    }
    memset(recv_buf,0,sizeof(recv_buf));

    struct timeval tv, tv_old;
    gettimeofday(&tv_old,NULL);
    while(1){
        //Non-blocking recv()
        auto len = recv(sockfd_accepted, recv_buf, sizeof(recv_buf), MSG_DONTWAIT);
        if(len<0 && !(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)){
            //error occured
            std::cerr<<"ERR in recv(), errno = "<<errno<<" @Socket = "<<sockfd_accepted<<std::endl;
            break;
        }
        // else if(len<(int)recv_buf[0]*7*8+1){
        //     std::cerr<<"ERR in recv(), error package length: "<<len<<" expected length: "<<(int)recv_buf[0]*7*8+1<<std::endl;
        // }
        else if(len>=(int)recv_buf[0]*7*8+1){
            gettimeofday(&tv,NULL);
            for(int iterator = 0;iterator<(int)recv_buf[0];iterator++){
                //decode here
                // Vector3d pos;
                // Quaterniond ori;
                // pos = Decoder::DC_pos(recv_buf + bias + iterator*56);        
                // ori = Decoder::DC_ori(recv_buf + 24 + bias + iterator*56);

                //generate message
                geometry_msgs::PoseStamped ps = Decoder::DC_pose_stamped(recv_buf + bias + iterator*56);
                std::string fid = "drone";
                fid = fid + (char)(iterator+0x30);
                ps.header.frame_id = fid;
                ps.header.stamp = ros::Time::now();

                // std::cout<<iterator<<std::endl;
                //ros publish
                publishers[iterator].publish(ps);
            }
            //clean
            memset(recv_buf,0,sizeof(recv_buf));

            //exit: if Ctrl-C triggered or connection timed out
            gettimeofday(&tv_old,NULL);
        }
        /*TODO: thread will not exit if client stop first
        find a way to determine if to close socket*/
        gettimeofday(&tv,NULL);
        if(!ros::ok() || tv.tv_sec-tv_old.tv_sec>=5){
            break;
        }
        usleep(1000);
    }
    close(sockfd_accepted);
    std::cout<<"recv terminated @Socket = "<<sockfd_accepted<<". Exiting thread."<<std::endl;
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