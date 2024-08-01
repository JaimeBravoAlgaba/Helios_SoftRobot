from numpy import cos, sin, pi, sqrt, arange
from numpy import cos, sin, pi, arange
import numpy as np
import matplotlib.pyplot as plt

def rot_matrix(axis, angle):
    """
    Function to generate a rotation matrix for a given axis and angle.

    Parameters
    ----------
    axis : (list) - Axis of rotation
    angle: (float) - Angle of rotation[rads]

    Returns
    -------
    rot_mat : (numpy array) - Rotation matrix
    """

    x, y, z = axis
    c = cos(angle)
    s = sin(angle)
    rot_mat = np.array([[c + x**2*(1-c), x*y*(1-c) - z*s, x*z*(1-c) + y*s],
                        [y*x*(1-c) + z*s, c + y**2*(1-c), y*z*(1-c) - x*s],
                        [z*x*(1-c) - y*s, z*y*(1-c) + x*s, c + z**2*(1-c)]])
    return rot_mat

def parametric_arc(L, theta, N = 10):
    """
    Function to generate 3D points of an arc of a circle, contained in X-Z plane.

    Parameters
    ----------
    L : (float) - Length of the arc
    theta: (float) - Bending angle of the arc [rads]
    N : (int) - Number of points to generate

    Returns
    -------
    x, y, z : (numpy arrays) - 3D points of the arc
    """
    R = L/theta if theta != 0 else L
    theta_i = arange(0, theta, theta/N)
    z = R*sin(theta_i)
    x = -sqrt(R**2 - z**2)+R
    y = 0
    return x,y,z

def parametric_pcc(L, theta, phi, N = 10, p0 = [0,0,0]):
    """
    Function to generate chained arcs of circles in 3D space.

    Parameters
    ----------
    L : (list) - Lengths of the arcs
    theta: (list) - Bending angles of the arcs [rads]
    phi: (list) - Rotation angles of the arcs [rads]
    N : (int) - Number of points to generate for each arc. Default: 10
    p0 : (list) - Starting point of the chain. Default: [0,0,0]

    Returns
    -------
    x, y, z : (numpy arrays) - 3D points of the chained arcs
    knots : (list) - Joint points between arcs    
    """
    
    theta_prev = 0
    rot_mat = rot_matrix([0,0,1], phi[0])

    x_combined = []
    y_combined = []
    z_combined = []
    knots = []
    knots.append(p0)

    for i in range(6):
        x, y, z = parametric_arc(L[i], theta[i], N)

        rot_mat = np.dot(rot_mat, rot_matrix([0, 1, 0], theta_prev))
        rot_mat = np.dot(rot_mat, rot_matrix([0, 0, 1], phi[i]))

        x, y, z = np.dot(rot_mat, np.array([x, y, z], dtype=object)) + np.array(p0, dtype=object)

        p0 = np.array([x[-1], y[-1], z[-1]])

        theta_prev = theta[i]
        
        x_combined = np.concatenate((x_combined, x))
        y_combined = np.concatenate((y_combined, y))
        z_combined = np.concatenate((z_combined, z))
        
        knots.append(p0)

    return x_combined, y_combined, z_combined, knots

def main():
    # Example usage: Chained arcs of circles

    # L, theta an phi angles for each arc
    L = [8, 6, 4, 4, 2, 2]
    theta = [pi/2, pi/4, pi/4, pi/3, pi/2, pi/2]
    phi = [pi/2, 0, pi,0, 3*pi/2, 0]

    x, y, z, knots = parametric_pcc(L, theta, phi, N = 10, p0 = [0,0,0])

    # Plot the arc and spline curve
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    ax.plot(x, y, z, linewidth=2.0, c='k')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')

    # Axis limits
    l = 10
    ax.set_xlim([-l, l])
    ax.set_ylim([-l, l])
    ax.set_zlim([0, l])

    # Scatter end points
    end_points = np.array(knots)
    ax.scatter(end_points[:,0], end_points[:,1], end_points[:,2], c='r', marker='o')

    plt.show()

if __name__ == "__main__":
    main()