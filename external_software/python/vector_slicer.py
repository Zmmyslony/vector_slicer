import ctypes


import setup

if __name__ == "__main__":
    slicer = setup.import_slicer("cmake-build-release")

    patterns_directory = setup.get_project_directory() / "patterns"
    pattern_name = patterns_directory / "example_azimuthal_10_mm"
    pattern_name_b = str(pattern_name).encode('utf-8')

    slicer.slice_pattern(pattern_name_b)
