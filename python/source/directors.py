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

from .director import Director
import numpy as np


def uniaxial_alignment(theta: float) -> Director:
    """
    :param theta: angle between director and x-axis
    :return: uniaxial director
    """

    def director(v):
        return theta * np.ones(v.shape[0:2])

    return Director(director)


def radial_symmetry(director: Director, x_centre=0, y_centre=0) -> Director:
    """
    Radially symmetrises the director field, where the radial function corresponds to the (x, 0) coordinates.
    :param director: original xy defined director field.
    :param x_centre: coordinates of symmetry centre.
    :param y_centre:
    :return:
    """

    def symmetric_director(v):
        v_offset = v - np.array([x_centre, y_centre])
        radial_angle = np.arctan2(v_offset[:, :, 1], v_offset[:, :, 0])

        r = np.linalg.norm(v_offset, axis=2)
        # Append y-coordinates as 0 everywhere.
        r = r[:, :, None]
        r = np.append(r, np.zeros_like(r), axis=2)
        return np.mod(director.director(r) + radial_angle, np.pi)

    return Director(symmetric_director)


def spiral(angle, x_centre=0, y_centre=0) -> Director:
    """
    Returns spiral director of constant angle in relation to radial direction
    :param angle:
    :param x_centre:
    :param y_centre:
    :return:
    """
    return radial_symmetry(uniaxial_alignment(angle), x_centre, y_centre)


def azimuthal(x_centre=0, y_centre=0) -> Director:
    return spiral(np.pi / 2, x_centre, y_centre)


def radial(x_centre=0, y_centre=0) -> Director:
    return spiral(0, x_centre, y_centre)


def intermediate_charge_field(position, charge):
    """
    Vector field from an electric charge.
    :param position:
    :param charge:
    :return: function: mesh [x_dim, y_dim, 2] -> field [x_dim, y_dim, 2]
    """

    def director_vector(v):
        v_offset = v - position
        d = np.linalg.norm(v_offset, axis=2)
        return charge * v_offset / np.power(d, 2)[:, :, None]

    return director_vector


def charge_field(position_list, charge_list) -> Director:
    """
    Director field resulting from superposition of a number of charges
    :param position_list: float [n_charge, 2]
    :param charge_list: float [n_charge]
    :return:
    """

    def director(v):
        director_vector_sum = np.zeros_like(v)
        for i in range(len(position_list)):
            current_director = intermediate_charge_field(position_list[i], charge_list[i])
            director_vector_sum += current_director(v)
        return np.arctan2(director_vector_sum[:, :, 1], director_vector_sum[:, :, 0])

    return Director(director)


def single_charge_field(position, charge) -> Director:
    """
    Director field resulting from a single electric charge
    :param position: float [2]
    :param charge: float
    :return:
    """
    return charge_field([position], [charge])
