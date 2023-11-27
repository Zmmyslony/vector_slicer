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
import shape


def x_derivative(mesh, director_function, delta):
    return (director_function(mesh + [delta / 2, 0]) - director_function(mesh - [delta / 2, 0])) / delta


def y_derivative(mesh, director_function, delta):
    return (director_function(mesh + [0, delta / 2]) - director_function(mesh - [0, delta / 2])) / delta


def splay_numeric(director_function, derivative_delta):
    def splay(mesh):
        x_der = x_derivative(mesh, director_function, derivative_delta)
        y_der = y_derivative(mesh, director_function, derivative_delta)
        director = director_function(mesh)
        div = np.cos(director) * y_der - np.sin(director) * x_der
        x_splay = np.cos(director) * div
        y_splay = np.sin(director) * div
        splay_grid = np.transpose([x_splay, y_splay], [1, 2, 0])
        return splay_grid

    return splay


class Director:
    def __init__(self, director, splay_function=None, derivative_delta=1e-9):
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
