"""
Configuration of the slicer library.
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

import ctypes
import pathlib
import sys
import os


def get_project_directory():
    """
    :return: Vector Slicer directory
    """
    return pathlib.Path().absolute().parent


def get_patterns_directory():
    """
    :return: Patterns (input) directory.
    """
    return get_project_directory() / "patterns"


def configure_slicer(slicer):
    """
    Defines the types used by slicer library functions.
    :param slicer:
    :return:
    """
    slicer.slice_pattern.argtypes = [ctypes.c_char_p]
    slicer.slice_pattern_with_config.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    slicer.re_slice_pattern.argtypes = [ctypes.c_char_p]


def import_slicer(build_directory):
    """
    Imports slicer library and configures it for operation.
    :param build_directory:
    :return:
    """
    project_directory = get_project_directory()


    if sys.platform == "win32":
        library_name = "vector_slicer_api.dll"
    elif sys.platform == "linux":
        library_name = "libvector_slicer_api.so"
    elif sys.platform == "darwin":
        library_name = "libvector_slicer_api.dylib"
    else:
        raise Exception("Unrecognised platform " + sys.platform)

    vector_slicer_lib_path = project_directory / build_directory / library_name
    if not os.path.exists(vector_slicer_lib_path):
        print(f"Vector Slicer Api does not exist in \"{vector_slicer_lib_path}\". Remember to build it and choose the "
              f"correct build directory.")
        raise Exception('api_not_found')

    slicer = ctypes.CDLL(str(vector_slicer_lib_path))
    return slicer


def convert_pattern_name_into_input_name(pattern_name: str):
    """
    Converts string filename to binary format used by the library.
    :param pattern_name:
    :return: UTF-8 encoded path.
    """
    patterns_directory = get_patterns_directory()
    pattern_name = patterns_directory / pattern_name
    pattern_name_b = str(pattern_name).encode('utf-8')
    return pattern_name_b
