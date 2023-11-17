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

import ctypes
import pathlib
import sys


def get_project_directory():
    return pathlib.Path().absolute().parent


def get_patterns_directory():
    return get_project_directory() / "patterns"


def configure_slicer(slicer):
    slicer.slice_pattern.argtypes = [ctypes.c_char_p]
    slicer.slice_pattern_with_config.argtypes = [ctypes.c_char_p, ctypes.c_char_p]


def import_slicer(build_directory):
    project_directory = get_project_directory()

    if sys.platform == "win32":
        library_name = "vector_slicer_api.dll"
    else:
        library_name = "vector_slicer_api.so"
    vector_slicer_lib_path = project_directory / build_directory / library_name
    slicer = ctypes.CDLL(str(vector_slicer_lib_path))

    configure_slicer(slicer)
    return slicer


def convert_pattern_name_into_input_name(pattern_name):
    patterns_directory = get_patterns_directory()
    pattern_name = patterns_directory / "example_azimuthal_10_mm"
    pattern_name_b = str(pattern_name).encode('utf-8')
    return pattern_name_b
