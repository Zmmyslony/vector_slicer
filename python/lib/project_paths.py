import os
import pathlib

def get_project_directory():
    """
    :return: Vector Slicer directory
    """
    return pathlib.Path().absolute().parent


def get_patterns_directory():
    """
    :return: Patterns (input) directory.
    """
    return get_project_directory() / "patterns"

def get_slicer_output_directory():
    slicer_directory = get_project_directory()
    output_directory = slicer_directory / "output"
    return output_directory


def get_plot_output_directory():
    plot_output_directory = get_project_directory() / "python" / "output"
    if not os.path.exists(plot_output_directory):
        os.mkdir(plot_output_directory)
    return plot_output_directory