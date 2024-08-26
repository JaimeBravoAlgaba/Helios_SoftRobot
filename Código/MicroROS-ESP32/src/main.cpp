#include <HeliosPanel.h>
#include <micro_ros_platformio.h>

// ROS2 libraries for creating nodes, publishers, and executors
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

// Message type
#include <std_msgs/msg/int8.h>
#include <std_msgs/msg/float32_multi_array.h>
#include <std_msgs/msg/string.h>
#include "rosidl_runtime_c/string.h"

// Ensure that the transport layer being used is Arduino Serial.
// If it's not, compilation is stopped and error is printed.
#if !defined(MICRO_ROS_TRANSPORT_ARDUINO_SERIAL)
#error This project is only available for Arduino framework with serial transport.
#endif

// Define ROS2 objects for a publisher, a message, an executor, support objects, an allocator, a node, and a timer
rcl_subscription_t subscriber_lengths;
rcl_subscription_t subscriber_tool;

rcl_publisher_t publisher_sensors;
rcl_publisher_t publisher_debug;

std_msgs__msg__Float32MultiArray msg_lengths;
std_msgs__msg__Int8 msg_tool;
std_msgs__msg__Float32MultiArray msg_sensors;
std_msgs__msg__String msg_debug;

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

// Macros for checking return of ROS2 functions and entering an infinite error loop in case of error
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

TaskHandle_t motor_task;
float a[4] = {0,0,0,0};
float b[4] = {0,0,0,0};
float c[4] = {0,0,0,0};

// Infinite error loop function. If something fails, the device will get stuck here
void error_loop() {
  while(1) {
    delay(100);
  }
}

void move_sections(void* pvParameters)
{
  // Move the selected section
  moveSection(SEC0, a);
  moveSection(SEC1, b);
  moveSection(SEC2, c);
  vTaskDelete(motor_task);
}

void delta_lengths_callback(const void* msgin)
{
  std_msgs__msg__Float32MultiArray* msg = (std_msgs__msg__Float32MultiArray*)msgin;

  // Check if the length of the message is different from N_SECTIONS*N_CABLES
  if(msg_lengths.data.size != N_SECTIONS*N_CABLES){
    buzz(1, 100, 700, 100); // Buzz once to indicate that the message was not received correctly
    return;
  }
  else{
    buzz(2, 100, 100, 100); // Buzz twice to indicate that the message was received correctly
    
    for (int i = 0; i < 4; i++) {
      a[i] = msg_lengths.data.data[i];
      b[i] = msg_lengths.data.data[i+4];
      c[i] = msg_lengths.data.data[i+8];
    }

    xTaskCreatePinnedToCore(move_sections, "MoveSections", 10000, NULL, 0, &motor_task, 0);
  }
}

void tool_callback(const void* msgin)
{
  buzz(2, 100, 100, 100); // Buzz twice to indicate that the message was received correctly
  Wire.beginTransmission(I2C_TOOL_ADDR);
  Wire.write(msg_tool.data + 127);
  Wire.endTransmission();
}

void sensors_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  extern float sensorData[N_MODULES*N_SENSORS];
  updateSensors(); // Read the sensors

  msg_sensors.data.size = N_MODULES*N_SENSORS; // Set the size of the message to the number of sensors
  /*for(uint8_t i=0; i<N_MODULES*N_SENSORS; ++i)
  {
    msg_sensors.data.data[i] = sensorData[i]; // Fill the message with the sensor readings
  }*/

  // Fill the message with the sensor readings
  msg.sensors.data.data = sensorData;

  // Publish the sensor readings to the ROS2 topic
  RCCHECK(rcl_publish(&publisher_sensors, &msg_sensors, NULL));
}

