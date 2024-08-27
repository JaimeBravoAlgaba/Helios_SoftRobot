# Description: This node calculates the cable lengths from the section coordinates
#
# The node subscribes to the topic 'helios_pose_cmd' to receive the section coordinates (theta and phi) and calculates the cable lengths.
# The mounting matrix is calculated from the cable offset and the cable lengths are calculated using the mounting matrix and the section coordinates.
# The calculated cable lengths are published to the topic 'helios_cables_cmd'.
#

import rclpy
import rclpy.logging
from rclpy.node import Node
import numpy as np
from std_msgs.msg import Float32MultiArray

rc = 0.043/2.0 # Radius of the robot disks
offset = [-10/180.0*np.pi, 0, 10/180.0*np.pi] # Angle offset between the cables of consecutive sections

class KinePCC(Node):
    def __init__(self):
        super().__init__('kine_pcc')

        self.subscription = self.create_subscription(Float32MultiArray, 'helios_pose_cmd', self.calculate_kine, 10)
        self.publisher_ = self.create_publisher(Float32MultiArray, 'helios_cables_cmd', 10)

        self.i = 0

    # Private method to calculate the mouting matrix from the cable offset
    def mounting_matrix(self, offset):
        # Define the mounting matrix "A"
        A = np.array([[np.cos(offset), np.sin(offset)],
                      [np.cos(offset+np.pi*0.5), np.sin(offset+np.pi*0.5)], 
                      [np.cos(offset+np.pi), np.sin(offset+np.pi)],
                      [np.cos(offset+np.pi*1.5), np.sin(offset+np.pi*1.5)]])
        return A

    # Callback function to calculate the cable lengths from the section coordinates
    def calculate_kine(self, msg: Float32MultiArray):
        # data: [theta0, theta1, ..., thetan, phi0, phi1, ..., phin]
        theta = msg.data[0:3]
        phi = msg.data[3:6]

        print(">>New input received:")
        print("Theta:", theta)
        print("Phi:", phi)

        # Initialization: array of 12 zeros
        length_cmd = np.zeros((1, 12), float)

        # Calculate the cable lengths from the section coordinates
        # Section 0
        b = np.array([[np.cos(phi[0])], [np.sin(phi[0])]])*theta[0]
        length_cmd = self.mounting_matrix(offset[0]).dot(b)*(-rc)
        
        # Section 1
        b = b+theta[1]*np.array([[np.cos(phi[1])], [np.sin(phi[1])]])
        length_cmd = np.concatenate((length_cmd, -rc*self.mounting_matrix(offset[1]).dot(b)))

        # Section 2
        b = b+theta[2]*np.array([[np.cos(phi[2])], [np.sin(phi[2])]])
        length_cmd = np.concatenate((length_cmd, -rc*self.mounting_matrix(offset[2]).dot(b)))

        print("Length command:\n", length_cmd)
        print("<<")

        # Publish the calculated cable lengths
        self.publisher_.publish(Float32MultiArray(data=length_cmd))
        
def main(args=None):
    rclpy.init(args=args)

    kine_pcc = KinePCC()

    rclpy.spin(kine_pcc)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    kine_pcc.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()