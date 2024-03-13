"""
Implementation shape generation from images.
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

from lib.shape.shape import Shape
import imageio.v3 as iio
import os
import numpy as np
from scipy.interpolate import RegularGridInterpolator


def shape_from_image(file_path, line_width_mm, line_width_pixel, centre_origin=None, scale=1):
    """
    Imports image to be used as shape array.
    :param file_path:
    :param line_width_mm: WARNING: has to agree with what is later used for slicing.
    :param line_width_pixel: WARNING: has to agree with what is later used for slicing.
    :param centre_origin:
    :param scale: rescales the size
    :return:
    """
    if centre_origin is None:
        centre_origin = [0, 0]
    extension = os.path.splitext(file_path)[-1]
    if extension == ".png":
        shape_matrix = np.array(iio.imread(file_path, mode='L'), dtype=float)
        shape_matrix *= -1
        shape_matrix = np.flip(shape_matrix, axis=0)
        shape_matrix = np.transpose(shape_matrix)
    elif extension == ".csv":
        shape_matrix = np.genfromtxt(file_path, delimiter=',')
    else:
        raise Exception(f"Unimplemented image extension {extension}. Use .png or .csv.")

    # Normalize shape matrix
    shape_matrix -= shape_matrix.min()
    shape_matrix /= shape_matrix.max()

    if scale <= 0: raise Warning(f"Scale has to be a positive number.")

    pixel_size = scale * line_width_mm / line_width_pixel
    bounds = np.array([centre_origin[0] - (shape_matrix.shape[0] - 1) * pixel_size / 2,
                       centre_origin[1] - (shape_matrix.shape[1] - 1) * pixel_size / 2,
                       centre_origin[0] + (shape_matrix.shape[0] - 1) * pixel_size / 2,
                       centre_origin[1] + (shape_matrix.shape[1] - 1) * pixel_size / 2])

    x_grid = np.arange(bounds[0], bounds[2] + pixel_size / 2, pixel_size)
    y_grid = np.arange(bounds[1], bounds[3] + pixel_size / 2, pixel_size)

    shape_interpolator = RegularGridInterpolator([x_grid, y_grid], shape_matrix,
                                                 fill_value=0, bounds_error=False)

    def shape_function(v):
        v_flattened = np.vstack([v[:, :, 0].flatten(), v[:, :, 1].flatten()]).transpose()
        shape_flattened = np.round(shape_interpolator(v_flattened))
        return np.array(np.reshape(shape_flattened, v.shape[0:2]), dtype=int)

    return Shape(shape_function, bounds, is_defined_explicitly=False)
