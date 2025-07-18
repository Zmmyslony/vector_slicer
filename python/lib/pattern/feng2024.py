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

Geometry, mechanics and actuation of intrinsically curved folds
F. Feng, K. Dradrach, M. Zmyślony, M. Barnes and J. S. Biggins,
Soft Matter, 2024, DOI: 10.1039/D3SM01584J


"""


import numpy as np
from lib.shape import basic as shapes
from lib.director import basic as directors
from lib.pattern.pattern import Pattern, SymmetricPattern


def translationally_invariant_gauss_flat_alignment(y_begin, y_end, is_negative=True, is_flipped=False):
    def alignment(v):
        w = y_end - y_begin
        y = v[:, :, 1] - y_begin
        # 0 -> 1/2 ArcCos(1 - 2 y/w) -> Pi / 2
        if is_flipped:
            director_grid = np.where(y < 0, 0,
                                     np.where(y > w, np.pi / 2,
                                              -1 / 2 * np.arccos(1 - 2 * y / w)))
        else:
            director_grid = np.where(y < 0, 0,
                                     np.where(y > w, np.pi / 2,
                                              1 / 2 * np.arccos(1 - 2 * y / w)))
        if is_negative:
            return director_grid
        else:
            return director_grid + np.pi / 2

    return directors.Director(alignment)


def gripper(length, w_inner, w_outer, line_width_mm, arm_number: int, line_width_pixel=9, is_positive: bool = True,
            is_displayed=False, is_flipped=True):
    y_1 = -w_inner / 2 - w_outer
    y_2 = -w_inner / 2
    y_3 = w_inner / 2
    y_4 = w_inner / 2 + w_outer

    rectangle_top = shapes.rectangle(0, y_3, length, y_4)
    rectangle_mid = shapes.rectangle(0, y_2, length, y_3)
    rectangle_bot = shapes.rectangle(0, y_1, length, y_2)

    longitudinal_alignment = directors.uniaxial_alignment(0)
    transverse_alignment = directors.uniaxial_alignment(np.pi / 2)

    if is_positive:
        type_p = "positive"
        pattern_top = Pattern(rectangle_top,
                              translationally_invariant_gauss_flat_alignment(y_3, y_4, False, is_flipped=is_flipped))
        pattern_mid = Pattern(rectangle_mid, transverse_alignment)
        pattern_bot = Pattern(rectangle_bot,
                              translationally_invariant_gauss_flat_alignment(y_1, y_2, True, is_flipped=is_flipped))
    else:
        type_p = "negative"
        pattern_top = Pattern(rectangle_top,
                              translationally_invariant_gauss_flat_alignment(y_3, y_4, True, is_flipped=is_flipped))
        pattern_mid = Pattern(rectangle_mid, longitudinal_alignment)
        pattern_bot = Pattern(rectangle_bot,
                              translationally_invariant_gauss_flat_alignment(y_1, y_2, False, is_flipped=is_flipped))

    pattern = pattern_top + pattern_mid + pattern_bot

    gripper_pattern = pattern.symmetrise(arm_number)
    name = f"gripper_{type_p}_w_out_{w_outer:d}_w_in_{w_inner:d}_l_{length:d}_arms_{arm_number}"

    return gripper_pattern.generateInputFiles(line_width_mm, name, line_width_pixel, is_displayed=is_displayed)


def generate_lines_of_concentrated_gauss_curvature_patterns(w_bottom, w_top, length, line_width_mm, line_width_pixel,
                                                            y_begin=None, x_begin=None, is_displayed=False):
    """
    Generates input files for director patterns from Feng2024.
    :param w_bottom: width of the bottom part (curving one)
    :param w_top: width of the top part (uniaxial one)
    :param line_width_pixel:
    :param line_width_mm:
    :param is_displayed:
    :param length:
    :param y_begin:
    :param x_begin:
    :return: A-, A+, S-, S+ - list of names corresponding to generated patterns
    """
    if y_begin is None:
        y_begin = -(w_bottom + w_top) / 2
    if x_begin is None:
        x_begin = -length / 2
    rectangle_bottom = shapes.rectangle(x_begin, y_begin, x_begin + length, y_begin + w_bottom)
    rectangle_top = shapes.rectangle(x_begin, y_begin + w_top, x_begin + length, y_begin + w_top + w_bottom)

    longitudinal_alignment = directors.uniaxial_alignment(0)
    transverse_alignment = directors.uniaxial_alignment(np.pi / 2)

    alignment_negative_top = translationally_invariant_gauss_flat_alignment(y_begin + w_bottom,
                                                                            y_begin + w_top + w_bottom, True)
    alignment_positive_top = translationally_invariant_gauss_flat_alignment(y_begin + w_bottom,
                                                                            y_begin + w_top + w_bottom, False)
    alignment_negative_bottom = translationally_invariant_gauss_flat_alignment(y_begin,
                                                                               y_begin + w_bottom, False)
    alignment_positive_bottom = translationally_invariant_gauss_flat_alignment(y_begin,
                                                                               y_begin + w_bottom, True)

    asymmetric_negative = (Pattern(rectangle_bottom, longitudinal_alignment) +
                           Pattern(rectangle_top, alignment_negative_top))
    asymmetric_negative.name = f"asymmetric_negative_L{length:d}_Wb{w_bottom:d}_Wt{w_top:d}"

    asymmetric_positive = (Pattern(rectangle_bottom, transverse_alignment) +
                           Pattern(rectangle_top, alignment_positive_top))
    asymmetric_positive.name = f"asymmetric_positive_L{length:d}_Wb{w_bottom:d}_Wt{w_top:d}"

    symmetric_negative = (Pattern(rectangle_bottom, alignment_negative_bottom) +
                          Pattern(rectangle_top, alignment_negative_top))
    symmetric_negative.name = f"symmetric_negative_L{length:d}_Wb{w_bottom:d}_Wt{w_top:d}"

    symmetric_positive = (Pattern(rectangle_bottom, alignment_positive_bottom) +
                          Pattern(rectangle_top, alignment_positive_top))
    symmetric_positive.name = f"symmetric_positive_L{length:d}_Wb{w_bottom:d}_Wt{w_top:d}"

    asymmetric_negative.generateInputFiles(line_width_mm, line_width_pixel, filling_method="Perimeter",
                                           is_displayed=is_displayed)

    asymmetric_positive.generateInputFiles(line_width_mm, line_width_pixel, filling_method="Perimeter",
                                           is_displayed=is_displayed)

    symmetric_negative.generateInputFiles(line_width_mm, line_width_pixel, filling_method="Perimeter",
                                          is_displayed=is_displayed)

    symmetric_positive.generateInputFiles(line_width_mm, line_width_pixel, filling_method="Perimeter",
                                          is_displayed=is_displayed)

    SymmetricPattern(rectangle_top, alignment_positive_top, 3).generateInputFiles(line_width_mm,
                                                                                  f"symmetric_positive_3_arms_L{length:d}_Wb{w_bottom:d}_Wt{w_top:d}",
                                                                                  line_width_pixel,
                                                                                  filling_method="Perimeter",
                                                                                  is_displayed=True)

    return [f"asymmetric_negative_L{length:d}_Wb{w_bottom:d}_Wt{w_top:d}",
            f"asymmetric_positive_L{length:d}_Wb{w_bottom:d}_Wt{w_top:d}",
            f"symmetric_negative_L{length:d}_Wb{w_bottom:d}_Wt{w_top:d}",
            f"symmetric_positive_L{length:d}_Wb{w_bottom:d}_Wt{w_top:d}",
            f"symmetric_positive_3_arms_L{length:d}_Wb{w_bottom:d}_Wt{w_top:d}"]
