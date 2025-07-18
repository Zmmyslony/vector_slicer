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

"""
Base class defining pattern operations and input file generation together with plotting the input files.
"""

from copy import copy
import math
import numpy as np
import time
import os
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib as mpl
from mpl_toolkits.axes_grid1 import make_axes_locatable

from lib.director.director import Director
from lib.project_paths import get_plot_output_directory
from lib.shape.shape import Shape
from lib.shape.basic import regular_hexagon, square, regular_triangle
from lib import project_paths

def refine_low_magnitude_splay(shape_grid, theta_grid, splay):
    splay_magnitude = np.linalg.norm(splay, axis=2)
    x_vector = np.where(shape_grid, np.cos(theta_grid), 0)
    y_vector = np.where(shape_grid, np.sin(theta_grid), 0)
    director = np.stack([x_vector, y_vector], axis=2)

    return np.where(
        np.logical_and(splay_magnitude[..., None] < 0.001, shape_grid[..., None]),
        director * (splay_magnitude[..., None] + 0.001),
        splay)


def plot_pattern(shape_grid, mesh, theta_grid, shape: Shape, splay, filename=None,
                 is_splay_shown=False, is_labels_shown=False, is_director_shown=True):
    mpl.rcParams.update({'font.size': 8})
    refine_low_magnitude_splay(shape_grid, theta_grid, splay)
    fig = plt.figure(figsize=[4, 3], dpi=450)

    x_min = mesh[:, :, 0].min()
    x_max = mesh[:, :, 0].max()
    y_min = mesh[:, :, 1].min()
    y_max = mesh[:, :, 1].max()

    x_len = x_max - x_min
    y_len = y_max - y_min

    if x_len > y_len:
        streamplot_density = [1, float(y_len) / float(x_len)]
    else:
        streamplot_density = [float(x_len) / float(y_len), 1]

    discrete_colormap = mpl.colors.ListedColormap(["white", "lightgray"])
    colormap_bounds = mpl.colors.BoundaryNorm([0, 0.5], discrete_colormap.N)

    local_shape_grid = copy(shape_grid)
    ax = plt.gca()
    ax.imshow(np.transpose(local_shape_grid),
              extent=[shape.x_min, shape.x_max, shape.y_min, shape.y_max],
              origin="lower", cmap=discrete_colormap, norm=colormap_bounds, rasterized=True,
              zorder=0)

    ax.set_aspect('equal')
    divider = make_axes_locatable(ax)
    # cax = divider.append_axes("right", size="5%", pad=0.05)
    # plt.colorbar(ticks=np.arange(2), label="", cax=cax)
    dx = (x_max - x_min) * 0.02
    dy = (y_max - y_min) * 0.02
    ax.set_xlim(x_min - dx, x_max + dx)
    ax.set_ylim(y_min - dy, y_max + dy)

    x_mesh = np.linspace(x_min, x_max, shape_grid.shape[0], endpoint=True)
    y_mesh = np.linspace(y_min, y_max, shape_grid.shape[1], endpoint=True)
    x_vector = np.where(shape_grid, np.cos(theta_grid), 0)
    y_vector = np.where(shape_grid, np.sin(theta_grid), 0)

    refined_splay = refine_low_magnitude_splay(shape_grid, theta_grid, splay)
    splay_norm = np.linalg.norm(refined_splay, axis=2)

    if is_director_shown:
        ax.streamplot(x_mesh,
                      y_mesh,
                      x_vector.transpose(),
                      y_vector.transpose(),
                      density=streamplot_density,
                      arrowsize=0,
                      integration_direction="both",
                      zorder=1)

    if is_splay_shown:
        resolution = 15
        x_size = splay.shape[0]
        y_size = splay.shape[1]
        if x_size > resolution and x_size >= y_size:
            scale = int(x_size / resolution)
        elif y_size > resolution and y_size > x_size:
            scale = int(y_size / resolution)
        else:
            scale = 1
        index_offset = int(scale / 2)

        ax.quiver(x_mesh[index_offset::scale],
                  y_mesh[index_offset::scale],
                  refined_splay[index_offset::scale, index_offset::scale, 0].transpose(),
                  refined_splay[index_offset::scale, index_offset::scale, 1].transpose(),
                  color="tab:orange",
                  zorder=2,
                  width=0.008
                  )

    if is_labels_shown:
        ax.set_xlabel("x [mm]")
        ax.set_ylabel("y [mm]")
    else:
        ax.set_yticklabels([])
        ax.set_xticklabels([])
        ax.axis('off')

    plt.tight_layout()
    if filename is None:
        plt.show()
    else:
        plt.savefig(filename, format='png', transparent=True)
        plt.close()


