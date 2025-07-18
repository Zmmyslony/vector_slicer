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
This file contains the minimal working example for the Slicer, starting from pattern definition, through slicing
and visualising the results.

The workflow starts by creating a Shape and a Director, which together form a Pattern. The pattern is then
used to generate the input files for the Vector Slicer, which are then used to slice the pattern. The results
are then plotted.

"""

from lib import output_reading
from lib.slicer import Slicer
from lib.director import basic as directors
from lib.shape import basic as shapes
from lib.pattern.pattern import Pattern

if __name__ == "__main__":
    # Definition of a disk shape of a radius 5 mm
    disk = shapes.disk(5)

    # Definition of the desired director pattern, here - radial
    radial_director = directors.radial()

    # Shape and Director are combined to create a pattern with specified name
    radial_pattern = Pattern(disk, radial_director, "radial_r_5_mm")

    # Input files are created for the specified pattern, using experimental print width of 0.2 mm
    radial_pattern.generateInputFiles(0.2)

    # Slicer needs to be initialised before it can be used for slicing
    slicer = Slicer()
    slicer.slice(radial_pattern)

    # Display the slicing output
    output_reading.plot_pattern(radial_pattern)
