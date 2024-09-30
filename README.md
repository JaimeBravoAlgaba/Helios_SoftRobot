**Helios SoftRobot**
=====================================

**Instalación y Configuración**
-------------------------------

### Paso 1: Clonar el repositorio de Helios

Clona el repositorio de Helios utilizando Git:

```bash
git clone https://github.com/JaimeBravoAlgaba/Helios_SoftRobot.git
```

### Paso 2: Crear un workspace de ROS2

Crea un directorio para el workspace de ROS2 y asegúrate de que esté vacío:

```bash
mkdir -p helios_ws/src
```

### Paso 3: Instalar el paquete de helios_robot en el workspace

Copia el paquete de helios_robot al workspace de ROS2:

```bash
cp -r Helios_SoftRobot/Código/helios_robot helios_ws/src
cd helios_ws/src
```

### Paso 4: Instalar el paquete micro_ros_setup

Clona el repositorio de micro_ros_setup y asegúrate de que esté en la rama correspondiente a tu versión de ROS2:

```bash
git clone https://github.com/micro-ROS/micro_ros_setup.git -b $ROS_DISTRO
cd ..
```

### Paso 5: Compilar el workspace

Actualiza los dependencias y compila el workspace de ROS2:

```bash
rosdep update && rosdep install --from-paths src --ignore-src -y
colcon build --symlink-install
source install/local_setup.bash
```

### Paso 6: Crear un agente de MicroROS

Crea un agente de MicroROS y configura el entorno:

```bash
ros2 run micro_ros_setup create_agent_ws.sh
ros2 run micro_ros_setup build_agent.sh
source install/local_setup.sh
```

### Paso 7: Conectar el ESP32 y ejecutar el agente

Conecta el ESP32 al puerto USB y ejecuta el agente de MicroROS:

```bash
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0
```

### Paso 8: Resolución de errores comunes

Si aparece un error al conectar el ESP32, asegúrate de conceder acceso a tu usuario al puerto USB ttyUSB0:

1. Comprueba el grupo de usuarios que tiene acceso al puerto:
```bash
stat /dev/ttyUSB0
```

Debería aparecer algo como: "Gid: (   20/ dialout)"

2. Añade tu usuario al grupo "dialout" o el que haya aparecido:
```bash
sudo usermod -a -G dialout $USER
```
3. Da permisos de lectura y escritura al puerto:
```bash
sudo chmod a+rw /dev/ttyUSB0
```

Después de resetear el ESP32, deberían escucharse sus topics desde otro terminal.

**Control por terminal**
-----------
- Puede utilizarse el archivo helios_robot.sh para lanzar automáticamente los nodos necesarios:
```bash
# Lanza el agente de MicroROS, la cinemática y la predicción de pose
# En la carpeta en la que se encuentre el archivo hacer:
./helios_robot.sh
```

- Lanzar el nodo de visualización:
```bash
ros2 run helios_robot helios_robot_plot
```

- Controlar la longitud de los cables, en metros:
```bash
# [L00, L01, L02, L03, L10, L11, ... L22, L23]
ros2 pub --once helios_robot /helios_cables_cmd std_msgs/msg/Float32MultiArray "data: [0,0,0,0,  0,0,0,0,  0,0,0,0]"
```

- Enviar comando en coordenadas theta y phi, en radianes:
```bash
# [th0, th1, th2, phi0, phi1, phi2]
ros2 pub --once helios_robot /helios_sections_cmd std_msgs/msg/Float32MultiArray "data: [0,0,0,  0,0,0,]"
```

- Controlar la herramienta (dato de 0 a 255):
```bash
ros2 pub --once helios_robot /helios_cables_cmd std_msgs/msg/UInt8 "data: 255"
```