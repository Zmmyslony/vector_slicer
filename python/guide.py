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


from lib import slicer_setup, output_reading
from lib.director import basic as directors
from lib.shape import basic as shapes
from lib.pattern.pattern import Pattern, SymmetricPattern
import numpy as np


# This file contains examples of a few basic functionalities of the program. All units are in millimetres.

def complete_usage_example(slicer):
    """
    Generation of the input files for uniaxial pattern, slicing it and visualising the result.
    :return:
    """
    # Define domains that are used for pattern generation. See basic.py for predefined shapes.
    rectangle = shapes.rectangle(0, 0, 20, 10)

    # Define director patterns used for pattern generation. See basic.py for predefined directors.
    uniaxial_x_director = directors.uniaxial_alignment(0)

    # Combine domains and directors to create patterns.
    uniaxial_longitudinal_pattern = Pattern(rectangle, uniaxial_x_director)

    # Width/spacing of the paths that is obtained experimentally.
    line_width = 0.2

    # Generate input files for defined patterns. Their names are then used for slicing.
    uniaxial_longitudinal_pattern.generateInputFiles("example_longitudinal_20_10_mm", line_width,
                                                     filling_method="Perimeter", is_displayed=True)

    # Pattern name needs to be first encoded
    input_name = slicer_setup.convert_pattern_name_into_input_name("example_longitudinal_20_10_mm")
    # Slices the pattern specified by name. Second boolean option allows us to use default configuration (True) or
    # to modify it (False).
    slicer.slice_pattern(input_name, True)

    # Plots the sliced pattern.
    output_reading.plot_pattern("example_longitudinal_20_10_mm")
    # Plots how the disagreement developed during the optimisation.
    output_reading.plot_disagreement_progress("example_longitudinal_20_10_mm")


# Examples below do not generate any gcode files but only demonstrate how input files can be generated.

def shape_addition(line_width):
    """ Adds two rectangular shapes to form a cross with uniaxial alignment."""
    x_rectangle = shapes.rectangle(-10, -5, 10, 5)
    y_rectangle = shapes.rectangle(-5, -10, 5, 10)

    cross_shape = x_rectangle + y_rectangle
    uniaxial_x_director = directors.uniaxial_alignment(0)

    cross_pattern = Pattern(cross_shape, uniaxial_x_director)

    cross_pattern.generateInputFiles(None, line_width, is_displayed=True)


def shape_subtraction(line_width):
    """ Subtracts the disk from a rectangle to create a pattern with a hole."""
    x_rectangle = shapes.rectangle(-10, -5, 10, 5)
    disk = shapes.disk(2.5)

    rectangle_with_hole = x_rectangle - disk
    uniaxial_x_director = directors.uniaxial_alignment(0)

    pattern = Pattern(rectangle_with_hole, uniaxial_x_director)
    pattern.generateInputFiles(None, line_width, is_displayed=True)


def shape_rotation(line_width):
    """ Creates a rectangle in y-direction through rotation of a rectangle in x-direction."""
    x_rectangle = shapes.rectangle(-10, -5, 10, 5)
    y_rectangle = x_rectangle.rotate(np.pi / 2)

    uniaxial_x_director = directors.uniaxial_alignment(0)

    pattern = Pattern(y_rectangle, uniaxial_x_director)
    pattern.generateInputFiles(None, line_width, is_displayed=True)


def shape_symmetrisation(line_width):
    """ Symmetrises a rectangular shape into a 5-armed one, with global, uniaxial director."""
    x_rectangle = shapes.rectangle(0, -5, 20, 5)
    five_armed_shape = x_rectangle.symmetrise(5)

    uniaxial_x_director = directors.uniaxial_alignment(0)

    pattern = Pattern(five_armed_shape, uniaxial_x_director)
    pattern.generateInputFiles(None, line_width, is_displayed=True)


def pattern_addition(line_width):
    """ Adds an uniaxial rectangular pattern to azimuthal circular one in both orders (a + b vs b + a)."""
    x_rectangle = shapes.rectangle(0, -5, 20, 5)
    disk = shapes.disk(2.5)

    uniaxial_x_director = directors.uniaxial_alignment(0)
    azimuthal_director = directors.azimuthal()

    uniaxial_pattern = Pattern(x_rectangle, uniaxial_x_director)
    azimuthal_pattern = Pattern(disk, azimuthal_director)

    # Addition of patterns, overwrites the director of the first pattern with the director of the second one in common
    # domains.
    (uniaxial_pattern + azimuthal_pattern).generateInputFiles(None, line_width, is_displayed=True)
    (azimuthal_pattern + uniaxial_pattern).generateInputFiles(None, line_width, is_displayed=True)


def pattern_symmetrisation(line_width):
    """ Creates a five-armed pattern with uniaxial alignment in each arm. """
    x_rectangle = shapes.rectangle(0, -5, 20, 5)
    disk = shapes.disk(2.5, 20, 5)
    joined_shape = x_rectangle + disk

    uniaxial_x_director = directors.uniaxial_alignment(0)
    # Patterns can be symmetrised similarly to the shape.
    symmetrised_pattern = SymmetricPattern(joined_shape, uniaxial_x_director, 5)
    symmetrised_pattern.generateInputFiles(None, line_width, is_displayed=True)


