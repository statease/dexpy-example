
# Dependencies

To run the example script you'll need to place a python environment alongside the executable.

## Windows

You'll need `python34.dll`, the python 3.4 stdlib zipped up as `python34.zip`.

Create a folder named `dexpy_env` and copy the following modules into it (from a conda env, most likely):

 * dateutil
 * dexpy
 * matplotlib
 * numpy
 * pandas
 * pytz
 * scipy
 * statsmodels

You'll also need the following DLLs in the path (alongside the exe is fine):

 * libpng16.dll (used by _png.pyd, which is from matplotlib)
 * zlib.dll (used by _png.pyd, which is from matplotlib)
 * mkl_intel_thread.dll (used by multiarray.pyd, which is from numpy/core)
 * libifcoremd.dll, libmmbd.dll (used by _ufuncs.pyd, which is from scipy/special)
 * mkl_def.dll, libiomp5md.dll (used by the MKL)

Finally, edit `matplotlib/mpl-data/matplotlibrc` to set the backend (qt requires qt, gtk requires pygtk, etc)
