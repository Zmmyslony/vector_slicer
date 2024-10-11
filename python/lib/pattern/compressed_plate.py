"""
This file implements pattern, which encodes the direction of principal stress in infinite plate with circular hole,
compressed along x-axis from the Kirsch equations.
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
from lib.shape import basic as shapes
from lib.director.director import Director
from lib.pattern.pattern import Pattern


# For hole of radius 1 centred at (0, 0)
def theta_base(x, y):
    x_component = ((-2 * y * (x ** 2 + y ** 2) ** 4 - (x * (
                -2 * (x ** 2 + y ** 2) ** 5 + 2 * (-2 + x ** 2 + y ** 2) * (x ** 2 + y ** 2) ** 5 * np.cos(
            2 * np.arctan2(y, x)) + np.sqrt(2) * np.sqrt((x ** 2 + y ** 2) ** 8 * (
                    9 - 12 * (x ** 2 + y ** 2) + 4 * (x ** 2 + y ** 2) ** 2 + 2 * (x ** 2 + y ** 2) ** 4 - 4 * (
                        -2 + x ** 2 + y ** 2) * (x ** 4 - y ** 4) - (3 - 4 * (x ** 2 + y ** 2)) * (
                                3 - 2 * (x ** 2 + y ** 2) ** 2) * np.cos(4 * np.arctan2(y, x))))) * 1 / np.sin(
        2 * np.arctan2(y, x))) / ((-1 + x ** 2 + y ** 2) * (3 + x ** 2 + y ** 2))) / (2. * (x ** 2 + y ** 2) ** 4.5))
    y_component = (x / np.sqrt(x ** 2 + y ** 2) - (y * (
                -2 * (x ** 2 + y ** 2) ** 5 + 2 * (-2 + x ** 2 + y ** 2) * (x ** 2 + y ** 2) ** 5 * np.cos(
            2 * np.arctan2(y, x)) + np.sqrt(2) * np.sqrt((x ** 2 + y ** 2) ** 8 * (
                    9 - 12 * (x ** 2 + y ** 2) + 4 * (x ** 2 + y ** 2) ** 2 + 2 * (x ** 2 + y ** 2) ** 4 - 4 * (
                        -2 + x ** 2 + y ** 2) * (x ** 4 - y ** 4) - (3 - 4 * (x ** 2 + y ** 2)) * (
                                3 - 2 * (x ** 2 + y ** 2) ** 2) * np.cos(4 * np.arctan2(y, x))))) * 1 / np.sin(
        2 * np.arctan2(y, x))) / (2. * (-1 + x ** 2 + y ** 2) * (x ** 2 + y ** 2) ** 4.5 * (3 + x ** 2 + y ** 2)))
    return np.arctan2(y_component, x_component)


def compressed_plate_pattern(x_length, y_width, r_hole=1):
    shape = shapes.rectangle(-x_length / 2, -y_width / 2, x_length / 2, y_width / 2) - shapes.disk(r_hole)

    def director_function(v):
        x = v[..., 0] / r_hole
        y = v[..., 1] / r_hole
        return theta_base(x, y)

    director = Director(director_function)
    pattern_name = "compressed_plate"
    return Pattern(shape, director, name=pattern_name)
