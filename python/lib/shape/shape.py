#  Copyright (c) 2023-2025, Michał Zmyślony, mlz22@cam.ac.uk.
#
#  Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
#
#  This file is part of Vector Slicer.
#
#  Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
#  License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
#  later version.
#
#  Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
#  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#  Public License for more details.
#
#  You should have received a copy of the GNU General Public License along with Vector Slicer.
#  If not, see <https://www.gnu.org/licenses/>.

"""
Definition of the shape class.
"""


import numpy as np
import copy


class Shape:
    def __init__(self, shape_function, bounds, is_defined_explicitly=False):
        """
        Shape class for creation of domains that are to be printed.
        :param shape_function: function defining which coordinates are to be filled: mesh [x_dim, y_dim, 2] -> bool [x_dim, y_dim]
        :param bounds: [x_min, y_min, x_max, y_max]
        :param is_defined_explicitly: indicator whether the object is defined as binary array or numpy logic conditions.
        """
        self.shape_function = shape_function
        self.x_min = bounds[0]
        self.y_min = bounds[1]
        self.x_max = bounds[2]
        self.y_max = bounds[3]
        self.is_defined_explicitly = is_defined_explicitly

    def union(self, other):
        if self.is_defined_explicitly or other.is_defined_explicitly:
            raise Exception("Cannot add explicitly defined shapes.")

        def new_shape_function(v):
            return np.logical_or(self.shape_function(v), other.shape_function(v))

        x_min = min(self.x_min, other.x_min)
        y_min = min(self.y_min, other.y_min)
        x_max = max(self.x_max, other.x_max)
        y_max = max(self.y_max, other.y_max)
        bounds = [x_min, y_min, x_max, y_max]
        return Shape(new_shape_function, bounds)

    def intersection(self, other):
        if self.is_defined_explicitly or other.is_defined_explicitly:
            raise Exception("Cannot subtract explicitly defined shapes.")

        def new_shape_function(v):
            return np.logical_and(self.shape_function(v), np.logical_not(other.shape_function(v)))

        return Shape(new_shape_function, [self.x_min, self.y_min, self.x_max, self.y_max])

    def rotate(self, angle: float):
        """ Angle in radians """
        rotation_matrix = np.array([[np.cos(angle), -np.sin(angle)],
                                    [np.sin(angle), np.cos(angle)]])

        def new_shape_function(v):
            rotation = np.transpose(rotation_matrix)[None, None, :, :]
            v_copy = copy.copy(v)[:, :, :, None]
            v_rotated = np.matmul(rotation, v_copy)[:, :, :, 0]
            return self.shape_function(v_rotated)

        corners = np.array([[self.x_min, self.y_min],
                            [self.x_min, self.y_max],
                            [self.x_max, self.y_min],
                            [self.x_max, self.y_max]])
        rotated_corners = np.matmul(rotation_matrix[None, :, :], corners[:, :, None])

        bounds = [np.min(rotated_corners[:, 0]),
                  np.min(rotated_corners[:, 1]),
                  np.max(rotated_corners[:, 0]),
                  np.max(rotated_corners[:, 1])]
        return Shape(new_shape_function, bounds)

    def symmetrise(self, arm_number: int, begin_angle: float = None):
        """
        Creates a symmetric shape with a selected number of arms, where each arm is copy of the initial one.
        E.g. 4-armed symmetrisation of a rectangle is a cross.
        :param arm_number:
        :param begin_angle: defines initial arm which will range between begin_angle and begin_angle + 2 pi / arm_number
        :return:
        """
        if arm_number <= 1:
            return self
        sector_size = 2 * np.pi / arm_number
        if begin_angle is None:
            begin_angle = -1 / 2 * sector_size

        base_segment = self.angular_slice(begin_angle, begin_angle + sector_size)
        symmetrised_shape = base_segment
        for i in range(1, arm_number):
            symmetrised_shape = symmetrised_shape + base_segment.rotate(sector_size * i)
        return symmetrised_shape

    def angular_slice(self, begin_angle: float, end_angle: float):
        return self - pacman_shape(begin_angle, end_angle)

    def __add__(self, other):
        return self.union(other)

    def __sub__(self, other):
        return self.intersection(other)

    def bounds(self):
        return np.array([self.x_min, self.y_min, self.x_max, self.y_max])

    def invert(self):
        """
        Inverts (negates) the current shape while keeping the bounds unchanged.
        :return:
        """

        def new_shape_function(v):
            shape_array = self.shape_function(v)
            return np.logical_not(shape_array)

        return Shape(new_shape_function, self.bounds())

    def scale(self, scale):
        """
        Scales the shape together with its boundaries
        :param scale:
        :return:
        """

        def new_shape_function(v):
            v_scaled = v / scale
            return self.shape_function(v_scaled)

        return Shape(new_shape_function, scale * self.bounds())

    def print_bounds(self):
        print(f"x: {self.x_min:.2f}-{self.x_max:.2f}\n"
              f"y: {self.y_min:.2f}-{self.y_max:.2f}")


def pacman_shape(opening_angle: float, closing_angle: float, centre=np.array([0, 0])):
    """
    This shape is meant for localising the shapes into a slice-type shape through subtraction. It is not meant for use
    by itself
    :param opening_angle:
    :param closing_angle:
    :param centre:
    :return:
    """

    def shape_function(v):
        v_offset = copy.copy(v) - centre
        phi = np.arctan2(v_offset[:, :, 1], v_offset[:, :, 0]) % (2 * np.pi)
        return np.logical_and(phi < opening_angle % (2 * np.pi), phi > closing_angle % (2 * np.pi))

    return Shape(shape_function, [0, 0, 0, 0])


def half_plane(r, angle, is_edge_inclusive=False):
    """
    Half plane used for trimming the patterns.
    :param r: distance from origin where the plane should begin.
    :param angle: rotation of the plane - 0 means half-plane extending to x-infinity, Pi / 2 to y-infinity etc.
    :param is_edge_inclusive: whether distance r is included or not.
    :return:
    """

    def shape_function(v):
        r_coord = np.cos(angle) * v[:, :, 0] + np.sin(angle) * v[:, :, 1]
        if is_edge_inclusive:
            return r_coord <= r
        else:
            return r_coord < r

    return Shape(shape_function, [0, 0, 0, 0])