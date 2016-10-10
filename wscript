# -*- mode: python -*-
# vi: set ft=python :

import os
import platform

def options(opt):
    opt.load('compiler_cxx python')

def configure(conf):

    # initialize the debug/release environments
    # all conf.env calls from here on out will be stored in a sepcific variant
    #
    # to store something in release, do:
    #    conf.setenv("release")
    #    conf.env[foo] = "bar"
    variants = _get_variants()
    for v in variants:
        # create a copy of the default environment
        new_env = conf.env.derive()
        # and detach so we aren't referencing the same lists
        new_env.detach()
        conf.setenv(v, env=new_env)

        conf.load('compiler_cxx python')
        conf.check_python_version()

        dependency_path = os.path.join(os.path.expanduser("~"), ".dex_dependencies/install")
        dexpy_env_path = os.path.join(dependency_path, "miniconda/envs/dexpy_env")

        conf.env["INCLUDES_DEXPY_ENV"] = os.path.join(dexpy_env_path, "include")
        conf.env["LIB_DEXPY_ENV"] = os.path.join(dexpy_env_path, "libs/python34")

        conf.env["INCLUDES_PYBIND11"] = os.path.join(dependency_path, "pybind11/include")

        # these are the libs used by dex11, definitely not all needed here but I don't want to bother sorting them out
        conf.env["INCLUDES_WXWIDGETS"] = [os.path.join(dependency_path, "wxWidgets/include") , os.path.join(dependency_path, "wxWidgets/lib/vc_x64_lib/mswu")]
        conf.env["LIB_WXWIDGETS"] = [ "wxmsw31u_aui", "wxmsw31u_adv", "wxbase31u", "wxmsw31u_core", "wxmsw31u_html", "wxmsw31u_propgrid", "wxmsw31u_stc", "wxmsw31u_webview", "wxbase31u_xml", "wxmsw31u_xrc", "wxscintilla", "wxpng", "wxtiff", "wxjpeg", "wxzlib", "wxregexu", "wxexpat"]
        conf.env["LIBPATH_WXWIDGETS"] = os.path.join(dependency_path, "wxWidgets/lib/vc_x64_lib")

        conf.env['DEXPY_SYSTEM_LIBS'] = [ "gdi32", "winspool", "shell32", "ole32", "oleaut32", "comdlg32", "advapi32", "wininet", "winhttp", "user32", "ws2_32", "winmm", "comctl32", "rpcrt4", "wsock32" ]

        conf.check_libs_msvc(conf.env['DEXPY_SYSTEM_LIBS'], mandatory=True)

        _set_cxx_flags(conf)

def build(bld):

    # this compiles a py file to pyc
    # bld.shlib(source="examples/filtrate.py", features="py")

    dexpy_sources = [
        "dexpy_example.cpp",
        "examples/filtrate.cpp",
        "dexpy_grid.cpp",
    ]

    dexpy_libs = ["DEXPY_ENV", "PYBIND11", "WXWIDGETS"]
    dexpy_libs.extend([x.upper() for x in bld.env['DEXPY_SYSTEM_LIBS']])
    bld.program(source=dexpy_sources, target="dexpy_example", use=dexpy_libs)

