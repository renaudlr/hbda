# Hypervolume Box Decomposition Algorithm

This is the source code for two algorithms to compute the _hypervolume indicator_, a widely used measure to compare approximations of the nondominated set in multiobjective optimization.

The incremental algorithm allows online comptation of the hypervolume (with arbitrary insertions).

The nonincremental algorithms assumes the whole set of points to compute the hypervolume for to be given in advance. No further insertions are possible.

## Compiling

You will need `cmake` and a C compiler to build the project.

Then run from the root directory of the project:

    mkdir build
    cd build
    cmake ..
    make

This will build the project in the `build` subdirectory.

## Usage

After building, two executables are created, `incremental` and `nonincremental`. Both executables include a short online help (with `-h` or no argument).

The input should be a plain text file containing floating point values. Each line starting with a digit is considered as a point and should contain its component values separated by single spaces from the beginning of the line. Other lines are ignored.

## Reference paper

*In case you use this work in your research, please cite the following paper:*

Renaud Lacour, Kathrin Klamroth, Carlos M. Fonseca, *A Box Decomposition Algorithm to Compute the Hypervolume Indicator*, Computers & Operations Research, Available online 3 July 2016, ISSN 0305-0548, http://dx.doi.org/10.1016/j.cor.2016.06.021

The corresponding working paper is available online on arXiv: https://arxiv.org/abs/1510.01963
