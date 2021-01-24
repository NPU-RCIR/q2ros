# Q2ROS

* 用于在 Quanser 和 Linux ROS 间传递数据的 ROS 功能包

* master 分支是11月写的验证 demo，只能从 Simulink 接收并发布 PoseStamped 消息

* server-1sim 分支新增了发送数据的线程。

* 目前在 TX2，ROS melodic 和 MATLAB 2019a，Simulink Stream Client 采样（发送）频率 100Hz 下测试通过

* 功能包目前还有很多bug，越严重的修的越快 :)

## 快速开始

### 在Linux设备上部署server：

  ```git
  cd ${CATKIN_WORKSPACE}/src
  git clone -b server-1sim https://github.com/NPU-RCIR/q2ros.git
  git clone https://github.com/NPU-RCIR/mav_msgs.git
  cd ../catkin_make
  rosrun q2ros q2ros_node 17201
  ```

### 在Simulink上部署QUARC Stream Client：

* 参考 sim/demo.slx，理论上只需要在输出端继续接 Selector 就能用了
* Stream Client 输入端是一个 bus 信号，第一位需要是一个 uini8_t 常量，用于标记无人机数量，Q2ROS 根据数据包第一位的值为每架无人机创建独立的 ROS Publisher。后续的数据按照第一架无人机的位置 (xyz) 姿态 (xyzw)，第二架无人机的位姿依次先接在Pack再接到bus上。
* Stream Client 输出端给出三架无人机期望的位姿，依次是第一架无人机期望的位置 (xyz) 速度 (xyz) 加速度 (xyz) 力 (xyz)，第二架，第三架。共27个double型。
* 详情参阅协议章节。

## 协议
* 这部分内容针对后续的维护人员，简要说明协议内容
* Q2ROS 目前跑在三个线程上，主线程负责建立连接，然后为每个客户端建立发送和接收线程

### 从 Simulink 到 ROS
* Pack 模块和 Stream Client 对输入的数据不做加密，仅编码后进行发送。对于要发送的数据，将它们按照顺序在bus上排好然后输出给 Stream Client 即可。以发送三架飞机的数据为例，数据包第一位为3，后续为编码的double数据，八位一组。Q2ROS会发布三个topic，分别为/q2ros/ps0，/q2ros/ps1，/q2ros/ps2

* **注意：Server端不会辨别飞机的编号，也就是说如果顺序接错了，1号机（048403）的数据可能被作为/q2ros/ps1或者/q2ros/ps2发布**

* 数据协议如下：

  每架无人机的位姿 (ps_data)：

  |      x       |      y       |      z       |      x       |      y       |      z       |      w       |
  | :----------: | :----------: | :----------: | :----------: | :----------: | :----------: | :----------: |
  | double pos_x | double pos_y | double pos_z | double ori_x | double ori_y | double ori_z | double ori_w |  
  
  全部TCP数据：
  
  | Eth  | TCP  | IP   | CHK  | DATA |          |          |          |      |
  | ---- | ---- | ---- | ---- | ---- | -------- | -------- | -------- | ---- |
  |      |      |      |      | bias | ps_data1 | ps_data2 | ps_data3 | ...  |  
  
  ROS话题：PoseStamped
  
  ```git
  rosmsg show geometry_msgs/PoseStamped
  ```
  
### 从ROS到Simulink

* Q2ROS 订阅名为 /State，类型为 mav_msgs/ClusterState 的话题，并发送到Simulink

  ROS话题：ClusterState，由三架飞机的ExpState组成

  ```git
  rosmsg show mav_msgs/ExpState
  rosmsg show mav_msgs/ClusterState
  ```

  每架飞机的期望状态 (expstate)：

  | position     |              |              | velocity     |              |              | acceleration |              |              | force      |            |            |
  | ------------ | ------------ | ------------ | ------------ | ------------ | ------------ | ------------ | ------------ | ------------ | ---------- | ---------- | ---------- |
  | double pos_x | double pos_y | double pos_z | double vel_x | double vel_y | double vel_z | double acc_x | double acc_y | double acc_z | double f_x | double f_y | double f_z |  
  
  全部 TCP 数据：
  
  | Eth  | TCP  | IP   | CHK  | DATA |            |            |            |
  | ---- | ---- | ---- | ---- | ---- | ---------- | ---------- | ---------- |
  |      |      |      |      | bias | exp_drone1 | exp_drone2 | exp_drone3 |
## 自定协议

* Decoder 和 Encoder 类用于对 TCP 数据进行解码和编码
* Pack 模块对数据进行编码时的字节序是**小端序**的，而 Stream Client 对收到的数据进行解码时采用的是**大端序**，在自定义编解码时需要注意
## 目前已知的bug
* 主线程的 accept() 函数是阻塞的，会把线程挂起，也就是打 Ctrl-C 没反应，要用 kill 命令杀掉
* Simulink 上发送数据频率太高时，会导致粘包，第一位数据如果解析出错，后续发布消息时会导致访问超限，访问到未定义的 ROS Publisher，core dumped
* 建议先开 Q2ROS，再开发布话题的程序，最后链接 Simulink External 模式
* Q2ROS 和 Client 触发重连时，发送数据的线程会过早退出
* Q2ROS 不区分飞机编号，这个是协议层面的问题，暂时先这么用