def _set_cxx_flags(ctx):

    # set compiler/link flags for the variants
    if platform.system() == 'Windows':

        # common to all variants
        ctx.env.CXXFLAGS = [
            "/EHsc", # enable c++ exceptions
            "/W4", # warning level 4 (/Wall is not advised with MSVC)
            "/WX", # treat warnings as errors
            "/wd4127", # suppress C4127: conditional expression is constant
            "/wd4589", # suppress C4589: ctor ignores virtual base initializer
            "/wd4512", # suppress C4512: Assignment operator not generated
            "/GR", # enable run time type info
            "/nologo", # enable run time type info
        ]

        ctx.env.LINKFLAGS = [
            "/nologo",
            "/manifest",
        ]

        # set the minimum version (5.01 is XP, 5.02 is XP64)
        ctx.env.LINKFLAGS.append("/SUBSYSTEM:WINDOWS,5.02")

        if "debug" in ctx.variant:

            # add MSVC debug-only compiler flags here
            ctx.env.CXXFLAGS += [
                "/Zi", # generate debugging info (pdb files)
                "/Od", # disable optimization
                "/Ob0", # disable inlining
                "/RTC1", # runtime checks
            ]

            if "dynamic_runtime" in ctx.variant:
                ctx.env.CXXFLAGS.append("/MDd") # multithreaded debug dll runtime
            else:
                ctx.env.CXXFLAGS.append("/MTd") # multithreaded debug static runtime

            # add debug-only preprocessor definitions to this list
            ctx.env.CXXFLAGS += [
                "/D_DEBUG",
                "/D_SCL_SECURE_NO_WARNINGS",
                "/D_CRT_SECURE_NO_WARNINGS",
            ]

            if ctx.env.MSVC_VERSION > 11:
                ctx.env.CXXFLAGS.append("/FS") # synchronize writes to pdb files (or else you'll get C1041 errors when building on multiple threads)

            ctx.env.LINKFLAGS += [
                "/debug",
            ]

        else: # release

            # add MSVC release-only compiler flags here
            ctx.env.CXXFLAGS += [
                "/O2", # max speed
                "/GF", # string pooling
                "/Gm-", # no minimal rebuild
                "/GS", # security check
                "/Gy", # function-level linking
                "/fp:precise", # precise fp
                "/openmp",
                "/Gd", # calling convention
            ]
            if "dynamic_runtime" in ctx.variant:
                ctx.env.CXXFLAGS.append("/MD") # multithreaded dll runtime
            else:
                ctx.env.CXXFLAGS.append("/MT") # multithreaded static runtime

            # add release-only preprocessor definitions to this list
            ctx.env.CXXFLAGS += [
                "/DNDEBUG",
            ]

        # this is the XP compatable SDK, required if targeting XP (/SUBSYSTEM:WINDOWS,5.01 or 5.02)
        ctx.env.LIBPATH.insert(0, "{}\\Microsoft SDKs\\Windows\\7.1A\\Lib\\x64".format(os.environ["ProgramFiles(x86)"]))
        ctx.env.INCLUDES.insert(0, "{}\\Microsoft SDKs\\Windows\\7.1A\\Include".format(os.environ["ProgramFiles(x86)"]))
        ctx.env.CXXFLAGS.append("/D_USING_V110_SDK71_")

    else:

        # gcc/linux

        ctx.env.CXXFLAGS = [ "-Wall", "-Wno-write-strings", "-Wno-parentheses", "-Werror", "-std=c++11", "-fno-strict-aliasing", "-msse2", "-fPIC", "-fopenmp", "-DBOOST_SYSTEM_NO_DEPRECATED" ]
        if ctx.env.CXX_NAME == "clang":
            ctx.env.CXXFLAGS.append("-stdlib=libc++")
        else:
            ctx.env.LINKFLAGS = [ "-pthread" ]
        ctx.env.LINKFLAGS.append("-fopenmp")

        if "debug" in ctx.variant:
            ctx.env.CXXFLAGS += [
                "-g", # turn on debugging info
                "-O0", # turn off optimization
            ]
        else: # release
            ctx.env.CXXFLAGS += [ "-O3" ]

def init(ctx):
    """Parses the command to see if a build_xxx variant was passed (e.g. build_release, build_debug)

    from: https://github.com/waf-project/waf/blob/master/demos/variants/wscript"""
    from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext
    from waflib.Context import Context

    for x in _get_variants():
        for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
            name = y.__name__.replace('Context','').lower()
            class tmp(y):
                cmd = name + '_' + x
                variant = x

    test_variants = _get_variants()

    ## if you work on "debug" 99% of the time, here is how to re-enable "waf build":
    for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
       class tmp(y):
           variant = 'release'

def _get_variants():
    return [ "release", "debug" ]
