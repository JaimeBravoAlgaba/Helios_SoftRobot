import yaml
import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32MultiArray
from numpy import sin, cos, pi, arctan2, sqrt

class HeliosRobotControl(Node):
    def __init__(self):
        super().__init__('helios_robot_control')
        self.subscription = self.create_subscription(
            Float32MultiArray,
            'helios_pose_ref',
            self.pose_ref_callback,
            10
        )

        self.subscription = self.create_subscription(
            Float32MultiArray,
            'helios_pose',
            self.pose_meas_callback,
            10
        )

        self.publisher = self.create_publisher(
            Float32MultiArray,
            'delta_sections_cmd',
            10
        )

        self.theta0_prev = [0]*3
        self.theta1_prev = [0]*3

        self.theta0_ref = [0]*3
        self.theta1_ref = [0]*3

        # Load control parameters
        with open('/home/jaime/microros_ws/src/helios_robot/config/control_parameters.yaml') as file:
            params = yaml.load(file, Loader=yaml.FullLoader)
            pose_control = params['pose_control']
            self.kp = pose_control['kp']
            self.ki = pose_control['ki']
            self.kd = pose_control['kd']

    def pose_ref_callback(self, msg):
        """
        Callback function to update the pose reference.
        """     
        self.get_logger().info('Received new pose reference: %s' % msg.data)

        theta = msg.data[0:3]
        phi = msg.data[3:6]

        for i in range(3):
            self.theta0_ref = theta[i]*cos(phi[i])
            self.theta1_ref = theta[i]*sin(phi[i])

    def pose_meas_callback(self, msg):
        """
        Callback function to update the pose measurement and the position error.
        """        
        self.get_logger().info('Received pose measurement: %s' % msg.data)

        theta = msg.data[0:3]
        phi = msg.data[3:6]

        for i in range(3):
            delta_theta0 = (self.theta0_ref[i] - theta[i]*cos(phi[i]))*self.kp
            delta_theta1 = self.theta1_ref[i] - theta[i]*sin(phi[i])*self.kp

            theta[i] = sqrt(delta_theta0**2 + delta_theta1**2)
            phi[i] = arctan2(delta_theta1, delta_theta0)
        
        data = theta + phi
        self.get_logger().info('Publishing new command: %s' % data)
        
        delta_msg = Float32MultiArray()
        delta_msg.data = data
        self.publisher.publish(delta_msg)

def main(args=None):
    rclpy.init(args=args)
    helios_robot_control = HeliosRobotControl()
    rclpy.spin(helios_robot_control)
    helios_robot_control.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()