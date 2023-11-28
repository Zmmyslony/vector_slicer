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


def q_tensor(director_function, mesh, offset):
    director = director_function(mesh + offset)
    q_tensor_value = np.transpose(np.array(
        [[np.cos(director) * np.cos(director), np.sin(director) * np.cos(director)],
         [np.sin(director) * np.cos(director), np.sin(director) * np.sin(director)]]), [2, 3, 0, 1])
    return q_tensor_value


def div_q_tensor(director_function, mesh, delta):
    return (np.dot(q_tensor(director_function, mesh, [delta / 2, 0]), np.array([1, 0])) +
            np.dot(q_tensor(director_function, mesh, [-delta / 2, 0]), np.array([-1, 0])) +
            np.dot(q_tensor(director_function, mesh, [0, delta / 2]), np.array([0, 1])) +
            np.dot(q_tensor(director_function, mesh, [0, -delta / 2]), np.array([0, -1]))
            ) / delta


def splay_numeric(director_function, derivative_delta):
    if derivative_delta < 1e-13:
        print("Derivative delta lower than 1e-13 will yield inaccurate results due to 52 bit long mantissa of float. ")
        derivative_delta = 1e-13

    def splay(mesh):
        q_tensor_grid = q_tensor(director_function, mesh, [0, 0])
        div_q_tensor_grid = div_q_tensor(director_function, mesh, derivative_delta)
        q_div_q = np.matmul(q_tensor_grid, div_q_tensor_grid[:, :, :, None])[:, :, :, 0]

        return q_div_q

    return splay


class Director:
    def __init__(self, director, splay_function=None, derivative_delta=1e-11):
        self.director = director
        if splay_function is None:
            self.splay = splay_numeric(director, derivative_delta)
        else:
            self.splay = splay_function


def uniaxial_alignment(theta):
    def director(v):
        return theta * np.ones(v.shape[0:2])

    return Director(director)


def radial_symmetry(director: Director, x_offset=0, y_offset=0):
    def symmetric_director(v):
        v -= np.array([x_offset, y_offset])[np.newaxis, np.newaxis, :]
        d = np.linalg.norm(v, axis=2)
        radial_angle = np.arctan2(v[:, :, 1], v[:, :, 0])
        return np.mod(director.director(d) + radial_angle, np.pi)

    return Director(symmetric_director)


def single_charge_field(position, charge):
    def director_vector(v):
        v_offset = v - position
        d = np.linalg.norm(v_offset, axis=2)
        return charge * v_offset / np.power(d, 2)[:, :, None]

    return director_vector


def charge_field(position_list, charge_list):
    def director(v):
        director_vector_sum = np.zeros_like(v)
        for i in range(len(position_list)):
            current_director = single_charge_field(position_list[i], charge_list[i])
            director_vector_sum += current_director(v)
        return np.arctan2(director_vector_sum[:, :, 1], director_vector_sum[:, :, 0])

    return Director(director)
