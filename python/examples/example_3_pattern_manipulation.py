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
This file contains information about how Patterns can be manipulated to create complex patterns.

These operations include: addition, rotation, and symmetrisation.

These Patterns are shown using streamplots, without the slicing, to speed up file generation.

"""

from lib.director import basic as directors
from lib.shape import basic as shapes
from lib.pattern.pattern import Pattern
import numpy as np

if __name__ == "__main__":
    # 1: Patterns can be combined through addition
    rectangle = shapes.rectangle(0, -4, 10, 4)
    uniaxial_director = directors.uniaxial_alignment(0)
    uniaxial_rectangle = Pattern(rectangle, uniaxial_director)

    disk = shapes.disk(4, 10, 0)
    azimuthal_director = directors.azimuthal(10, 0)
    azimuthal_disk = Pattern(disk, azimuthal_director)

    # Pattern specified with a name None, will not generate input files, but can be used for visualisation.
    # Generally, this is recommended when designing the pattern as slicing is the computationally heavy aspect
    composed_pattern = uniaxial_rectangle + azimuthal_disk
    composed_pattern.generateInputFiles(0.2, is_displayed=True)

    # WARNING: The order of addition matters, as the director of the second pattern, overwrites the director
    # in the overlapping region.
    other_composed_pattern = azimuthal_disk + uniaxial_rectangle
    other_composed_pattern.generateInputFiles(0.2, is_displayed=True)

    # 2: Similar to shapes, patterns can be rotated.
    rotated_pattern = other_composed_pattern.rotate(np.pi / 4)
    rotated_pattern.generateInputFiles(0.2, is_displayed=True)

    # 3: And same as shapes, they can be symmetrised
    symmetrised_pattern = other_composed_pattern.symmetrise(5)
    symmetrised_pattern.generateInputFiles(0.2, is_displayed=True)