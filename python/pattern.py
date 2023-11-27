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

from copy import copy
from director import Director
from shape import Shape
import numpy as np
import time
import os
import slicer_setup as slicer
import matplotlib.pyplot as plt
import matplotlib.colors as colors
from mpl_toolkits.axes_grid1 import make_axes_locatable


def domain_director(domain, director_function, default_director=lambda mesh: 0):
    def local_director(mesh):
        return np.where(domain.shape_function(mesh), director_function(mesh), default_director(mesh))

    return local_director


def domain_splay(domain, splay_function, default_splay=lambda mesh: np.zeros_like(mesh)):
    def local_splay(mesh):
        return np.where(domain.shape_function(mesh)[:, :, None], splay_function(mesh), default_splay(mesh))

    return local_splay


def generate_shape_matrix(shape: Shape, pixel_size):
    x_grid = np.arange(shape.x_min, shape.x_max + pixel_size, pixel_size)
    y_grid = np.arange(shape.y_min, shape.y_max + pixel_size, pixel_size)

    x_mesh, y_mesh = np.meshgrid(x_grid, y_grid, indexing='ij')
    mesh = np.transpose([x_mesh, y_mesh], [1, 2, 0])
    shape_grid = shape.shape_function(mesh)
    return mesh, shape_grid


def generate_director_field(mesh, director_function, splay_function):
    director = director_function(mesh)
    splay = splay_function(mesh)
    return director, splay


def validate_filling_method(filling_method):
    return filling_method in ["Splay", "Perimeter", "Dual"]


def plot_pattern(shape_grid, mesh, theta_grid, shape: Shape):
    fig = plt.figure(figsize=[6, 4], dpi=300)
    plt.imshow(shape_grid, vmin=0, vmax=4, cmap="Greys", extent=[shape.x_min, shape.x_max, shape.y_min, shape.y_max],
               origin="lower")
    x_vector = np.cos(theta_grid)
    y_vector = np.sin(theta_grid)
    plt.streamplot(mesh[:, 0, 0].transpose(),
                   mesh[0, :, 1].transpose(),
                   x_vector.transpose(),
                   y_vector.transpose(),
                   density=1)
    plt.title("Pattern")
    plt.xlabel("x [mm]")
    plt.ylabel("y [mm]")
    ax = plt.gca()
    ax.set_aspect('equal')
    ax.set_xlim(shape.x_min - (shape.x_max - shape.x_min) * 0.05, shape.x_max + (shape.x_max - shape.x_min) * 0.05)
    ax.set_ylim(shape.y_min - (shape.y_max - shape.y_min) * 0.05, shape.y_max + (shape.y_max - shape.y_min) * 0.05)
    plt.show()


def plot_splay(mesh, splay):
    splay_norm = np.linalg.norm(splay, axis=2)
    fig = plt.figure(figsize=[6, 4], dpi=300)
    if splay_norm.min() == 0 or splay_norm.max() < splay_norm.min() * 10:
        im = plt.pcolormesh(mesh[:, 0, 0].transpose(),
                            mesh[0, :, 1].transpose(),
                            splay_norm.transpose(),
                            vmin=0,
                            cmap='OrRd')
    else:
        im = plt.pcolormesh(mesh[:, 0, 0].transpose(),
                            mesh[0, :, 1].transpose(),
                            splay_norm.transpose(),
                            norm=colors.LogNorm(vmin=splay_norm.min(), vmax=np.median(splay_norm) * 10),
                            cmap='OrRd')

    plt.title("Splay vector")
    plt.xlabel("x [mm]")
    plt.ylabel("y [mm]")

    ax = plt.gca()
    ax.set_aspect('equal')
    x_min = mesh[:, :, 0].min()
    x_max = mesh[:, :, 0].max()
    y_min = mesh[:, :, 1].min()
    y_max = mesh[:, :, 1].max()
    ax.set_xlim(x_min - (x_max - x_min) * 0.05, x_max + (x_max - x_min) * 0.05)
    ax.set_ylim(y_min - (y_max - y_min) * 0.05, y_max + (y_max - y_min) * 0.05)
    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="5%", pad=0.05)
    plt.colorbar(im, cax=cax)

    ax.streamplot(mesh[:, 0, 0].transpose(),
                  mesh[0, :, 1].transpose(),
                  splay[:, :, 0].transpose(),
                  splay[:, :, 1].transpose(),
                  density=1)
    plt.show()
    plt.close()


class Pattern:
    def __init__(self, domain: Shape, director: Director):
        self.domain = domain
        self.domain_director = domain_director(domain, director.director)
        self.domain_splay = domain_splay(domain, director.splay)

    # NOTE Pattern addition is non-commutative, that is P1 + P2 != P2 + P1, as the second pattern in the pair overwrites
    # the first in the common domain.
    def __add__(self, other):
        shape_copy = copy(self)
        shape_copy.domain = self.domain + other.domain
        shape_copy.domain_director = domain_director(other.domain, other.domain_director, self.domain_director)
        shape_copy.domain_splay = domain_splay(other.domain, other.domain_splay, self.domain_splay)
        return shape_copy

    def generateInputFiles(self, pattern_name, line_width_millimetre, line_width_pixel, filling_method=None,
                           is_plotting_shown=False):
        begin_time = time.time()
        print(f"\n{time.time() - begin_time:.3f}s: Generating input files for {pattern_name}.")
        mesh, shape_grid = generate_shape_matrix(self.domain, line_width_millimetre / line_width_pixel)
        print(f"{time.time() - begin_time:.3f}s: Meshing complete.")
        director_grid, splay_grid = generate_director_field(mesh, self.domain_director, self.domain_splay)
        print(f"{time.time() - begin_time:.3f}s: Director and splay calculation complete.")

        pattern_directory = slicer.get_patterns_directory() / pattern_name
        if not pattern_directory.exists():
            os.mkdir(pattern_directory)

        np.savetxt(pattern_directory / "shape.csv", shape_grid, delimiter=',', fmt="%d")
        print(f"{time.time() - begin_time:.3f}s: Shape grid saved.")

        np.savetxt(pattern_directory / "theta_field.csv", director_grid, delimiter=',', fmt="%.10f")
        print(f"{time.time() - begin_time:.3f}s: Theta grid saved.")

        flattened_splay = np.transpose(splay_grid, [0, 2, 1])
        flattened_splay = np.reshape(flattened_splay, [flattened_splay.shape[0], flattened_splay.shape[2] * 2])
        np.savetxt(pattern_directory / "splay.csv", flattened_splay, delimiter=',', fmt="%.10f")
        print(f"{time.time() - begin_time:.3f}s: Splay grid saved.")

        if not validate_filling_method(filling_method):
            print("\tUndefined filling method. Defaulting to Splay")
        filling_method = "Splay"
        config_file = open(pattern_directory / "config.txt", "w")
        config_file.write("PrintRadius " + str(line_width_pixel / 2) + "\n")
        config_file.write("InitialSeedingMethod " + filling_method)
        config_file.close()
        print(f"{time.time() - begin_time:.3f}s: Configuration file saved.")

        if is_plotting_shown:
            plot_pattern(shape_grid, mesh, director_grid, self.domain)
            if splay_grid is not None:
                plot_splay(mesh, splay_grid)
            print(f"{time.time() - begin_time:.3f}s: Plotting complete.")
