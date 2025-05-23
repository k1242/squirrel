from pathlib import Path
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

ext_modules = [
    Pybind11Extension(
        "squirrel",
        [
            "bindings/squirrel_py.cpp",
            "engine/Board.cpp",
            "engine/Move.cpp",
            "engine/notation.cpp",
            "engine/movegen.cpp",
        ],
        include_dirs=["engine"],
        cxx_std=20,
        extra_compile_args=["/EHsc"] # ["-std=c++20", "-Ofast"]
    ),
]

setup(
    name="squirrel",
    version="0.1.0",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
)
