# -*- mode: python -*-
# vi: set ft=python :

import os

def options(opt):
    opt.load('compiler_cxx')

def configure(conf):
    conf.load('compiler_cxx')

    dependency_path = os.path.join(os.path.expanduser("~"), ".dex_dependencies/install")
    dexpy_env_path = os.path.join(dependency_path, "miniconda/envs/dexpy_env")

    conf.env["INCLUDES_DEXPY_ENV"] = os.path.join(dexpy_env_path, "include")
    conf.env["LIB_DEXPY_ENV"] = os.path.join(dexpy_env_path, "libs/python34")

    conf.env["INCLUDES_PYBIND11"] = os.path.join(dependency_path, "pybind11/include")

def build(bld):
    bld.program(source="main.cpp", target="main", use=["DEXPY_ENV", "PYBIND11"])