def example_extensive():
    """ This example shows a whole workflow with multiple patterns being generated, sliced and plotted."""
    # The path is relative to the parent of the Slicer's lib files, i.e. the directory into which the project
    # was pulled if the installation guide was directly followed.
    slicer = slicer_setup.import_slicer("build")

    # Define domains that are used for pattern generation. See basic.py for predefined shapes.
    annulus = shapes.annulus(5, 10)
    disk = shapes.disk(10)
    rectangle = shapes.rectangle(0, 0, 20, 10)
    square = shapes.rectangle(-10, -10, 10, 10)

    # Domains can be added or subtracted from another to create new shapes.
    cross = shapes.rectangle(-30, -10, 30, 10) + shapes.rectangle(-10, -30, 10, 30)
    intersection_square_disk = square - disk

    # Define director patterns used for pattern generation. See basic.py for predefined directors.
    uniaxial_longitudinal_director = directors.uniaxial_alignment(0)
    uniaxial_diagonal_director = directors.uniaxial_alignment(np.pi / 4)
    uniaxial_transverse_director = directors.uniaxial_alignment(np.pi / 2)

    radial_alignment = directors.radial()
    azimuthal_director = directors.azimuthal()
    three_charge_field_alignment = directors.charge_field([[-5, -5], [-5, 5], [5, 3]], [1, -1, 1])

    # Combine domains and directors to create patterns.
    uniaxial_longitudinal_pattern = Pattern(rectangle, uniaxial_longitudinal_director)
    uniaxial_diagonal_pattern = Pattern(rectangle, uniaxial_diagonal_director)
    uniaxial_transverse_pattern = Pattern(rectangle, uniaxial_transverse_director)
    azimuthal_pattern = Pattern(disk, azimuthal_director)
    radial_pattern = Pattern(disk, radial_alignment)
    three_charge_pattern = Pattern(square, three_charge_field_alignment)

    # Patterns can be added in a way that the second pattern overwrites the first one in common domains.
    first_composite_pattern = three_charge_pattern + azimuthal_pattern
    second_composite_pattern = (Pattern(cross, uniaxial_longitudinal_director) +
                                Pattern(intersection_square_disk, azimuthal_director))

    # Generate input files for defined patterns. Their names are then used for slicing.
    line_width_mm = 0.2
    uniaxial_longitudinal_pattern.generateInputFiles("example_longitudinal_20_10_mm", line_width_mm,
                                                     filling_method="Perimeter", is_displayed=True)
    uniaxial_diagonal_pattern.generateInputFiles("example_diagonal_20_10_mm", line_width_mm,
                                                 filling_method="Perimeter", is_displayed=True)
    uniaxial_transverse_pattern.generateInputFiles("example_transverse_20_10_mm", line_width_mm,
                                                   filling_method="Perimeter", is_displayed=True)
    azimuthal_pattern.generateInputFiles("example_azimuthal_10_mm", line_width_mm,
                                         filling_method="Perimeter", is_displayed=True)
    radial_pattern.generateInputFiles("example_radial_10_mm", line_width_mm,
                                      filling_method="Perimeter", is_displayed=True)

    three_charge_pattern.generateInputFiles("example_three_charge_field", line_width_mm,
                                            filling_method="Splay", is_displayed=True)
    first_composite_pattern.generateInputFiles("example_first_composite_pattern", line_width_mm,
                                               filling_method="Perimeter", is_displayed=True)
    second_composite_pattern.generateInputFiles("example_second_composite_pattern", line_width_mm,
                                                filling_method="Perimeter", is_displayed=True)

    # Slice patterns
    pattern_names = ["example_longitudinal_20_10_mm",
                     "example_diagonal_20_10_mm",
                     "example_transverse_20_10_mm",
                     "example_azimuthal_10_mm",
                     "example_radial_10_mm",
                     "example_three_charge_field"]

    for pattern_name in pattern_names:
        # Pattern name needs to be first encoded
        input_name = slicer_setup.convert_pattern_name_into_input_name(pattern_name)
        # Slices the pattern specified by name. Second boolean option allows us to use default configuration (True) or
        # to modify it (False).
        slicer.slice_pattern(input_name, True)

    # View sliced patterns
    for pattern_name in pattern_names:
        output_reading.plot_pattern(pattern_name)
        output_reading.plot_disagreement_progress(pattern_name)


if __name__ == "__main__":
    # The path is relative to the parent of the Slicer's lib files, i.e. the directory into which the project
    # was pulled if the installation guide was directly followed.
    slicer = slicer_setup.import_slicer("build")
    complete_usage_example(slicer)

    line_width = 0.2
    shape_addition(line_width)
    shape_subtraction(line_width)
    shape_rotation(line_width)
    shape_symmetrisation(line_width)

    pattern_addition(line_width)
    pattern_symmetrisation(line_width)

    # example_extensive()
