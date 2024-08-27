import os
import tensorflow as tf
import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32MultiArray
import yaml
from math import pi, sqrt, atan2

class SensorToPoseNode(Node):
    def __init__(self):
        super().__init__('sensor_to_pose_node')
        self.subscription = self.create_subscription(Float32MultiArray, 'helios_sensors', self.sensor_callback, 10)
        self.publisher = self.create_publisher(Float32MultiArray, 'helios_pose_meas', 10)

        # Load the pretrained AI models
        folder = os.getcwd() + '/helios_ws/src/helios_robot/models/'
        self.models = []
        self.models.append(tf.keras.models.load_model(folder + 'model_40.keras'))
        self.models.append(tf.keras.models.load_model(folder + 'model_41.keras'))
        self.models.append(tf.keras.models.load_model(folder + 'model_44.keras'))
        self.models.append(tf.keras.models.load_model(folder + 'model_45.keras'))
        self.models.append(tf.keras.models.load_model(folder + 'model_48.keras'))
        self.models.append(tf.keras.models.load_model(folder + 'model_4A.keras'))

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
            # Compute the average of the four sensors
            sensor_avg = (sensor_values[index*4] + sensor_values[index*4+1] + sensor_values[index*4+2] + sensor_values[index*4+3]) / 4.0

            # Append the average to the sensor values
            input_vector = sensor_values[index*4:index*4+4]
            input_vector.append(sensor_avg)

            # Predict the pose
            prediction = self.models[index](tf.constant([input_vector], dtype=tf.float32))
            euler_y = self.denormalize(float(prediction[0][0]), -60, 60)
            euler_z = self.denormalize(float(prediction[0][1]), -60, 60)

            theta[index] = sqrt(euler_y**2 + euler_z**2)
            phi[index] = atan2(euler_z, euler_y)*180/pi
        
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