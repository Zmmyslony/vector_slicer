"""
Reading and plotting the slicer output.
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

from . import slicer_setup as slicer
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import os.path
import math
from mpl_toolkits.axes_grid1 import make_axes_locatable


def get_slicer_output_directory():
    slicer_directory = slicer.get_project_directory()
    output_directory = slicer_directory / "output"
    return output_directory


def get_plot_output_directory():
    plot_output_directory = slicer.get_project_directory() / "python" / "output"
    if not os.path.exists(plot_output_directory):
        os.mkdir(plot_output_directory)
    return plot_output_directory


def read_fill_matrix(pattern_name):
    input_path = get_slicer_output_directory() / "filled_matrices" / (pattern_name + ".csv")
    data = np.genfromtxt(input_path, dtype=int, delimiter=",").transpose()
    return data


def plot_fill_matrix(axis: plt.axis, data: np.ndarray, max_fill_value=None, is_axes_shown=False):
    if max_fill_value is None:
        max_value = max(3, np.max(data))
    else:
        max_value = max_fill_value

    color_values = np.linspace(1, 0, max_value + 1)
    color_list = np.char.mod('%f', color_values)

    discrete_colormap = mpl.colors.ListedColormap(color_list)
    colormap_bounds = mpl.colors.BoundaryNorm(np.arange(-0.5, max_value + 1), discrete_colormap.N)

    pcm = axis.matshow(data, cmap=discrete_colormap, norm=colormap_bounds, origin="lower")

    # plt.colorbar(pcm, ticks=np.arange(max_value + 1), label="number of times filled", cax=cax)
    if is_axes_shown:
        divider = make_axes_locatable(axis)
        cax = divider.append_axes("right", size="5%", pad=0.05)
        plt.colorbar(pcm, ticks=np.arange(max_value + 1), cax=cax)
    return axis


def obtain_lowest_reached_value(data):
    min_value_array = np.zeros_like(data)
    for index in range(len(data)):
        min_value_array[index] = np.min(data[0:index + 1])
    return min_value_array


def read_optimisation_sequence(pattern_name) -> list:
    """
    :param pattern_name:
    :return: List of disagreements in each optimisation iteration.
    """
    input_path = get_slicer_output_directory() / "optimisation_save" / (pattern_name + ".txt")

    file = open(input_path, "r")
    optimisation_sequence = []
    for line in file:
        if line[:3] == "mY=":
            starting_index = line.find("(")
            finishing_index = line.find(")")
            string_sequence = line[starting_index + 1: finishing_index]
            optimisation_sequence = np.fromstring(string_sequence, dtype=float, sep=',')
    return optimisation_sequence


def plot_optimisation_sequence(fig: plt.figure, optimisation_sequence):
    min_value_sequence = obtain_lowest_reached_value(optimisation_sequence)
    ax = fig.gca()
    ax.plot(optimisation_sequence, 'o', label="iteration disagreement")
    ax.plot(min_value_sequence, label="minimal disagreement")

    min_value = np.min(optimisation_sequence)
    max_value = np.max(optimisation_sequence)
    if max_value / min_value > 10:
        ax.set_yscale("log")
    return fig


def convert_to_coordinates(string_data):
    data = np.fromstring(string_data, dtype=float, sep=',')
    coordinate_data = np.reshape(data, [int(len(data) / 2), 2])
    return coordinate_data


def read_paths(pattern_name):
    """
    Reads paths in format where the first level corresponds to paths, second level to coordinates within a path
    and the first one to specific pixel-based x-y coordinates.
    :param pattern_name:
    :return:
    """
    input_path = get_slicer_output_directory() / "paths" / (pattern_name + ".csv")

    file = open(input_path, "r")
    list_of_lines = []
    is_start_found = False
    for line in file:
        if "# Start of pattern" in line:
            is_start_found = True
        elif "# End of pattern" in line:
            break
        elif is_start_found:
            list_of_lines.append(convert_to_coordinates(line))
    return list_of_lines


def read_seeds(pattern_name):
    input_path = get_slicer_output_directory() / "used_seeds" / (pattern_name + ".csv")
    seeds = np.genfromtxt(input_path, delimiter=",")
    return seeds


def plot_paths(axis: plt.axis, list_of_lines, is_non_printing_moves_shown=True):
    if len(list_of_lines) == 0:
        raise RuntimeError("There are no paths to plot.")
    for path in list_of_lines:
        path = np.array(path)
        axis.plot(path[:, 0], path[:, 1], color="C0", linewidth=0.5)

    if len(list_of_lines) > 1 and is_non_printing_moves_shown:
        for i in range(1, len(list_of_lines)):
            connecting_path = np.array([list_of_lines[i - 1][-1], list_of_lines[i][0]])
            axis.plot(connecting_path[:, 0], connecting_path[:, 1], color="C1", linewidth=0.5)

    axis.axis("equal")

    return axis


def plot_pattern(pattern_name, axis: plt.axis = None, is_fill_density_shown=True, is_paths_shown=True,
                 is_axes_shown=True, max_fill_value=None, is_non_printing_moves_shown=True, is_seeds_shown=False):
    """
    Plots the sliced pattern with fill density (gray), printing moves (blue) and non-printing moves (orange).
    :param pattern_name:
    :param axis: If specified the plots will not be immediately shown but returned for further manipulation.
    :param is_fill_density_shown: allows to choose whether gray background showing the fill density is shown.
    :param is_paths_shown: allows to choose printer movements are shown.
    :param is_non_printing_moves_shown: allows to choose whether non-printing moves between paths are shown.
    :param is_axes_shown:
    :param max_fill_value: maximum value of the fill matrix.
    :return:
    """

    if axis is None:
        fig = plt.figure(figsize=[6, 4], dpi=300)
        plotting_axis = fig.gca()
    else:
        plotting_axis = axis
    if is_fill_density_shown:
        fill_matrix = read_fill_matrix(pattern_name)
        plotting_axis = plot_fill_matrix(plotting_axis, fill_matrix, max_fill_value=max_fill_value,
                                         is_axes_shown=is_axes_shown)
    if is_paths_shown:
        paths = read_paths(pattern_name)
        plotting_axis = plot_paths(plotting_axis, paths, is_non_printing_moves_shown=is_non_printing_moves_shown)

    if is_seeds_shown:
        seeds = read_seeds(pattern_name)
        plotting_axis.scatter(seeds[..., 0], seeds[..., 1], color="red", s=0.1)

    if is_axes_shown:
        plotting_axis.set_title(pattern_name)
        plotting_axis.set_xlabel("x [pixel]")
        plotting_axis.set_ylabel("y [pixel]")
        plotting_axis.set_aspect('equal')
    else:
        plotting_axis.axis('off')
        plotting_axis.get_xaxis().set_visible(False)
        plotting_axis.get_yaxis().set_visible(False)
        plt.tight_layout()

    if axis is None:
        save_name = get_plot_output_directory() / f"{pattern_name}_pattern.svg"
        plt.savefig(save_name, transparent=True)
        plt.show()
    return axis


def plot_director_distribution_disagreement(pattern_name, bucket_count=None):
    input_path = get_slicer_output_directory() / "bucketed_disagreement" / (pattern_name + ".csv")
    original_data = np.genfromtxt(input_path, delimiter=",")
    original_data /= np.sum(original_data) # Normalize
    original_data *= 100 # Percentages
    original_angle_separators = np.linspace(0, 90, original_data.shape[0], endpoint=True)
    if bucket_count is not None:
        interval = math.ceil(original_data.shape[0] / bucket_count)
    else:
        interval = 1
    data = original_data[::interval]
    for i in range(1, interval):
        data += original_data[i::interval]
    angle_separators = original_angle_separators[::interval]

    fig = plt.figure(figsize=[6, 4], dpi=300)
    plt.bar(angle_separators[:-1], data[:-1], width=interval * 0.8)
    plt.xlabel("angle disagreement [°]")
    plt.ylabel("density [%]")
    plt.show()


def plot_disagreement_progress(pattern_name):
    """
    Plots how the disagreement developed throughout optimisation, with the solid line showing minimal disagreement.
    :param pattern_name:
    :return:
    """
    optimisation_sequence = read_optimisation_sequence(pattern_name)

    fig = plt.figure(figsize=[6, 4], dpi=300)
    plot_optimisation_sequence(fig, optimisation_sequence)

    ax = fig.gca()
    ax.set_title(pattern_name)
    ax.set_xlabel("iteration")
    ax.set_ylabel("disagreement")
    ax.legend()
    save_name = get_plot_output_directory() / f"{pattern_name}_disagreement_progress.svg"
    plt.savefig(save_name)

    plt.show()
