"""
This file implements some of the director patterns designed in:

Interfacial metric mechanics: stitching patterns of shape change in active sheets
Fan Feng, Daniel Duffy, Mark Warner and John S. Biggins
Proc. R. Soc. A.478: 20220230 - Published 29 June 2022

DOI: https://doi.org/10.1098/rspa.2022.0230
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

import numpy as np
from lib.shape import basic, shape
from lib.director import basic as directors
from lib.pattern.pattern import Pattern


def spiral_pattern_vertical_boundary_pattern(r: float, alpha_left: float, alpha_right: float, distance_left: float,
                                             distance_right: float):
    """
    Inspired by Fig. 8a
    :param r:
    :param alpha_left:
    :param alpha_right:
    :param distance_left: positive values are further right
    :param distance_right: positive values are further right
    :return:
    """
    left_shape = basic.disk(r, distance_left, 0) - shape.half_plane(0, 0)
    right_shape = basic.disk(r, distance_right, 0) - shape.half_plane(0, np.pi)

    left_director = directors.spiral(alpha_left, distance_left)
    right_director = directors.spiral(alpha_right, distance_right)

    left_pattern = Pattern(left_shape, left_director)
    right_pattern = Pattern(right_shape, right_director)

    pattern = left_pattern + right_pattern
    pattern.name = f"feng2022_vert_r_{r:.1f}_a1_{alpha_left:.2f}_a2_{alpha_right:.2f}_d1_{distance_left:.2f}_d2_{distance_right:.2f}"
    return pattern


def spiral_pattern_horizontal_boundary(r: float, alpha_top: float, alpha_bottom: float, distance_top: float,
                                       distance_bottom: float):
    """
    Inspired by Fig. 8b
    :param r:
    :param alpha_top:
    :param alpha_bottom:
    :param distance_top: positive values are further right
    :param distance_bottom: positive values are further right
    :return:
    """
    top_shape = basic.disk(r, distance_top, 0) - shape.half_plane(0, -np.pi / 2)
    bottom_shape = basic.disk(r, distance_bottom, 0) - shape.half_plane(0, np.pi / 2)

    top_director = directors.spiral(alpha_top, distance_top)
    bottom_director = directors.spiral(alpha_bottom, distance_bottom)

    top_pattern = Pattern(top_shape, top_director)
    bottom_pattern = Pattern(bottom_shape, bottom_director)

    pattern = top_pattern + bottom_pattern
    pattern.name = f"feng2022_vert_r_{r:.1f}_a1_{alpha_top:.2f}_a2_{alpha_bottom:.2f}_d1_{distance_top:.2f}_d2_{distance_bottom:.2f}"
    return pattern