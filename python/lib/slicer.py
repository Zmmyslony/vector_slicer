"""
Slicer object used for slicing the director patterns.
"""

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

from lib import slicer_setup
from lib.pattern.pattern import Pattern


class Slicer:
    def __init__(self, build_directory=None):
        self.slicer = slicer_setup.import_slicer(build_directory)
        slicer_setup.configure_slicer(self.slicer)

    def slice(self, pattern: str | Pattern, use_default=True):
        """
        Slices the pattern name using either default configuration or opening a prompt asking for modifications.
        :param pattern:
        :param use_default:
        :return:
        """
        input_name = slicer_setup.convert_pattern_name_into_input_name(pattern)
        self.slicer.slice_pattern(input_name, use_default)

    def re_slice(self, pattern: str | Pattern):
        """
        Re-slices the pattern using the best config exported during prior optimisation.
        :param pattern:
        :return:
        """
        input_name = slicer_setup.convert_pattern_name_into_input_name(pattern)
        self.slicer.re_slice_pattern(input_name)

    def slice_seeds_only(self, pattern: str | Pattern, seeds: int):
        """
        Slices the pattern using the config in the input directory for a given number of seeds.
        :param pattern:
        :param seeds:
        :return:
        """
        input_name = slicer_setup.convert_pattern_name_into_input_name(pattern)
        self.slicer.slice_pattern_seeds_only(input_name, seeds)

    def slice_variable_width(self, pattern: str | Pattern, seeds: int):
        """
                Slices the pattern using the config in the input with a variable width preset for a given number of seeds.
                :param pattern:
                :param seeds:
                :return:
                """
        input_name = slicer_setup.convert_pattern_name_into_input_name(pattern)
        self.slicer.slice_pattern_variable_width(input_name, seeds)