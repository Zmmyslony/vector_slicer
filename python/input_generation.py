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


from shape import Shape
import numpy as np
import matplotlib.pyplot as plt
import slicer_setup as slicer
import os


def generate_shape_matrix(shape: Shape, pixel_size):
    x_grid = np.arange(shape.x_min, shape.x_max + pixel_size, pixel_size)
    y_grid = np.arange(shape.y_min, shape.y_max + pixel_size, pixel_size)

    x_mesh, y_mesh = np.meshgrid(x_grid, y_grid, indexing='ij')
    mesh = np.transpose([x_mesh, y_mesh], [1, 2, 0])
    shape_grid = shape.shape_function(mesh)
    return mesh, shape_grid


def plot_shape_matrix(shape_grid, shape: Shape):
    plt.imshow(shape_grid, cmap="Greys", extent=[shape.x_min, shape.x_max, shape.y_min, shape.y_max], origin="lower")
    plt.title("Shape array")
    plt.xlabel("x [mm]")
    plt.ylabel("y [mm]")
    plt.show()


def plot_director(mesh, theta_grid):
    x_vector = np.sin(theta_grid)
    y_vector = np.cos(theta_grid)
    plt.streamplot(mesh[:, 0, 0], mesh[0, :, 1], x_vector, y_vector, density=2)
    plt.title("Director")
    plt.xlabel("x [mm]")
    plt.ylabel("y [mm]")
    ax = plt.gca()
    ax.set_aspect('equal')
    plt.show()


def generate_director_field(mesh, director_function, splay_function=None):
    director = director_function(mesh)
    if splay_function is not None:
        splay = splay_function(mesh)
    else:
        splay = None
    return director, splay


def validate_filling_method(filling_method):
    return filling_method in ["Splay", "Perimeter", "Dual"]


def generate_input(pattern_name, shape: Shape, line_width_millimetre, line_width_pixel, director_function,
                   splay_function=None, filling_method=None, is_plotting_shown=False):
    print("Generating input files for", pattern_name)
    mesh, shape_grid = generate_shape_matrix(shape, line_width_millimetre / line_width_pixel)
    director_grid, splay_grid = generate_director_field(mesh, director_function, splay_function)
    pattern_directory = slicer.get_patterns_directory() / pattern_name

    if not pattern_directory.exists():
        os.mkdir(pattern_directory)

    if not validate_filling_method(filling_method):
        print("\tUndefined filling method. Defaulting to Splay")
        filling_method = "Splay"

    np.savetxt(pattern_directory / "shape.csv", shape_grid, delimiter=',', fmt="%d")
    np.savetxt(pattern_directory / "theta_field.csv", director_grid, delimiter=',', fmt="%.3f")
    if splay_grid is not None:
        flattened_splay = np.transpose(splay_grid, [0, 2, 1])
        flattened_splay = np.reshape(flattened_splay, [flattened_splay.shape[0], flattened_splay.shape[2] * 2])
        np.savetxt(pattern_directory / "theta_field.csv", flattened_splay, delimiter=',', fmt="%.3f")

    config_file = open(pattern_directory / "config.txt", "w")
    config_file.write("PrintRadius " + str(line_width_pixel / 2) + "\n")
    config_file.write("InitialSeedingMethod " + filling_method)
    config_file.close()

    if is_plotting_shown:
        plot_shape_matrix(shape_grid, shape)
        plot_director(mesh, director_grid)
