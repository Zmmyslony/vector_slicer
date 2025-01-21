"""
Definition of director class.
"""

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
import copy


def q_tensor(director_function, mesh, offset):
    high_precision_mesh = np.array(mesh + offset, dtype=np.longdouble)
    director = director_function(high_precision_mesh)
    cos_cos = np.cos(director) * np.cos(director)
    cos_sin = np.cos(director) * np.sin(director)
    sin_sin = np.sin(director) * np.sin(director)

    first_stack = np.stack([cos_cos, cos_sin], axis=-1)
    second_stack = np.stack([cos_sin, sin_sin], axis=-1)
    return np.stack([first_stack, second_stack], axis=first_stack.ndim)


def div_q_tensor(director_function, mesh, delta):
    return (np.dot(q_tensor(director_function, mesh, [delta / 2, 0]), np.array([1, 0])) +
            np.dot(q_tensor(director_function, mesh, [-delta / 2, 0]), np.array([-1, 0])) +
            np.dot(q_tensor(director_function, mesh, [0, delta / 2]), np.array([0, 1])) +
            np.dot(q_tensor(director_function, mesh, [0, -delta / 2]), np.array([0, -1]))
            ) / delta


def splay_numeric(director_function, derivative_delta):
    """
    Calculates numerically the splay function based on the director function as s = Q (Div Q), where Q = n tensor n.
    :param director_function: numpy compatible function returning angle theta when provided with xy mesh [x_dim, y_dim, 2]
    :param derivative_delta: Numerical delta used for calculation.
    :return: Grid of splay vectors [x_dim, y_dim, 2].
    """
    if derivative_delta < 1e-8:
        print("Derivative delta lower than 1e-13 will yield inaccurate results due to 52 bit long mantissa of double. ")
        derivative_delta = 1e-8

    def splay(mesh):
        q_tensor_grid = q_tensor(director_function, mesh, [0, 0])
        div_q_tensor_grid = div_q_tensor(director_function, mesh, derivative_delta)
        q_div_q = np.matmul(q_tensor_grid, div_q_tensor_grid[..., None])[..., 0]
        q_div_q = np.nan_to_num(q_div_q)
        return q_div_q

    return splay


class Director:
    def __init__(self, director, splay_function=None, derivative_delta=1e-6):
        """
        Director class contains the definitions of director and splay functions.
        :param director: function returning angle theta [x_dim, y_dim] when provided with xy mesh [x_dim, y_dim, 2].
        :param splay_function: analytical formula for splay - usually unnecessary unless high performance operation is required.
        :param derivative_delta: distance used for numerical calculation of splay.
        """
        self.director = director
        if splay_function is None:
            self.splay = splay_numeric(director, derivative_delta)
        else:
            self.splay = splay_function

    def rotate(self, angle):
        rotation_matrix = np.array([[np.cos(angle), -np.sin(angle)],
                                    [np.sin(angle), np.cos(angle)]])

        def rotated_director(v):
            rotation = np.transpose(rotation_matrix)[None, None, :, :]
            v_copy = copy.copy(v)[:, :, :, None]
            v_rotated = np.matmul(rotation, v_copy)[:, :, :, 0]
            return self.director(v_rotated) + angle

        return Director(rotated_director)

