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
Basic director patterns such as uniaxial, radial, azimuthal, spiral.
"""

from lib.director.director import Director
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

        return np.mod(director.director(r) + radial_angle, 2 * np.pi)

    return Director(symmetric_director)


def topological_defect(charge, zero_phase, x_centre=0, y_centre=0):
    """
    Creates a pattern corresponding to a topological defect with the prescribed charge and zero phase.
    :param charge:
    :param zero_phase:
    :param x_centre:
    :param y_centre:
    :return:
    """
    def director(v):
        v_offset = v - np.array([x_centre, y_centre])
        radial_angle = np.arctan2(v_offset[:, :, 1], v_offset[:, :, 0])

        return np.mod(radial_angle * charge + zero_phase, 2 * np.pi)
    return Director(director)


def spiral(angle, x_centre=0, y_centre=0) -> Director:
    """
    Returns spiral director of constant angle in relation to radial direction
    :param angle:
    :param x_centre:
    :param y_centre:
    :return:
    """
    return topological_defect(1, angle, x_centre, y_centre)


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


def polygon_constant_angle(angle: float, vertex_count: int, first_vertex_angle: float = None):
    """
    Piecewise-constant pattern resembling a regular polygon,
    :param angle: angle of the director in the first segment right of the centre.
    :param vertex_count:
    :param first_vertex_angle: angle, corresponding to the first edge of the first vertex.
    :return:
    """
    delta_angle = 2 * np.pi / vertex_count
    if first_vertex_angle is None: first_vertex_angle = -delta_angle / 2

    def director(v):
        angular_position = np.arctan2(v[:, :, 1], v[:, :, 0])
        segment = np.floor_divide(angular_position - first_vertex_angle, delta_angle)
        return segment * delta_angle + angle

    return Director(director)


def azimuthal_piecewise_polygon(vertex_count: int, angle_offset=0, first_vertex_angle=0):
    """
    Director pattern corresponding to piecewise +1 topological defect (cone).
    :param vertex_count:
    :return:
    """
    return polygon_constant_angle(np.pi / 2 + angle_offset,
                                  vertex_count,
                                  first_vertex_angle=first_vertex_angle)
