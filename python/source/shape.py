#  Copyright (c) 2023, Michał Zmyślony, mlz22@cam.ac.uk.
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

import numpy as np
import imageio.v3 as iio
import os


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

    def __add__(self, other):
        return self.union(other)

    def __sub__(self, other):
        return self.intersection(other)


def annulus(r_min, r_max, x_centre=0, y_centre=0):
    """
    Annulus with offset centre.
    :param r_min:
    :param r_max:
    :param x_centre:
    :param y_centre:
    :return:
    """
    def shape_function(v):
        v -= np.array([x_centre, y_centre])[np.newaxis, np.newaxis, :]
        d = np.linalg.norm(v, axis=2)
        return np.logical_and(r_min <= d, d <= r_max)

    bounds = [-r_max + x_centre, -r_max + y_centre, r_max + x_centre, r_max + y_centre]
    return Shape(shape_function, bounds)


def disk(r, x_centre=0, y_centre=0):
    return annulus(0, r, x_centre, y_centre)


def rectangle(x_min, y_min, x_max, y_max):
    def shape_function(v):
        x_condition = np.logical_and(x_min <= v[:, :, 0], v[:, :, 0] <= x_max)
        y_condition = np.logical_and(y_min <= v[:, :, 1], v[:, :, 1] <= y_max)

        return np.logical_and(x_condition, y_condition)

    bounds = [x_min, y_min, x_max, y_max]
    return Shape(shape_function, bounds)


def import_image(file_path, line_width_mm, line_width_pixel, centre_origin=None):
    """
    Imports image to be used as shape array.
    :param file_path:
    :param line_width_mm: WARNING: has to agree with what is later used for slicing.
    :param line_width_pixel: WARNING: has to agree with what is later used for slicing.
    :param centre_origin:
    :return:
    """
    if centre_origin is None:
        centre_origin = [0, 0]
    extension = os.path.splitext(file_path)[-1]
    if extension == ".png":
        shape_matrix = np.array(iio.imread(file_path, mode='L'), dtype=float)
        shape_matrix *= -1
        shape_matrix = np.flip(shape_matrix, axis=0)
        shape_matrix = np.transpose(shape_matrix)
    elif extension == ".csv":
        shape_matrix = np.genfromtxt(file_path, delimiter=',')
    else:
        raise Exception(f"Unimplemented image extension {extension}. Use .png or .csv.")

    # Normalize shape matrix
    shape_matrix -= shape_matrix.min()
    shape_matrix /= shape_matrix.max()

    pixel_width = line_width_mm / line_width_pixel
    bounds = [centre_origin[0] - (shape_matrix.shape[0] - 1) * pixel_width / 2,
              centre_origin[1] - (shape_matrix.shape[1] - 1) * pixel_width / 2,
              centre_origin[0] + (shape_matrix.shape[0] - 1) * pixel_width / 2,
              centre_origin[1] + (shape_matrix.shape[1] - 1) * pixel_width / 2]

    def shape_function(v):
        if v.shape[0:2] == shape_matrix.shape:
            return shape_matrix
        else:
            raise Exception(f"Incompatible mesh shape. Mesh: {v.shape[0:2]}. Image: {shape_matrix.shape}")

    return Shape(shape_function, bounds, is_defined_explicitly=True)
