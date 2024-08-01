from numpy import cos, sin, pi, sqrt, arange
from numpy import cos, sin, pi, arange
import numpy as np
import matplotlib.pyplot as plt

def parametric_arc(L, theta, N = 10):
    R = L/theta
    theta_i = arange(0, theta, theta/N)
    z = R*sin(theta_i)
    x = -sqrt(R**2 - z**2)+R
    y = 0
    return x,y,z

def rot_matrix(axis, angle):
    x, y, z = axis
    c = cos(angle)
    s = sin(angle)
    rot_mat = np.array([[c + x**2*(1-c), x*y*(1-c) - z*s, x*z*(1-c) + y*s],
                        [y*x*(1-c) + z*s, c + y**2*(1-c), y*z*(1-c) - x*s],
                        [z*x*(1-c) - y*s, z*y*(1-c) + x*s, c + z**2*(1-c)]])
    return rot_mat

# Example usage
L = 0.06

# Theta an phi angles for each arc
theta = [pi/2, pi/4, pi/4, pi/3, pi/2, pi/2]
phi = [pi/2, 0, pi,0, 3*pi/2, 0]

x0 = 0
y0 = 0
z0 = 0
theta_prev = 0

rot_mat = rot_matrix([0,0,1], phi[0])
end_points = []
end_points.append((x0, y0, z0))

x_combined = []
y_combined = []
z_combined = []

for i in range(6):
    x, y, z = parametric_arc(L, theta[i])

    rot_mat = np.dot(rot_mat, rot_matrix([0, 1, 0], theta_prev))
    rot_mat = np.dot(rot_mat, rot_matrix([0, 0, 1], phi[i]))

    x, y, z = np.dot(rot_mat, np.array([x, y, z], dtype=object)) + np.array([x0, y0, z0], dtype=object)

    x0 = x[len(x)-1]
    y0 = y[len(y)-1]
    z0 = z[len(z)-1]

    theta_prev = theta[i]
    
    x_combined = np.concatenate((x_combined, x))
    y_combined = np.concatenate((y_combined, y))
    z_combined = np.concatenate((z_combined, z))
    
    end_points.append((x0, y0, z0))


# Plot the arc and spline curve
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

ax.plot(x_combined, y_combined, z_combined)
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

# Axis limits
l = 6*L
ax.set_xlim([-l, l])
ax.set_ylim([-l, l])
ax.set_zlim([-l, l])

# Scatter end points
end_points = np.array(end_points)
ax.scatter(end_points[:,0], end_points[:,1], end_points[:,2], c='r', marker='o')

plt.show()