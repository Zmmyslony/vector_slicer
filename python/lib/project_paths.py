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


"""
This file contains methods for locating directories important for the communication with Vector Slicer and
saving the outputs.
"""

import os
import pathlib

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

def get_slicer_output_directory():
    slicer_directory = get_project_directory()
    output_directory = slicer_directory / "output"
    return output_directory


def get_plot_output_directory():
    plot_output_directory = get_project_directory() / "python" / "output"
    if not os.path.exists(plot_output_directory):
        os.mkdir(plot_output_directory)
    return plot_output_directory