# Q2ROS
* ROS package for transferring message between QUARC@MATLAB and Linux ROS, using TCP server based on C++.
* The prototype (master branch) only supports receiving geometry_msgs/PoseStamped from Simulink.
* If sending messages is needed, checkout **server-1sim** branch for support.
* Currently tested on Jeson TX2, MATLAB 2019a@step==0.01s & ROS melodic.
* The package is still under constructing.
## Quick Start For RCIR MAV projects
### Linux Server
```git
cd ${CATKIN_WORKSPACE}/src
git clone -b server-1sim https://github.com/NPU-RCIR/q2ros.git
git clone https://github.com/NPU-RCIR/mav_msgs.git
cd ../catkin_make
rosrun q2ros q2ros_node 17201
```
### Simulink Client

* Position & orientation are given by QUARC *OptiTrack Trackables* in the order of (x, y, z) & (x, y, z, w). Pack these messages first in the order of drone1, drone2, ... , then pack an uint_8 message in the first byte, which means **the number of drones**.
* Server sends expected posture in the order of (pos_x, pos_y, pos_z, vel_x, vel_y, vel_z, acc_x, acc_y, acc_z, force_x, force_y, force_z) and (drone1, drone2, drone3). Check **demo.slx** and Current Protocols for more detail.

## Current Protocols

* Q2ROS runs on 3 threads, the main thread accepts connections from clients and initialize receiving and sending threads for each client.

### Simulink to ROS
* Q2ROS will publish PoseStamped messages from Simulink, and the number of topics depends on the first byte of data(the *Constant* module named drone_count). For example, if recv_data[0] comes like {0x03 0x3F 0xF0 ... } then 3 topics will be publish, named /q2ros/ps0, /q2ros/ps1 and /q2ros/ps2.

* In Simulink, assume that data is given by a *bus signal*, then *Pack* simply encode all data in the order of *bus*. Q2ROS does **NOT** distinguish aircraft's serial number, so if the data order in *bus signal* is wrong, the topic will be uncertain, like drone@048403 goes to /q2ros/ps2. To sum up, the protocol should be like:  
  ps_data:
  
  | x    | y    | z    | x | y | z | w |
  | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
  | double pos_x | double pos_y | double pos_z | double ori_x | double ori_y | double ori_z | double ori_w |  
  
  all data:
  | Eth  | TCP  |  IP  | CHK  | DATA |          |          |          |      |
  | :--: | :--: | :--: | :--: | :--: | :------: | :------: | :------: | :--: |
  |      |      |      |      | bias | ps_data1 | ps_data2 | ps_data3 | ...  |
  
  ROS message: geometry_msgs/PoseStamped
  
  ```git
  rosmsg show geometry_msgs/PoseStamped
  ```
  
### ROS to Simulink
* Q2ROS will also encode *mav_msgs/ClusterState* message from **"/State"** and send to Simulink. Use *remap* in roslaunch file to change it. The protocol goes like:

  ROS message: mav_msgs/ClusterState & mav_msgs/ExpState
  ```git
  rosmsg show mav_msgs/ExpState
  rosmsg show mav_msgs/ClusterState
  ```
  expstate:
  
  |   position   |              |              |   velocity   |              |              | acceleration |              |              |   force    |            |            |
  | :----------: | :----------: | :----------: | :----------: | :----------: | :----------: | :----------: | :----------: | :----------: | :--------: | :--------: | :--------: |
  | double pos_x | double pos_y | double pos_z | double vel_x | double vel_y | double vel_z | double acc_x | double acc_y | double acc_z | double f_x | double f_y | double f_z |  
  
  all data:
  | Eth  | TCP  |  IP  | CHK  | DATA |            |            |            |
  | :--: | :--: | :--: | :--: | :--: | :--------: | :--------: | :--------: |
  |      |      |      |      | bias | exp_drone1 | exp_drone2 | exp_drone3 |
  
## Customize Protocols
* Class Decoder and Encoder is designed to process message from client.
### Basic Data Type
* Types bellow are supported for encode & decode:

  | data type | encode | decode |
  | :-------: | :----: | :----: |
  |  double   |   BE   |   LE   |
  |  uint8_t  | BE/LE  | BE/LE  |
* Notice that *Pack* module output is **Little Endian**, while QUARC Stream Client is  **Big Endian**.


## Currently Known Bugs
* accept() will hung up the main thread since it is working in blocking mode. Main thread will be hung up when no connection is coming in, causing not responding to Ctrl-C. Use 

  ```git
  top
  kill pid_q2ros
  ```

  to terminate Q2ROS (in *zsh*, you can easily terminate it with autocomplete).

* When *Stream Client* module in Simulink samples too fast, it would send data in a really high frequency, then Q2ROS core dumped for errors in decoding.

* It is recommended to run Q2ROS first, then run your own ROS message publisher, then link the Simulink External mode.

* When reconnecting, send thread would exit prematurely.

