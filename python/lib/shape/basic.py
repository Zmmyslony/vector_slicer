"""
Basic shapes including rectangles, circles, annuli, polygons.
"""

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

from lib.shape.shape import Shape
import numpy as np
import copy
from matplotlib import path


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
        v_copy = copy.copy(v)
        v_copy -= np.array([x_centre, y_centre])[np.newaxis, np.newaxis, :]
        d = np.linalg.norm(v_copy, axis=2)
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


def polygon(coordinates):
    """
    If path is clockwise, the elements on the edge WILL NOT be included, and when it is counterclockwise, the edge
    elements WILL be included.

    See https://github.com/matplotlib/matplotlib/issues/9704 for more details.
    :param coordinates:
    :return:
    """
    polygon_path = path.Path(coordinates, closed=True)

    bounds = np.array([np.min(coordinates[:, 0]),
                       np.min(coordinates[:, 1]),
                       np.max(coordinates[:, 0]),
                       np.max(coordinates[:, 1])])

    def shape_function(v):
        v_flattened = np.vstack([v[:, :, 0].flatten(), v[:, :, 1].flatten()]).transpose()
        shape_flattened = polygon_path.contains_point(v_flattened, radius=1e-9)
        return np.array(np.reshape(shape_flattened, v.shape[0:2]), dtype=int)

    return Shape(shape_function, bounds)


def square(size: float, x_offset: float = 0, y_offset: float = 0):
    return rectangle(
        -size / 2 + x_offset,
        -size / 2 + y_offset,
        size / 2 + x_offset,
        size / 2 + y_offset
    )


def regular_polygon(sides: int, radius: float, orientation: float = 0, x_offset: float = 0, y_offset: float = 0):
    angle = orientation
    delta_angle = 2 * np.pi / sides
    positions = np.empty([sides, 2])
    offset = np.array([x_offset, y_offset])

    for i in range(sides):
        positions[i] = radius * np.array([np.cos(angle), np.sin(angle)]) + offset
        angle += delta_angle

    return polygon(positions)


def regular_triangle(side_length: float, orientation: float = 0, x_offset: float = 0, y_offset: float = 0):
    """
    CCW orientation (edges included)
    :param side_length:
    :param orientation:
    :param x_offset:
    :param y_offset:
    :return:
    """
    radius = side_length / np.sqrt(3)
    return regular_polygon(3, radius, orientation, x_offset, y_offset)


def regular_hexagon(side_length: float, orientation: float = 0, x_offset: float = 0, y_offset: float = 0):
    """
    CCW orientation (edges included)
    :param side_length:
    :param orientation:
    :param x_offset:
    :param y_offset:
    :return:
    """
    radius = side_length / np.sqrt(3)
    return regular_polygon(6, radius, orientation, x_offset, y_offset)
