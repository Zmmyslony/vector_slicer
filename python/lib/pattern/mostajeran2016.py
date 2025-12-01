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
This file implements some of the director patterns designed in:

Encoding Gaussian curvature in glassy and elastomeric liquid crystal solids
Cyrus Mostajeran, Mark Warner, Taylor H. Ware and Timothy J. White
Proc. R. Soc. A.472: 20160112 – Published 1 May 2016

https://doi.org/10.1098/rspa.2016.0112
"""


import numpy as np
from lib.shape import basic as shapes
from lib.director import basic as directors
from lib.pattern.pattern import Pattern


def constant_gauss_curvature_director(w: float, is_positive=True, is_flipped=False):
    """
    Translationally symmetric director with constant Gauss curvature
    :param w: total width
    :param is_positive: sign of the Gauss curvature.
    :param is_flipped: mirror symmetry
    :return:
    """
    def alignment(v):
        y = v[:, :, 1]
        angle = np.where(np.abs(y) > w / 2, 0, np.arccos(2 * y / w))
        if not is_positive: angle += np.pi / 2
        if is_flipped: angle += np.pi
        return angle

    return directors.Director(alignment)


def constant_gauss_curvature_rectangle_pattern(l: float, w: float, is_positive=True, is_flipped=False):
    domain = shapes.rectangle(0, -w / 2, l, w / 2)
    if is_positive:
        type_name = 'positive'
    else:
        type_name = 'negative'
    name = f"constant_gauss_curvature_{type_name}_l_{l:.1f}_w_{w:.1f}"
    return Pattern(domain, constant_gauss_curvature_director(w, is_positive, is_flipped), name)


def constant_gauss_curvature_disk_pattern(r: float, is_positive=True, is_flipped=False):
    domain = shapes.disk(r)
    if is_positive:
        type_name = 'positive'
    else:
        type_name = 'negative'
    name = f"constant_gauss_curvature_{type_name}_r_{r:.1f}"
    return Pattern(domain, constant_gauss_curvature_director(2 * r, is_positive, is_flipped), name)


def constant_gauss_curvature_rectangle(l: float, w: float, line_width_mm, is_positive=True,
                                       is_flipped=False, is_displayed=False):
    """
    Translationally symmetric pattern with constant Gauss curvature on a rectangular domain.
    :param l:
    :param w:
    :param line_width_mm: printing width (line spacing) in mm.
    :param is_positive: sign of Gauss curvature
    :param is_flipped:
    :param is_displayed:
    :return:
    """
    pattern = constant_gauss_curvature_rectangle_pattern(l, w, is_positive, is_flipped)
    return pattern.generateInputFiles(line_width_mm, is_displayed=is_displayed)


def constant_gauss_curvature_disk(r: float, line_width_mm, is_positive=True,
                                  is_flipped=False, is_displayed=False):
    """
    Translationally symmetric pattern with constant Gauss curvature on a disk domain.
    :param r:
    :param line_width_mm: printing width (line spacing) in mm.
    :param is_positive: sign of the Gauss curvature.
    :param is_flipped:
    :param is_displayed:
    :return:
    """
    pattern = constant_gauss_curvature_disk_pattern(r, is_positive, is_flipped)
    return pattern.generateInputFiles(line_width_mm, is_displayed=is_displayed)