void setup() {  // Initialize Robot
  initSensor();
  initActuator();

  // Turn off motors
  disableSection(SEC_ALL);

  // Initialize ROS2
  Serial.begin(115200);   // Start serial communication with a baud rate of 115200
  set_microros_serial_transports(Serial);   // Configure Micro-ROS library to use Arduino serial
  delay(2000);

  allocator = rcl_get_default_allocator();  // Get the default memory allocator provided by rcl

  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));  // Initialize rclc_support with default allocator
  
  RCCHECK(rclc_node_init_default(&node, "uros_helios_robot_node", "", &support));  // Initialize a ROS node

  RCCHECK(rclc_publisher_init_default(&publisher_sensors, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray), "helios_sensors"));
  RCCHECK(rclc_publisher_init_default(&publisher_debug, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "helios_debug"));
  
  RCCHECK(rclc_subscription_init_default(&subscriber_lengths, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray), "delta_cables_cmd"));
  RCCHECK(rclc_subscription_init_default(&subscriber_tool, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int8), "tool_cmd"));

	RCCHECK(rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(READ_DELAY), sensors_callback));

  // Create an executor. IMPORTANT: Don't forget to increase the number of handles (3) when adding a new element.  
  RCCHECK(rclc_executor_init(&executor, &support.context, 3, &allocator));
    RCCHECK(rclc_executor_add_timer(&executor, &timer));
    RCCHECK(rclc_executor_add_subscription(&executor, &subscriber_lengths, &msg_lengths, &delta_lengths_callback, ON_NEW_DATA));
    RCCHECK(rclc_executor_add_subscription(&executor, &subscriber_tool, &msg_tool, &tool_callback, ON_NEW_DATA));

  // Memory allocation - msg_lengths
  // Init the memory of your array in order to provide it to the executor.
  // If a message from ROS comes and it is bigger than this, it will be ignored, so ensure that capacities here are big enough.
  msg_lengths.data.capacity = 12; 
  msg_lengths.data.size = 0;
  msg_lengths.data.data = (float*) malloc(msg_lengths.data.capacity * sizeof(float));

  msg_lengths.layout.dim.capacity = 12;
  msg_lengths.layout.dim.size = 0;
  msg_lengths.layout.dim.data = (std_msgs__msg__MultiArrayDimension*) malloc(msg_lengths.layout.dim.capacity * sizeof(std_msgs__msg__MultiArrayDimension));

  for(size_t i = 0; i < msg_lengths.layout.dim.capacity; i++){
      msg_lengths.layout.dim.data[i].label.capacity = 12;
      msg_lengths.layout.dim.data[i].label.size = 0;
      msg_lengths.layout.dim.data[i].label.data = (char*) malloc(msg_lengths.layout.dim.data[i].label.capacity * sizeof(char));
  }

  // Memory allocation - msg_sensors
  msg_sensors.data.capacity = N_SENSORS*2; 
  msg_sensors.data.size = 0;
  msg_sensors.data.data = (float*) malloc(msg_sensors.data.capacity * sizeof(float));

  msg_sensors.layout.dim.capacity = N_SENSORS*2;
  msg_sensors.layout.dim.size = 0;
  msg_sensors.layout.dim.data = (std_msgs__msg__MultiArrayDimension*) malloc(msg_sensors.layout.dim.capacity * sizeof(std_msgs__msg__MultiArrayDimension));

  for(size_t i = 0; i < msg_sensors.layout.dim.capacity; i++){
      msg_sensors.layout.dim.data[i].label.capacity = N_SENSORS*2;
      msg_sensors.layout.dim.data[i].label.size = 0;
      msg_sensors.layout.dim.data[i].label.data = (char*) malloc(msg_sensors.layout.dim.data[i].label.capacity * sizeof(char));
  }

  // Fill the msg_debug string with a known sequence
	msg_debug.data.data = (char * ) malloc(140 * sizeof(char));
	msg_debug.data.size = 0;
	msg_debug.data.capacity = 140;

  //Check I2C devices
  for(uint8_t i=0; i<N_SENSORS; ++i)
  {
    if(i2cCheckDevice(I2C_SENSOR_ADDR[i]))
    {
      // Publish the found device to the ROS2 debug topic
      sprintf(msg_debug.data.data, "Sensor found at address 0x%x", I2C_SENSOR_ADDR[i]);
		  msg_debug.data.size = strlen(msg_debug.data.data);
		  RCSOFTCHECK(rcl_publish(&publisher_debug, &msg_debug, NULL));
    }
  }

    if(i2cCheckDevice(I2C_TOOL_ADDR))
    {
      // Publish the found device to the ROS2 debug topic
      sprintf(msg_debug.data.data, "Tool found at address 0x%x", I2C_TOOL_ADDR);
		  msg_debug.data.size = strlen(msg_debug.data.data);
		  RCSOFTCHECK(rcl_publish(&publisher_debug, &msg_debug, NULL));
    }
}

void loop() {
  // Wait a little bit
  delay(100);
  // Execute pending tasks in the executor. This will handle all ROS communications.
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}
 