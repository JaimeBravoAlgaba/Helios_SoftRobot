from setuptools import find_packages, setup

package_name = 'helios_robot'

setup(
    name=package_name,
    version='1.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='jaime',
    maintainer_email='jaime.bravo.algaba@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            "helios_robot_kine_pcc = helios_robot.helios_robot_kine_pcc:main",
            "helios_robot_sensor_fake = helios_robot.helios_robot_sensor_fake:main",
            "helios_robot_sensor2pose = helios_robot.helios_robot_sensor2pose:main",
            "helios_robot_sensor2pose_ai = helios_robot.helios_robot_sensor2pose_ai:main",
            "helios_robot_control = helios_robot.helios_robot_control:main",
        ],
    },
)
