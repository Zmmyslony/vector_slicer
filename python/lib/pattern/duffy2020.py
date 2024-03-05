"""
This file implements some of the director patterns designed in:

Defective nematogenesis: Gauss curvature in programmable shape-responsive sheets with topological defects
D. Duffy and J. S. Biggins
Soft Mat. 16.48 – Published 1 October 2020

https://doi.org/10.1039/D0SM01192D
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
from lib.director import basic as directors
from lib.pattern.pattern import Pattern


def topological_defect_director(charge: float, angle_offset=None):
    if angle_offset is None: angle_offset = 0

    def alignment(v):
        phi = np.arctan2(v[:, :, 1], v[:, :, 0])
        return charge * phi + angle_offset

    return directors.Director(alignment)


def topological_defect_pattern(r: float, charge: float, angle_offset=None):
    disk = shapes.disk(r)
    pattern = Pattern(disk, topological_defect_director(charge, angle_offset))
    pattern.name = f"defect_r_{r:.1f}_c_{charge:.1f}"
    return pattern


def topological_defect(r: float, charge: float, line_width_mm: float, angle_offset=None, is_displayed=False):
    pattern = topological_defect_pattern(r, charge, angle_offset)
    return pattern.generateInputFiles(line_width_mm, is_displayed=is_displayed)