def domain_director(domain, director_function, default_director=lambda mesh: 0):
    def local_director(mesh):
        return np.where(domain.shape_function(mesh), director_function(mesh), default_director(mesh))

    return local_director


def domain_splay(domain, splay_function, default_splay=lambda mesh: np.zeros_like(mesh)):
    def local_splay(mesh):
        return np.where(domain.shape_function(mesh)[:, :, None], splay_function(mesh), default_splay(mesh))

    return local_splay


def generate_shape_matrix(shape: Shape, pixel_size: float, x_excess: float = 0, y_excess: float = 0):
    shape.x_max += x_excess
    shape.y_max += y_excess

    x_grid = np.arange(shape.x_min - pixel_size, shape.x_max + pixel_size * 3 / 2, pixel_size, dtype=np.double)
    y_grid = np.arange(shape.y_min - pixel_size, shape.y_max + pixel_size * 3 / 2, pixel_size, dtype=np.double)

    x_mesh, y_mesh = np.meshgrid(x_grid, y_grid, indexing='ij')
    mesh = np.transpose([x_mesh, y_mesh], [1, 2, 0])
    shape_grid = shape.shape_function(mesh)
    return mesh, shape_grid


def tile_basic(shape: Shape, mesh, x_spacing: float, y_spacing: float, x_count: int, y_count: int):
    compound_mesh = mesh
    compound_shape = shape.shape_function(mesh)
    for i in range(x_count):
        for j in range(y_count):
            current_mesh = mesh - [i * x_spacing, j * y_spacing]
            current_shape = shape.shape_function(current_mesh)
            compound_shape = np.logical_or(compound_shape, current_shape)
            compound_mesh = np.where(current_shape[:, :, None], current_mesh, compound_mesh)

    return compound_mesh, compound_shape


def generate_director_field(mesh, director_function, splay_function):
    director = director_function(mesh)
    splay = splay_function(mesh)
    return director, splay


def is_filling_method_valid(filling_method):
    if not isinstance(filling_method, str):
        return False
    return filling_method.lower() in ["splay", "perimeter", "dual"]


