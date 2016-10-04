# -*- mode: python -*-
# vi: set ft=python :

import os

def options(opt):
    opt.load('compiler_cxx')

def configure(conf):
    conf.load('compiler_cxx')
    dexpy_env_path = os.path.join(os.path.expanduser("~"), ".dex_dependencies/install/miniconda/envs/dexpy_env")
    conf.env["INCLUDES_DEXPY_ENV"] = os.path.join(dexpy_env_path, "include")
    conf.env["LIB_DEXPY_ENV"] = os.path.join(dexpy_env_path, "libs/python34")

def build(bld):
    bld.program(source="main.c", target="main", use="DEXPY_ENV")
