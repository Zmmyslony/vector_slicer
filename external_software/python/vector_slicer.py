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

if __name__ == "__main__":
    slicer = slicer_setup.import_slicer("cmake-build-release")

    pattern_name = "example_azimuthal_10_mm"
    # pattern_name = "example_longitudinal_20_10_mm"
    input_name = slicer_setup.convert_pattern_name_into_input_name(pattern_name)

    # slicer.slice_pattern(input_name)
    output_reading.read_fill_matrix(pattern_name)
    # output_reading.read_optimisation_sequence(pattern_name)
    # output_reading.read_paths(pattern_name)