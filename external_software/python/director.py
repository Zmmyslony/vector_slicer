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

import numpy as np


class Director:
    def __init__(self, function, domain=None):
        self.function = function
        self.domain = domain

    # TODO Add addition of director patterns - most likely director will be defined for a given domain and addition
    #  will overwrite the old one
    # def __add__(self, other):
    #     def new_function(v):
    #         np.where(v)


def uniaxial_alignment(theta):
    def director(v):
        return theta * np.ones(v.shape[0:2])

    return director


def radial_symmetry(director, x_offset=0, y_offset=0):
    def symmetric_director(v):
        v -= np.array([x_offset, y_offset])[np.newaxis, np.newaxis, :]
        d = np.linalg.norm(v, axis=2)
        radial_angle = np.arctan2(v[:, :, 1], v[:, :, 0])
        return director(d) + radial_angle

    return symmetric_director