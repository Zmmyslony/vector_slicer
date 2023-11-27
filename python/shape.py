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


class Shape:
    def __init__(self, shape_function, bounds):
        self.shape_function = shape_function
        self.x_min = bounds[0]
        self.y_min = bounds[1]
        self.x_max = bounds[2]
        self.y_max = bounds[3]

    def union(self, shape):
        def new_shape_function(v):
            return np.logical_or(self.shape_function(v), shape.shape_function(v))

        x_min = min(self.x_min, shape.x_min)
        y_min = min(self.y_min, shape.y_min)
        x_max = max(self.x_max, shape.x_max)
        y_max = max(self.y_max, shape.y_max)
        bounds = [x_min, y_min, x_max, y_max]
        return Shape(new_shape_function, bounds)

    def intersection(self, shape):
        def new_shape_function(v):
            return np.logical_and(self.shape_function(v), np.logical_not(shape.shape_function(v)))

        return Shape(new_shape_function, [self.x_min, self.y_min, self.x_max, self.y_max])

    def __add__(self, other):
        return self.union(other)

    def __sub__(self, other):
        return self.intersection(other)


def annulus(r_min, r_max, x_offset=0, y_offset=0):
    def shape_function(v):
        v -= np.array([x_offset, y_offset])[np.newaxis, np.newaxis, :]
        d = np.linalg.norm(v, axis=2)
        return np.logical_and(r_min <= d, d <= r_max)

    bounds = [-r_max + x_offset, -r_max + y_offset, r_max + x_offset, r_max + y_offset]
    return Shape(shape_function, bounds)


def disk(r, x_offset=0, y_offset=0):
    return annulus(0, r, x_offset, y_offset)


def rectangle(x_min, y_min, x_max, y_max):
    def shape_function(v):
        x_condition = np.logical_and(x_min <= v[:, :, 0], v[:, :, 0] <= x_max)
        y_condition = np.logical_and(y_min <= v[:, :, 1], v[:, :, 1] <= y_max)

        return np.logical_and(x_condition, y_condition)

    bounds = [x_min, y_min, x_max, y_max]
    return Shape(shape_function, bounds)
