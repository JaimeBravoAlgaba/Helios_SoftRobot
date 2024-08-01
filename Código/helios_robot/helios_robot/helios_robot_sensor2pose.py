import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32MultiArray
from numpy import cos, sin, sqrt, arctan2, pi, exp
import yaml

theta_off = [0.26, 0.14, 0, 0.13, 0.05, 0.12]

class SensorToPoseNode(Node):
    def __init__(self):
        super().__init__('sensor_to_pose_node')
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

        # Load the models from the configuration file
        with open('/home/jaime/microros_ws/src/helios_robot/config/model_parameters_cone.yaml') as file:
            models = yaml.load(file, Loader=yaml.FullLoader)
            self.modules = [models['module_40'],
                            models['module_41'],
                            models['module_44'],
                            models['module_45'],
                            models['module_48'],
                            models['module_4A']]

    def normalize(self, data, min, max):
        return (data - min) / (max - min)

    def sigmoid(self, x):
        return 1 / (1 + exp(-x))

    def sensor_callback(self, msg):
        # Get the sensor values from the message
        sensor_values = msg.data
        
        theta = [0]*6
        phi = [0]*6
        # Predict the pose. Each model predicts two values: x and y
        for index in range(6):
            # Normalize the sensor values
            a = self.modules[index]['a']
            b = self.modules[index]['b']
            c = self.modules[index]['c']
            d = self.modules[index]['d']
            e = self.modules[index]['e']
            h0range = self.modules[index]['h0_range']
            h1range = self.modules[index]['h1_range']

            h0 = 2*self.normalize(sensor_values[index*2], h0range[0], h0range[1])-1
            h1 = 2*self.normalize(sensor_values[index*2+1], h1range[0], h1range[1])-1
            r = h0**2+h1**2

            if r <0.05:
                theta[index] = 0.00001
                phi[index] = 0.00001
            else:
                # Apply the model to the normalized values
                theta[index] = sqrt(abs(c*(a*(h0+d)**2+b*(h1+e)**2)))*180/pi*2
                phi[index] = arctan2(h1,h0)
        
        # Create and publish the message with the predicted pose
        pose_msg = Float32MultiArray()
        pose_msg.data = theta + phi
        self.publisher.publish(pose_msg)

def main(args=None):
    rclpy.init(args=args)
    node = SensorToPoseNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()