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
from mpl_toolkits.axes_grid1 import make_axes_locatable


def get_output_directory():
    slicer_directory = slicer.get_project_directory()
    output_directory = slicer_directory / "output"
    return output_directory


def read_fill_matrix(pattern_name):
    input_path = get_output_directory() / "filled_matrices" / pattern_name
    input_path = input_path.with_suffix(".csv")
    data = np.genfromtxt(input_path, dtype=int, delimiter=",").transpose()
    return data


def plot_fill_matrix(axis: plt.axis, data: np.ndarray):
    max_value = max(3, np.max(data))

    color_values = np.linspace(1, 0, max_value + 1)
    color_list = np.char.mod('%f', color_values)

    discrete_colormap = mpl.colors.ListedColormap(color_list)
    colormap_bounds = mpl.colors.BoundaryNorm(np.arange(-0.5, max_value + 1), discrete_colormap.N)

    pcm = axis.matshow(data, cmap=discrete_colormap, norm=colormap_bounds, origin="lower")
    divider = make_axes_locatable(axis)
    cax = divider.append_axes("right", size="5%", pad=0.05)
    plt.colorbar(pcm, ticks=np.arange(max_value + 1), label="number of times filled", cax=cax)
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
    input_path = get_output_directory() / "optimisation_save" / pattern_name
    input_path = input_path.with_suffix(".txt")

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
    input_path = get_output_directory() / "paths" / pattern_name
    input_path = input_path.with_suffix(".csv")

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


def plot_paths(axis: plt.axis, list_of_lines):
    for path in list_of_lines:
        path = np.array(path)
        axis.plot(path[:, 0], path[:, 1], color="C0", linewidth=0.5)

    if len(list_of_lines) > 1:
        for i in range(1, len(list_of_lines)):
            connecting_path = np.array([list_of_lines[i - 1][-1], list_of_lines[i][0]])
            axis.plot(connecting_path[:, 0], connecting_path[:, 1], color="C1", linewidth=0.5)

    return axis


def plot_pattern(pattern_name, axis: plt.axis = None, is_fill_density_shown=True, is_paths_shown=True):
    """
    Plots the sliced pattern with fill density (gray), printing moves (blue) and non-printing moves (orange).
    :param pattern_name:
    :param axis: If specified the plots will not be immediately shown but returned for further manipulation.
    :param is_fill_density_shown: allows to choose whether gray background showing the fill density is shown.
    :param is_paths_shown: allows to choose printer movements are shown.
    :return:
    """
    fill_matrix = read_fill_matrix(pattern_name)
    paths = read_paths(pattern_name)

    if axis is None:
        fig = plt.figure(figsize=[6, 4], dpi=300)
        plotting_axis = fig.gca()
    else:
        plotting_axis = axis
    if is_fill_density_shown:
        plotting_axis = plot_fill_matrix(plotting_axis, fill_matrix)
    if is_paths_shown:
        plotting_axis = plot_paths(plotting_axis, paths)

    plotting_axis.set_title(pattern_name)
    plotting_axis.set_xlabel("x [pixel]")
    plotting_axis.set_ylabel("y [pixel]")
    plotting_axis.set_aspect('equal')
    if axis is None:
        plt.show()
    return axis


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
    plt.show()
