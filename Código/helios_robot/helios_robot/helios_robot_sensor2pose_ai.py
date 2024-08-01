import os
import tensorflow as tf
import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32MultiArray
import yaml
from math import pi

class SensorToPoseNode(Node):
    def __init__(self):
        super().__init__('sensor_to_pose_ai_node')
        self.subscription = self.create_subscription(
            Float32MultiArray,
            'helios_sensors',
            self.sensor_callback,
            10
        )
        self.publisher = self.create_publisher(
            Float32MultiArray,
            'helios_pose',
            10
        )

        # Load the models from the .h5 files
        folder = os.getcwd() + '/microros_ws/src/helios_robot/models/'
        self.models = []
        self.models.append(tf.keras.models.load_model(folder + 'model_40.h5'))
        self.models.append(tf.keras.models.load_model(folder + 'model_41.h5'))
        self.models.append(tf.keras.models.load_model(folder + 'model_44.h5'))
        self.models.append(tf.keras.models.load_model(folder + 'model_45.h5'))
        self.models.append(tf.keras.models.load_model(folder + 'model_48.h5'))
        self.models.append(tf.keras.models.load_model(folder + 'model_4A.h5'))

        self.h0_ranges = []
        self.h1_ranges = []
        # Open yaml file with the sensor ranges
        with open(folder + 'model_ranges.yaml', 'r') as file:
            content = yaml.load(file, Loader=yaml.FullLoader)
            self.h0_ranges = content['h0_ranges']
            self.h1_ranges = content['h1_ranges']

    def normalize(self, data, min, max):
        return (data - min) / (max - min)
    
    def denormalize(self, data, min, max):
        return data * (max - min) + min
    
    def sensor_callback(self, msg):
        # Get the sensor values from the message
        sensor_values = msg.data
        
        theta = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
        phi = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
        # Predict the pose. Each model predicts two values: x and y
        for index in range(6):
            # Normalize the sensor values
            h0range = self.h0_ranges[index]
            h1range = self.h1_ranges[index]

            h0 = self.normalize(sensor_values[index*2], h0range[0], h0range[1])
            h1 = self.normalize(sensor_values[index*2+1], h1range[0], h1range[1])

            # Predict the pose
            prediction = self.models[index](tf.constant([[h0, h1]]))
            theta[index] = abs(self.denormalize(float(prediction[0][0]), -30, 30))
            phi[index] = self.denormalize(float(prediction[0][1]), -30, 30)
        
        # Create and publish the message with the predicted pose
        pose_msg = Float32MultiArray()
        pose_msg.data = theta+phi
        self.publisher.publish(pose_msg)

def main(args=None):
    rclpy.init(args=args)
    node = SensorToPoseNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()