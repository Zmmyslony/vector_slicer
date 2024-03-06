"""
Base class defining pattern operations and input file generation together with plotting the input files.
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

from copy import copy
import numpy as np
import time
import os
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib as mpl
from mpl_toolkits.axes_grid1 import make_axes_locatable

from lib.director.director import Director
from lib.shape.shape import Shape
from lib import slicer_setup as slicer


def plot_pattern(shape_grid, mesh, theta_grid, shape: Shape):
    fig = plt.figure(figsize=[6, 4], dpi=300)
    color_values = np.linspace(1, 0.666, 2)
    color_list = np.char.mod('%f', color_values)

    discrete_colormap = mpl.colors.ListedColormap(color_list)
    colormap_bounds = mpl.colors.BoundaryNorm(np.arange(-0.5, 2), discrete_colormap.N)

    local_shape_grid = copy(shape_grid)
    plt.imshow(np.transpose(local_shape_grid), extent=[shape.x_min, shape.x_max, shape.y_min, shape.y_max],
               origin="lower", cmap=discrete_colormap, norm=colormap_bounds)

    ax = plt.gca()
    ax.set_aspect('equal')
    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="5%", pad=0.05)
    plt.colorbar(ticks=np.arange(2), label="is filled", cax=cax)
    ax.set_xlim(shape.x_min - (shape.x_max - shape.x_min) * 0.05, shape.x_max + (shape.x_max - shape.x_min) * 0.05)
    ax.set_ylim(shape.y_min - (shape.y_max - shape.y_min) * 0.05, shape.y_max + (shape.y_max - shape.y_min) * 0.05)

    x_vector = np.cos(theta_grid)
    y_vector = np.sin(theta_grid)
    ax.streamplot(mesh[:, 0, 0].transpose(),
                  mesh[0, :, 1].transpose(),
                  x_vector.transpose(),
                  y_vector.transpose(),
                  density=1)
    ax.set_title("Pattern")
    ax.set_xlabel("x [mm]")
    ax.set_ylabel("y [mm]")

    plt.show()


def plot_splay(mesh, splay):
    splay_norm = np.linalg.norm(splay, axis=2)
    fig = plt.figure(figsize=[6, 4], dpi=300)
    if splay_norm.min() == 0 or splay_norm.max() < splay_norm.min() * 10:
        im = plt.pcolormesh(mesh[:, 0, 0].transpose(),
                            mesh[0, :, 1].transpose(),
                            splay_norm.transpose(),
                            vmin=0,
                            vmax=1,
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
    plt.colorbar(im, cax=cax, label="splay")

    ax.streamplot(mesh[:, 0, 0].transpose(),
                  mesh[0, :, 1].transpose(),
                  splay[:, :, 0].transpose(),
                  splay[:, :, 1].transpose(),
                  density=1)
    plt.show()
    plt.close()


def domain_director(domain, director_function, default_director=lambda mesh: 0):
    def local_director(mesh):
        return np.where(domain.shape_function(mesh), director_function(mesh), default_director(mesh))

    return local_director


def domain_splay(domain, splay_function, default_splay=lambda mesh: np.zeros_like(mesh)):
    def local_splay(mesh):
        return np.where(domain.shape_function(mesh)[:, :, None], splay_function(mesh), default_splay(mesh))

    return local_splay


def generate_shape_matrix(shape: Shape, pixel_size: float):
    x_grid = np.arange(shape.x_min, shape.x_max + pixel_size / 2, pixel_size)
    y_grid = np.arange(shape.y_min, shape.y_max + pixel_size / 2, pixel_size)

    x_mesh, y_mesh = np.meshgrid(x_grid, y_grid, indexing='ij')
    mesh = np.transpose([x_mesh, y_mesh], [1, 2, 0])
    shape_grid = shape.shape_function(mesh)
    return mesh, shape_grid


def generate_director_field(mesh, director_function, splay_function):
    director = director_function(mesh)
    splay = splay_function(mesh)
    return director, splay


def is_filling_method_valid(filling_method):
    if not isinstance(filling_method, str):
        return False
    return filling_method.lower() in ["splay", "perimeter", "dual"]


class Pattern:
    def __init__(self, domain: Shape, director: Director, name=None):
        """
        Combination of the Director and Shape, where the director is defined over the domain.
        :param domain:
        :param director:
        """
        self.name = name
        self.domain = domain
        self.shape_list = [domain]
        self.director_list = [director]
        self.domain_director = domain_director(domain, director.director)
        self.domain_splay = domain_splay(domain, director.splay)

    def __add__(self, other):
        """
        WARNING: Addition is non-commutative as the second pattern overwrites the first in common domain.
        :param other:
        :return:
        """
        shape_copy = copy(self)
        shape_copy.domain = self.domain + other.domain
        shape_copy.shape_list = self.shape_list + other.shape_list
        shape_copy.director_list = self.director_list + other.director_list
        shape_copy.domain_director = domain_director(other.domain, other.domain_director, self.domain_director)
        shape_copy.domain_splay = domain_splay(other.domain, other.domain_splay, self.domain_splay)
        return shape_copy

    def generateInputFiles(self, line_width_millimetre: float, pattern_name = None, line_width_pixel: int = 9,
                           filling_method=None, is_displayed=False):
        """
        Generates theta, splay and config files for the pattern.
        :param line_width_millimetre: printing line width used for meshing.
        :param pattern_name: directory name, if is None, the self.name is used. If it is also None, the pattern is not saved for slicing.
        :param line_width_pixel: width of the line in pixels - higher values result in better slicing quality at the cost of runtime.
        :param filling_method: Splay, Perimeter or Dual.
        :param is_displayed: Is the pattern displayed after the generation.
        :return: pattern_name
        """
        if pattern_name is None:
            pattern_name = self.name

        stage_count = 3
        if pattern_name is not None: stage_count += 1
        if is_displayed: stage_count += 1

        begin_time = time.time()
        print(f"\n{time.time() - begin_time:.3f}s: [1/{stage_count:d}] Generating input files for {pattern_name}.")
        mesh, shape_grid = generate_shape_matrix(self.domain, line_width_millimetre / line_width_pixel)
        print(f"{time.time() - begin_time:.3f}s: [2/{stage_count:d}] Meshing complete.")
        director_grid, splay_grid = generate_director_field(mesh, self.domain_director, self.domain_splay)
        print(f"{time.time() - begin_time:.3f}s: [3/{stage_count:d}] Director and splay calculation complete.")

        current_stage = 3
        if pattern_name is not None:
            pattern_directory = slicer.get_patterns_directory() / pattern_name
            if not pattern_directory.exists():
                if not slicer.get_patterns_directory().exists():
                    os.mkdir(slicer.get_patterns_directory())
                os.mkdir(pattern_directory)

            np.savetxt(pattern_directory / "shape.csv", shape_grid, delimiter=',', fmt="%d")
            np.savetxt(pattern_directory / "theta_field.csv", director_grid, delimiter=',', fmt="%.10f")
            flattened_splay = np.reshape(splay_grid, [splay_grid.shape[0], splay_grid.shape[1] * 2])
            np.savetxt(pattern_directory / "splay.csv", flattened_splay, delimiter=',', fmt="%.10f")

            if not is_filling_method_valid(filling_method):
                print("\tUndefined filling method. Defaulting to Splay")
                filling_method = "Splay"
            config_file = open(pattern_directory / "config.txt", "w")
            config_file.write("PrintRadius " + str(line_width_pixel / 2) + "\n")
            config_file.write("InitialSeedingMethod " + filling_method.capitalize())
            config_file.close()
            current_stage += 1
            print(f"{time.time() - begin_time:.3f}s: [{current_stage:d}/{stage_count:d}] Input files exported.")

        if is_displayed:
            plot_pattern(shape_grid, mesh, director_grid, self.domain)
            if splay_grid is not None:
                plot_splay(mesh, splay_grid)
            current_stage += 1
            print(f"{time.time() - begin_time:.3f}s: [{current_stage:d}/{stage_count:d}]  Plotting complete.")

        return pattern_name

    def symmetrise(self, arm_number: int, begin_angle: float = None):
        if arm_number < 1:
            raise RuntimeError("Symmetrisation requires a positive number of arms")
        elif arm_number == 1:
            return self

        symmetrised_pattern = SymmetricPattern(self.shape_list[0], self.director_list[0], arm_number, begin_angle)
        patterns_number = len(self.shape_list)
        for i in range(1, patterns_number):
            symmetrised_pattern += SymmetricPattern(self.shape_list[i], self.director_list[i], arm_number, begin_angle)
        return symmetrised_pattern


def SymmetricPattern(shape: Shape, director: Director, arm_number: int, begin_angle: float = None):
    """
    Creates a symmetric pattern with a selected number of arms, where each arm is copy of the initial one.
    E.g. 4-armed symmetrisation of a rectangle is a cross.
    :param shape:
    :param director:
    :param arm_number:
    :param begin_angle: defines initial arm which will range between begin_angle and begin_angle + 2 pi / arm_number
    :return:
    """
    if arm_number <= 1:
        return Pattern(shape, director)
    sector_angular_size = 2 * np.pi / arm_number
    if begin_angle is None:
        begin_angle = -1 / 2 * sector_angular_size

    base_domain = shape.angular_slice(begin_angle, begin_angle + sector_angular_size)
    symmetrised_pattern = Pattern(base_domain, director)
    for i in range(1, arm_number):
        segment_domain = base_domain.rotate(sector_angular_size * i)
        segment_director = director.rotate(sector_angular_size * i)
        segment = Pattern(segment_domain, segment_director)
        symmetrised_pattern += segment
    return symmetrised_pattern
