import numpy as np
from scipy.interpolate import griddata
import vtk
from vtk.util import numpy_support as vtk_np
from source.director import Director
from shape import Shape
from pattern import Pattern


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
        return triangles, thetas, bounds
    elif thetas.shape[0] == centroids.shape[0]:
        return centroids, thetas, bounds
    else:
        raise RuntimeError("Length of director array does not match neither the number of cells nor points. Ensure "
                           "that it is properly defined in the input file.")


def interpolate_director_pattern(coordinates: np.ndarray, theta_at_coordinate: np.ndarray, bounds, scale: float = 1):
    coordinates *= scale
    bounds *= scale

    def interpolated_director_function(v):
        """
        :param v: [x_dim, y_dim, 2] array
        :return: director function resulting from interpolation
        """
        v_griddata = np.array([v[:, :, 0].flatten(), v[:, :, 1].flatten()]).transpose()

        director_array = griddata(coordinates[:, 0:2], theta_at_coordinate, v_griddata,
                                  method="cubic", fill_value=0)
        reshaped_director_array = np.reshape(director_array, v.shape[0:2])
        # print(reshaped_director_array.shape)
        return reshaped_director_array

    def interpolated_shape_function(v):
        v_griddata = np.array([v[:, :, 0].flatten(), v[:, :, 1].flatten()]).transpose()
        shape_array = griddata(coordinates[:, 0:2], np.ones(coordinates.shape[0]), v_griddata,
                               method="cubic", fill_value=0)
        reshaped_shape_array = np.array(np.reshape(shape_array, v.shape[0:2]), dtype=bool)
        return reshaped_shape_array

    interpolated_director = Director(interpolated_director_function, derivative_delta=1e-6)
    interpolated_shape = Shape(interpolated_shape_function, bounds)
    interpolated_pattern = Pattern(interpolated_shape, interpolated_director)
    return interpolated_pattern


def read_vtk_pattern(filename, field="theta", scale: float = 1):
    coordinates, thetas, bounds = read_vtk(filename, field)
    return interpolate_director_pattern(coordinates, thetas, bounds, scale)


if __name__ == "__main__":
    patt = read_vtk_pattern(r"C:\Users\zmmys\CLionProjects\ShelloMorph\results\splay_8_5mm_bottom_ansatz.vtk",
                            "progTaus_1", scale=2)
    patt.generateInputFiles("vtk", 0.2, 4, filling_method="splay",
                            is_displayed=True)
