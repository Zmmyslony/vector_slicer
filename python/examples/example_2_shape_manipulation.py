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
This file contains information about how Shapes can be manipulated to create complex geometries.

These operations include: addition, subtraction, rotation, and symmetrisation.

These shapes are shown using streamplots, without the slicing, to speed up file generation. The list of shapes
is not exhaustive, and we refer the user to explore the lib.shape director to see full range of implemented shapes.

"""

from lib.director import basic as directors
from lib.shape import basic as shapes
from lib.pattern.pattern import Pattern
import numpy as np

if __name__ == "__main__":
    # 1: Shapes can be combined through addition
    disk = shapes.disk(5)
    thin_rectangle = shapes.rectangle(-2, -20, 2, 0)
    lolipop_shape = disk + thin_rectangle

    # Definition of a director in order to be able to create a Pattern for visualisation.
    # This director has no effect on shape operations.
    director = directors.uniaxial_alignment(0)

    # Pattern specified with a name None, will not generate input files, but can be used for visualisation.
    # Generally, this is recommended when designing the pattern as slicing is the computationally heavy aspect
    lolipop_pattern = Pattern(lolipop_shape, director, name=None)
    lolipop_pattern.generateInputFiles(0.2, is_displayed=True)

    # 2: Analogously, we can create shapes through subtraction
    disk = shapes.disk(5)
    square = shapes.rectangle(-10, -10, 10, 10)
    square_with_hole = square - disk

    square_with_hole_pattern = Pattern(square_with_hole, director, name=None)
    square_with_hole_pattern.generateInputFiles(0.2, is_displayed=True)

    # 3: Shapes can be also rotated
    rotated_square_with_hole = square_with_hole.rotate(np.pi / 4)
    rotated_square_with_hole_pattern = Pattern(rotated_square_with_hole, director, name=None)
    rotated_square_with_hole_pattern.generateInputFiles(0.2, is_displayed=True)

    # 4: Shapes can be also be symmetrised
    arm = shapes.rectangle(0, -2, 10, 2) + shapes.disk(2, 10, 0)
    symmetric_shape = arm.symmetrise(5)
    symmetric_pattern = Pattern(symmetric_shape, director, name=None)
    symmetric_pattern.generateInputFiles(0.2, is_displayed=True)

    # 5. Shapes can also be created using a more fundamental, polygon-based way:
    trapezoid_shape = shapes.polygon
