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


import slicer_setup
import output_reading
import input_generation
import shape
import director
import numpy as np

if __name__ == "__main__":
    slicer = slicer_setup.import_slicer("cmake-build-release")

    annulus = shape.annulus(5, 10)
    disk = shape.disk(10)
    rectangle = shape.rectangle(0, 0, 20, 10)
    square = shape.rectangle(-10, -10, 10, 10)

    uniaxial_alignment = director.uniaxial_alignment(0)
    uniaxial_perpendicular_alignment = director.uniaxial_alignment(np.pi / 2)

    radial_alignment = director.radial_symmetry(uniaxial_alignment)
    azimuthal_director = director.radial_symmetry(uniaxial_perpendicular_alignment)
    three_charge_field_alignment = director.charge_field([[-5, -5], [-5, 5], [5, 3]], [1, -1, 1])

    input_generation.generate_input("example_longitudinal_20_10_mm", rectangle, 0.2, 9, uniaxial_alignment,
                                    filling_method="Perimeter")
    input_generation.generate_input("example_azimuthal_10_mm", disk, 0.2, 9,
                                    azimuthal_director, filling_method="Perimeter", is_plotting_shown=False)
    input_generation.generate_input("example_radial_10_mm", disk, 0.2, 9,
                                    radial_alignment, filling_method="Perimeter", is_plotting_shown=False)
    input_generation.generate_input("example_three_charge_field", square, 0.2, 9,
                                    three_charge_field_alignment, filling_method="Splay", is_plotting_shown=False)


    pattern_names = ["example_azimuthal_10_mm", "example_radial_10_mm"]
    pattern_names = ["three_charge_field"]

    for pattern_name in pattern_names:
        input_name = slicer_setup.convert_pattern_name_into_input_name(pattern_name)
        slicer.slice_pattern(input_name, True)

    for pattern_name in pattern_names:
        output_reading.read_fill_matrix(pattern_name)
        output_reading.read_optimisation_sequence(pattern_name)
        output_reading.read_paths(pattern_name)
