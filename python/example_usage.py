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


from source import slicer_setup, output_reading
from source import directors
from source import shapes
from source.pattern import Pattern
import numpy as np

if __name__ == "__main__":
    # Use the name of the directory in which the slicer has been built in relation to the Vector Slicer source
    # directory.
    slicer = slicer_setup.import_slicer("cmake-build-release")

    # Define domains that are used for pattern generation. See shapes.py for predefined shapes.
    annulus = shapes.annulus(5, 10)
    disk = shapes.disk(10)
    rectangle = shapes.rectangle(0, 0, 20, 10)
    square = shapes.rectangle(-10, -10, 10, 10)

    # Domains can be added or subtracted from another to create new shapes.
    cross = shapes.rectangle(-30, -10, 30, 10) + shapes.rectangle(-10, -30, 10, 30)
    intersection_square_disk = square - disk

    # Define director patterns used for pattern generation. See directors.py for predefined directors.
    uniaxial_longitudinal_director = directors.uniaxial_alignment(0)
    uniaxial_diagonal_director = directors.uniaxial_alignment(np.pi / 4)
    uniaxial_transverse_director = directors.uniaxial_alignment(np.pi / 2)

    radial_alignment = directors.radial()
    azimuthal_director = directors.azimuthal()
    three_charge_field_alignment = directors.charge_field([[-5, -5], [-5, 5], [5, 3]], [1, -1, 1])

    # Combine domains and directors to create patterns
    uniaxial_longitudinal_pattern = Pattern(rectangle, uniaxial_longitudinal_director)
    uniaxial_diagonal_pattern = Pattern(rectangle, uniaxial_diagonal_director)
    uniaxial_transverse_pattern = Pattern(rectangle, uniaxial_transverse_director)
    azimuthal_pattern = Pattern(disk, azimuthal_director)
    radial_pattern = Pattern(disk, radial_alignment)
    three_charge_pattern = Pattern(square, three_charge_field_alignment)

    # Patterns can be combined such that the second pattern overwrites the first one in the domain where it's defined
    first_composite_pattern = three_charge_pattern + azimuthal_pattern
    second_composite_pattern = (Pattern(cross, uniaxial_longitudinal_director) +
                                Pattern(intersection_square_disk, azimuthal_director))

    # Generate input files for defined patterns
    line_width_mm = 0.2
    line_width_pixel = 9
    uniaxial_longitudinal_pattern.generateInputFiles("example_longitudinal_20_10_mm", line_width_mm, line_width_pixel,
                                                     filling_method="Perimeter", is_displayed=True)
    uniaxial_diagonal_pattern.generateInputFiles("example_diagonal_20_10_mm", line_width_mm, line_width_pixel,
                                                 filling_method="Perimeter", is_displayed=True)
    uniaxial_transverse_pattern.generateInputFiles("example_transverse_20_10_mm", line_width_mm, line_width_pixel,
                                                   filling_method="Perimeter", is_displayed=True)
    azimuthal_pattern.generateInputFiles("example_azimuthal_10_mm", line_width_mm, line_width_pixel,
                                         filling_method="Perimeter", is_displayed=True)
    radial_pattern.generateInputFiles("example_radial_10_mm", line_width_mm, line_width_pixel,
                                      filling_method="Perimeter", is_displayed=True)

    three_charge_pattern.generateInputFiles("example_three_charge_field", line_width_mm, line_width_pixel,
                                            filling_method="Splay", is_displayed=True)
    first_composite_pattern.generateInputFiles("example_first_composite_pattern", line_width_mm, line_width_pixel,
                                               filling_method="Perimeter", is_displayed=True)
    second_composite_pattern.generateInputFiles("example_second_composite_pattern", line_width_mm, line_width_pixel,
                                                filling_method="Perimeter", is_displayed=True)

    # Slice patterns
    pattern_names = ["example_longitudinal_20_10_mm",
                     "example_diagonal_20_10_mm",
                     "example_transverse_20_10_mm",
                     "example_azimuthal_10_mm",
                     "example_radial_10_mm",
                     "example_three_charge_field"]

    for pattern_name in pattern_names:
        input_name = slicer_setup.convert_pattern_name_into_input_name(pattern_name)
        slicer.slice_pattern(input_name, True)

    # View sliced patterns
    for pattern_name in pattern_names:
        output_reading.plot_pattern(pattern_name)
        output_reading.plot_disagreement_progress(pattern_name)
