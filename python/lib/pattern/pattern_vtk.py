"""
Implements support for generation of patterns based on .vtk data where the director is defined either on triangles or
nodes.
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

import numpy as np
from scipy.interpolate import griddata
import vtk
from vtk.util import numpy_support as vtk_np
from lib.director.director import Director
from lib.shape.shape import Shape
from lib.pattern.pattern import Pattern
import matplotlib.pyplot as plt


def read_vtk(filename, field="theta"):
    """
    Reads the VTK file, projects the mesh into 2D (first 2 dims are kept), and extracts the mesh and director field.

    The director field can either be defined on points or on cells.
    :param filename:
    :param field: name of the field encoding the director angle.
    :return:
    """
    reader = vtk.vtkPolyDataReader()
    reader.SetFileName(filename)
    reader.ReadAllVectorsOn()
    reader.ReadAllScalarsOn()
    reader.Update()

    data = reader.GetOutput()

    centroid_filter = vtk.vtkCellCenters()
    centroid_filter.SetInputData(data)
    centroid_filter.Update()

    x_min, x_max, y_min, y_max, _, _ = data.GetBounds()
    bounds = np.array([x_min, y_min, x_max, y_max])
    triangles = vtk_np.vtk_to_numpy(data.GetPoints().GetData())
    centroids = vtk_np.vtk_to_numpy(centroid_filter.GetOutput().GetPoints().GetData())
    thetas = vtk_np.vtk_to_numpy(data.GetCellData().GetArray(field))

    if thetas.shape[0] == triangles.shape[0]:
        return triangles[:, :2], thetas, bounds
    elif thetas.shape[0] == centroids.shape[0]:
        return centroids[:, :2], thetas, bounds
    else:
        raise RuntimeError("Length of director array does not match neither the number of cells nor points. Ensure "
                           "that it is properly defined in the input file.")


def interpolate_with_offset(coordinates: np.ndarray, thetas: np.ndarray, xi: np.ndarray, offset: float = 0,
                            method="linear"):
    """ Interpolates thetas with an offset, so that the points where the angle jumps from Pi to 0, are in a different
    part of the pattern. """
    offset_thetas = np.remainder(thetas + offset, np.pi)

    interpolated_data = griddata(coordinates, offset_thetas, xi, method=method)
    return np.remainder(interpolated_data - offset, np.pi)


def median_interpolate(coordinates: np.ndarray, thetas: np.ndarray, xi: np.ndarray, segments: int = 7,
                       method="linear"):
    """ Interpolates the director pattern with angular offsets so that the numerical errors resulting
    from theta jumping from Pi to 0 are in different places, and then takes their median to keep the well-behaved data.
    """
    interpolated_data_sequence = np.empty(shape=[xi.shape[0], segments])
    for i in range(segments):
        interpolated_data_sequence[:, i] = interpolate_with_offset(coordinates, thetas, xi, method=method,
                                                                   offset=np.pi * i / segments)
    return np.median(interpolated_data_sequence, axis=1)


def interpolate_director_pattern(coordinates: np.ndarray, thetas: np.ndarray, bounds, scale: float = 1,
                                 method="linear", segments: int = 7):
    coordinates *= scale
    bounds *= scale

    def interpolated_shape_function(v):
        v_griddata = np.array([v[:, :, 0].flatten(), v[:, :, 1].flatten()]).transpose()
        shape_array = griddata(coordinates, thetas, v_griddata, fill_value=0, method=method)
        reshaped_shape_array = np.array(np.reshape(shape_array, v.shape[0:2]), dtype=bool)
        return reshaped_shape_array

    def interpolated_director_function(v):
        """
        :param v: [x_dim, y_dim, 2] array
        :return: director function resulting from interpolation
        """
        v_griddata = np.array([v[:, :, 0].flatten(), v[:, :, 1].flatten()]).transpose()
        director_array = median_interpolate(coordinates, thetas, v_griddata, method=method, segments=segments)
        reshaped_director_array = np.reshape(director_array, v.shape[0:2])

        return reshaped_director_array

    interpolated_director = Director(interpolated_director_function, derivative_delta=1e-6)
    interpolated_shape = Shape(interpolated_shape_function, bounds, is_defined_explicitly=True)
    interpolated_pattern = Pattern(interpolated_shape, interpolated_director)
    return interpolated_pattern


def read_vtk_pattern(filename, field="theta", scale: float = 1):
    coordinates, thetas, bounds = read_vtk(filename, field)
    return interpolate_director_pattern(coordinates, thetas, bounds, scale)


if __name__ == "__main__":
    data = np.genfromtxt(
        r"C:\Users\zmmys\OneDrive - University of Cambridge\Projects\1. In progress\Printing for Dan\spherewrapping_director_num_pet=3.txt",
    delimiter=",")

    plt.scatter(data[..., 0], data[..., 1], c=data[..., 2], s=0.1)
    plt.show()