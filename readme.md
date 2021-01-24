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
git clone -b server-1sim https://e.coding.net/dradcr7/q2ros/q2ros.git
git clone https://e.coding.net/dradcr7/q2ros/mav_msgs.git
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