class Tiling:
    def __init__(self, tiling_type, side_length: float, x_count: int, y_count: int):
        """
        Tiling defines the shape and repetition
        :param tiling_type: 'triangular', 'square' or 'hexagonal'.
        :param side_length:
        :param x_count: number of tiles in the x-direction
        :param y_count: number of tiles in the y-direction.
        """
        if tiling_type in {"triangular", "square", "hexagonal"}:
            self.tiling_type = tiling_type
        else:
            raise RuntimeError(f"Unrecognised tiling type '{tiling_type}'.")
        self.side_length = side_length
        self.x_count = x_count
        self.y_count = y_count

    def generate_shape_matrix(self, pixel_size: float):
        if self.tiling_type == "triangular":
            x_spacing = self.side_length * np.sqrt(3) / 2
            y_spacing = self.side_length

            triangle_vertical_left = regular_triangle(self.side_length, orientation=0)
            triangle_vertical_right = regular_triangle(self.side_length, orientation=np.pi)

            mesh, _ = generate_shape_matrix(triangle_vertical_right, pixel_size,
                                            x_excess=x_spacing * (self.x_count - 1),
                                            y_excess=y_spacing * (self.y_count - 1) / 2)

            x_count_first = math.ceil(self.x_count / 2)
            x_count_second = math.floor(self.x_count / 2)

            y_count_first = math.ceil(self.y_count / 2)
            y_count_second = math.floor(self.y_count / 2)

            offset = [0, 0]
            mesh_one, shape_one = tile_basic(triangle_vertical_right, mesh,
                                             x_spacing * 2, y_spacing,
                                             x_count_first, y_count_first)

            offset = [-x_spacing / 3, 0.5 * y_spacing]
            mesh_two, shape_two = tile_basic(triangle_vertical_left, mesh - offset,
                                             x_spacing * 2, y_spacing,
                                             x_count_first, y_count_second)

            offset = [x_spacing * 2 / 3, 0]
            mesh_three, shape_three = tile_basic(triangle_vertical_left, mesh - offset,
                                                 x_spacing * 2, y_spacing,
                                                 x_count_second, y_count_first)

            offset = [x_spacing, 0.5 * y_spacing]
            mesh_four, shape_four = tile_basic(triangle_vertical_right, mesh - offset,
                                               x_spacing * 2, y_spacing,
                                               x_count_second, y_count_second)

            compound_shape = shape_one + shape_two + shape_three + shape_four
            compound_mesh = np.where(shape_two[:, :, None], mesh_two, mesh_one)
            compound_mesh = np.where(shape_three[:, :, None], mesh_three, compound_mesh)
            compound_mesh = np.where(shape_four[:, :, None], mesh_four, compound_mesh)

            return compound_mesh, compound_shape

        elif self.tiling_type == "square":
            triangle_vertical_left = square(self.side_length)

            mesh, _ = generate_shape_matrix(triangle_vertical_left, pixel_size,
                                            x_excess=self.side_length * (self.x_count - 1),
                                            y_excess=self.side_length * (self.y_count - 1))

            return tile_basic(triangle_vertical_left, mesh,
                              self.side_length, self.side_length,
                              self.x_count, self.y_count)

        elif self.tiling_type == "hexagonal":
            # Hexagon with its bottom side being horizontal
            triangle_vertical_left = regular_hexagon(self.side_length, orientation=- np.pi / 6)

            x_spacing = np.sqrt(3) * self.side_length
            y_spacing = 3 * self.side_length

            mesh, _ = generate_shape_matrix(triangle_vertical_left, pixel_size,
                                            x_excess=x_spacing * (self.x_count - 1 + 0.5),
                                            y_excess=y_spacing / 2 * (self.y_count - 1))

            mesh_one, shape_one = tile_basic(triangle_vertical_left, mesh,
                                             x_spacing, y_spacing,
                                             self.x_count, math.ceil(self.y_count / 2))
            tile_offset = [0.5 * x_spacing, 0.5 * y_spacing]
            mesh_two, shape_two = tile_basic(triangle_vertical_left, mesh - tile_offset,
                                             x_spacing, y_spacing,
                                             self.x_count, math.floor(self.y_count / 2))
            compound_shape = np.logical_or(shape_one, shape_two)
            compound_mesh = np.where(shape_two[:, :, None], mesh_two, mesh_one)

            return compound_mesh, compound_shape
        else:
            raise RuntimeError("Unknown tiling type.")


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

    def generateInputFiles(self, line_width_millimetre: float, pattern_name:str=None, line_width_pixel: int = 9,
                           filling_method=None, is_displayed=False, tiling: Tiling = None, is_splay_shown=False,
                           is_director_shown=True) -> str:
        """
        Generates theta, splay and config files for the pattern.
        :param line_width_millimetre: printing line width used for meshing.
        :param pattern_name: directory name, if is None, the self.name is used. If it is also None, the pattern is not saved for slicing.
        :param line_width_pixel: width of the line in pixels - higher values result in better slicing quality at the cost of runtime.
        :param filling_method: Splay, Perimeter or Dual.
        :param is_displayed: Is the pattern displayed after the generation.
        :param tiling: Tiling overwrites the default shape and tiles using
        :param is_splay_shown: Bool to change whether a quiver plot should be superimposed on the director.
        :return: pattern_name
        """
        if pattern_name is None:
            pattern_name = self.name

        stage_count = 4
        if pattern_name is not None: stage_count += 1

        begin_time = time.time()
        print(f"\n{time.time() - begin_time:.3f}s: [1/{stage_count:d}] Generating input files for {pattern_name}.")
        if tiling is None:
            mesh, shape_grid = generate_shape_matrix(self.domain, line_width_millimetre / line_width_pixel)
        else:
            mesh, shape_grid = tiling.generate_shape_matrix(line_width_millimetre / line_width_pixel)
            # Domain boundaries have to be updated for later plots
            self.domain.x_min = mesh[:, :, 0].min()
            self.domain.y_min = mesh[:, :, 1].min()
            self.domain.x_max = mesh[:, :, 0].max()
            self.domain.y_max = mesh[:, :, 1].max()

        print(f"{time.time() - begin_time:.3f}s: [2/{stage_count:d}] Meshing complete.")
        director_grid, splay_grid = generate_director_field(mesh, self.domain_director, self.domain_splay)
        print(f"{time.time() - begin_time:.3f}s: [3/{stage_count:d}] Director and splay calculation complete.")

        current_stage = 3
        if pattern_name is not None:
            pattern_directory = project_paths.get_patterns_directory() / pattern_name
            if not pattern_directory.exists():
                if not project_paths.get_patterns_directory().exists():
                    os.mkdir(project_paths.get_patterns_directory())
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
            config_file.write("InitialSeedingMethod " + filling_method.capitalize() + "\n")
            config_file.write("SeedSpacing " + str(line_width_pixel) + "\n")
            config_file.close()
            current_stage += 1
            print(f"{time.time() - begin_time:.3f}s: [{current_stage:d}/{stage_count:d}] Input files exported.")

        if is_displayed:
            director_filename = None
        else:
            director_filename = get_plot_output_directory() / f"{pattern_name}_design.png"

        plot_pattern(shape_grid, mesh, director_grid, self.domain, splay_grid, filename=director_filename,
                     is_splay_shown=is_splay_shown, is_director_shown=is_director_shown)
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
