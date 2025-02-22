"""
This file implements some of the director patterns designed in:

Metric mechanics with nontrivial topology: Actuating irises, cylinders, and evertors
D. Duffy, M. Javed, M. K. Abdelrahman, T. H. Ware, M. Warner, and J. S. Biggins
Phys. Rev. E 104, 065004 – Published 20 December 2021

https://journals.aps.org/pre/abstract/10.1103/PhysRevE.104.065004
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
from lib.shape import basic as shapes
from lib.director import basic as directors
from lib.pattern.pattern import Pattern


def iris_alignment(r_in, target_elongation):
    r_out = r_in * np.sqrt(1 + 1 / target_elongation + 1 / np.sqrt(target_elongation))

    def alignment(v):
        r = v[:, :, 0]
        cos_term = (2 * r_in ** 2 + r ** 2 * (target_elongation ** 1.5 - 1)) / (r ** 2 * (target_elongation ** 1.5 + 1))
        cos_term = np.where(np.abs(cos_term) <= 1, cos_term, 1)
        return 1 / 2 * np.arccos(cos_term)

    symmetric_alignment = directors.radial_symmetry(directors.Director(alignment))
    return symmetric_alignment, r_out


def iris_pattern(r_in: float, target_elongation: float):
    """
    :param r_in: inner radius in mm
    :param target_elongation: elongation at which the iris will be flat (material elongation at desired temperature)
    :return:
    """
    iris_director, r_out = iris_alignment(r_in, target_elongation)
    iris_annulus = shapes.annulus(r_in, r_out)
    pattern = Pattern(iris_annulus, iris_director)
    pattern.name = f"iris_{r_in:d}_mm_{int(target_elongation * 100):d}%"
    return pattern


def cylinder_alignment(r_in, target_elongation):
    r_out = r_in * target_elongation ** -1.5

    def alignment(v):
        r = v[:, :, 0]
        cos_term = (- 2 * r_in ** 2 + r ** 2 * (target_elongation ** 3 + 1)) / (r ** 2 * (target_elongation ** 3 - 1))
        cos_term = np.where(np.abs(cos_term) <= 1, cos_term, 1)
        return 1 / 2 * np.arccos(cos_term)

    symmetric_alignment = directors.radial_symmetry(directors.Director(alignment))
    return symmetric_alignment, r_out


def cylinder_pattern(r_in, target_elongation):
    """
    :param r_in: inner radius in mm
    :param target_elongation: elongation at which the cylinder will stand upright (material elongation at desired temperature)
    :return:
    """
    cylinder_director, r_out = cylinder_alignment(r_in, target_elongation)
    cylinder_annulus = shapes.annulus(r_in, r_out)
    pattern = Pattern(cylinder_annulus, cylinder_director)
    pattern.name = f"cylinder_{r_in:d}_mm_{int(target_elongation * 100):d}%"
    return pattern


def evertor_alignment(r_in, target_elongation):
    r_out = r_in * target_elongation ** -0.75

    def alignment(v):
        r = v[:, :, 0]
        cos_term = 1 + 2 * (r ** 2 - r_in ** 2) / (r ** 2 * (target_elongation ** 1.5 - 1))
        cos_term = np.where(np.abs(cos_term) <= 1, cos_term, 1)
        return 1 / 2 * np.arccos(cos_term)

    symmetric_alignment = directors.radial_symmetry(directors.Director(alignment))
    return symmetric_alignment, r_out


def evertor_pattern(r_in, target_elongation):
    """
     :param r_in: inner radius in mm
     :param target_elongation: elongation at which the evertor will evert itself (material elongation at desired temperature)
     :return:
     """
    evertor_director, r_out = evertor_alignment(r_in, target_elongation)
    evertor_annulus = shapes.annulus(r_in, r_out)
    pattern = Pattern(evertor_annulus, evertor_director)
    pattern.name = f"evertor_{r_in:d}_mm_{int(target_elongation * 100):d}%"
    return pattern


def generate_radial_gauss_flat_patterns(r_in: float, target_elongation: float, line_width_mm: float,
                                        line_width_pixel: int = 9, is_displayed=False):
    """
    Generates iris, cylinder and evertor director patterns.
    :param r_in:
    :param target_elongation:
    :param line_width_mm:
    :param line_width_pixel:
    :param is_displayed:
    :return: list of pattern names [iris, cylinder, evertor]
    """
    iris = iris_pattern(r_in, target_elongation)
    cylinder = cylinder_pattern(r_in, target_elongation)
    evertor = evertor_pattern(r_in, target_elongation)

    pattern_names = [
        iris.generateInputFiles(line_width_mm, filling_method="Perimeter", is_displayed=is_displayed),
        cylinder.generateInputFiles(line_width_mm, filling_method="Perimeter", is_displayed=is_displayed),
        evertor.generateInputFiles(line_width_mm, filling_method="Perimeter", is_displayed=is_displayed)]

    return pattern_names
