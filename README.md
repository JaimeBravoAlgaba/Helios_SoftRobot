![logo_name](https://user-images.githubusercontent.com/90116779/190921175-1b6f6de5-6861-49d6-b9e2-9f3607bc0cc0.png)

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

**Revisión**
-----------

Después de resetear el ESP32, deberían escucharse sus topics desde otro terminal.

Esperamos que disfrutes utilizando Helios SoftRobot. Si tienes alguna pregunta o problema, no dudes en contactarnos.