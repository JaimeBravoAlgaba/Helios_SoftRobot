# Code for a publisher and subscriber node
import rclpy
import rclpy.logging
from rclpy.node import Node

import numpy as np

from std_msgs.msg import Float32MultiArray

rc = 0.043/2.0 # Radius of the robot disks
offset = [-6/180.0*np.pi, 0, 6/180.0*np.pi] # Angle offset between the cables of consecutive sections

class KinePCC(Node):

    def __init__(self):
        super().__init__('kine_pcc')

        # Create a subscriber to the topic 'delta_sections_cmd' with the callback function 'callback' on new data
        self.subscription = self.create_subscription(Float32MultiArray, 'delta_sections_cmd', self.calculate_kine, 10)
        # Create a publisher to the topic 'delta_cables_cmd'
        self.publisher_ = self.create_publisher(Float32MultiArray, 'delta_cables_cmd', 10)

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
        delta_length = np.zeros((1, 12), float)

        # Calculate the cable lengths from the section coordinates
        # Section 0
        b = np.array([[np.cos(phi[0])], [np.sin(phi[0])]])*theta[0]
        delta_length = self.mounting_matrix(offset[0]).dot(b)*(-rc)
        
        # Section 1
        b = b+theta[1]*np.array([[np.cos(phi[1])], [np.sin(phi[1])]])
        delta_length = np.concatenate((delta_length, -rc*self.mounting_matrix(offset[1]).dot(b)))

        # Section 2
        b = b+theta[2]*np.array([[np.cos(phi[2])], [np.sin(phi[2])]])
        delta_length = np.concatenate((delta_length, -rc*self.mounting_matrix(offset[2]).dot(b)))

        print("Delta length:\n", delta_length)
        print("<<")

        # Publish the calculated cable lengths
        self.publisher_.publish(Float32MultiArray(data=delta_length))
        
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