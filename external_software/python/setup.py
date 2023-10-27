import ctypes
import pathlib
import sys


def get_project_directory():
    return pathlib.Path().absolute().parent.parent


def configure_slicer(slicer):
    slicer.slice_pattern.argtypes = [ctypes.c_char_p]
    slicer.slice_pattern_with_config.argtypes = [ctypes.c_char_p, ctypes.c_char_p]


def import_slicer(build_directory):
    project_directory = get_project_directory()

    if sys.platform == "win32":
        library_name = "vector_slicer_api.dll"
    else:
        library_name = "vector_slicer_api.so"
    vector_slicer_lib_path = project_directory / build_directory / library_name
    slicer = ctypes.CDLL(str(vector_slicer_lib_path))

    configure_slicer(slicer)
    return slicer
