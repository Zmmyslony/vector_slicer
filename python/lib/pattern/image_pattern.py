from PIL import Image, ImageOps
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import RegularGridInterpolator

from lib.slicer import Slicer
from lib.pattern.pattern import Pattern
from lib.director.director import Director
from lib.shape.basic import rectangle
from lib.output_reading import plot_pattern


def openImage(file_path):
    image = Image.open(file_path)
    image_gs = ImageOps.grayscale(image)
    image_np = np.array(image_gs, dtype=float) / 255.
    return image_np


def convertIntensityToDirector(intensity_array):
    director_array = 1 / 2 * np.arcsin(np.sqrt(intensity_array))
    return director_array


def getInterpolation(director_array, bounds):
    x = np.linspace(0, bounds[0], director_array.shape[0], endpoint=True)
    y = np.linspace(0, bounds[1], director_array.shape[1], endpoint=True)
    return RegularGridInterpolator((x, y), director_array, bounds_error=False, fill_value=0)


class ImagePattern(Pattern):
    """
    This class generates a pattern with rectangular shape, corresponding to the aspect ratio of the input image,
    with the grayscale based director such that I = Sin(2 * theta) ** 2.
    """

    def __init__(self, filename: str | Path, long_dimension: float):
        intensity_array = openImage(filename)
        director_array = convertIntensityToDirector(intensity_array)
        dimensions = long_dimension * np.array(intensity_array.shape) / max(intensity_array.shape)
        shape = rectangle(0, 0, dimensions[0], dimensions[1])
        director = Director(getInterpolation(director_array, dimensions))

        super().__init__(shape, director)


if __name__ == "__main__":
    filename = Path("").absolute()  / "input" / "ucam-logo-colour-preferred.jpg"
    pattern = ImagePattern(filename, 40)
    pattern.generateInputFiles(0.2, "cambridge_logo", is_displayed=True)

    # slicer = Slicer()
    # slicer.slice("cambridge_logo")
    plot_pattern("cambridge_logo", is_non_printing_moves_shown=False, is_fill_density_shown=False,
                 is_axes_shown=False)
