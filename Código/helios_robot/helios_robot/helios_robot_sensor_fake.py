# Python code for a publisher node that publushes sensor values to the topic 'helios_sensors'.
# It generates sine functions for the values
#
import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32MultiArray
import numpy as np

class SensorFake(Node):
    def __init__(self):
        super().__init__('sensor_fake')
        self.publisher_ = self.create_publisher(Float32MultiArray, 'helios_robot_sensor', 10)
        timer_period = 0.1  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        msg = Float32MultiArray()
        msg.data = [np.sin(self.i), np.sin(self.i), np.sin(self.i), np.sin(self.i), np.sin(self.i), np.sin(self.i),
                    np.sin(self.i), np.sin(self.i), np.sin(self.i), np.sin(self.i), np.sin(self.i), np.sin(self.i)]
        self.publisher_.publish(msg)
        self.i += 0.01

def main(args=None):
    rclpy.init(args=args)
    sensor_fake = SensorFake()
    rclpy.spin(sensor_fake)
    sensor_fake.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()