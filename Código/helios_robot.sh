#!/usr/bin/env bash

# Launch the agent
gnome-terminal --title="Micro-ROS Agent" -- sh -c "bash -c \"
	source /opt/ros/${ROS_DISTRO}/setup.bash;
	source /home/${USER}/microros_ws/install/setup.bash;
	ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0;
	exec bash\""

sleep 2
# Launch the sensor to pose node
gnome-terminal --title="Sensor to Pose" -- sh -c "bash -c \"
	source /opt/ros/${ROS_DISTRO}/setup.bash;
	source /home/${USER}/microros_ws/install/setup.bash;
	ros2 run helios_robot helios_robot_sensor2pose_ai;
	exec bash\""

sleep 1
# Launch the kinematics
gnome-terminal --title="KinePCC" -- sh -c "bash -c \"
	source /opt/ros/${ROS_DISTRO}/setup.bash;
	source /home/${USER}/microros_ws/install/setup.bash;
	ros2 run helios_robot helios_robot_kine_pcc;
	exec bash\""

#sleep 1
# Launch the control node
#gnome-terminal --title="Control" -- sh -c "bash -c \"
#	source /opt/ros/${ROS_DISTRO}/setup.bash;
#	source /home/${USER}/microros_ws/install/setup.bash;
#	ros2 run helios_robot helios_robot_control;
#	exec bash\""
