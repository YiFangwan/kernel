#!/usr/bin/env python3
# -*- coding:utf-8 -*-

"""
Add a custom environment to the SALOME extension.
"""

import os


def init(context, root_dir):
    """
    Set an environment for an extension on start of SALOME app.

    Args:
        context - an SalomeContext object.
        root_dir - a path to __SALOME_EXT__ directory.

    Returns:
        None.
    """

    python_version = '3.10'
    python_libdir = os.path.join('lib', 'python' + python_version, 'site-packages')

    reinitialise_paths = True

    #[KERNEL]
    kernel_root_dir = os.path.join(root_dir, 'KERNEL')
    context.setVariable('KERNEL_ROOT_DIR', kernel_root_dir, overwrite=True)
    context.addToPath(os.path.join(kernel_root_dir, 'bin', 'salome'))
    context.addToLdLibraryPath(os.path.join(kernel_root_dir, 'lib', 'salome'))
    context.addToPythonPath(os.path.join(kernel_root_dir, 'bin', 'salome'))
    context.addToPythonPath(os.path.join(kernel_root_dir, 'lib', 'salome'))
    context.addToPythonPath(os.path.join(kernel_root_dir, python_libdir, 'salome'))

    #[GUI]
    gui_root_dir = os.path.join(root_dir, 'GUI')
    context.setVariable('GUI_ROOT_DIR', gui_root_dir, overwrite=True)
    context.addToPath(os.path.join(gui_root_dir, 'bin', 'salome'))
    context.addToLdLibraryPath(os.path.join(gui_root_dir, 'lib', 'salome'))
    context.addToPythonPath(os.path.join(gui_root_dir, 'bin', 'salome'))
    context.addToPythonPath(os.path.join(gui_root_dir, 'lib', 'salome'))
    context.addToPythonPath(os.path.join(gui_root_dir, python_libdir, 'salome'))

    gui_res_dir = os.path.join(gui_root_dir, 'share', 'salome', 'resources', 'gui')
    if reinitialise_paths:
        context.setVariable('SalomeAppConfig', gui_res_dir, overwrite=True)
    else:
        context.addToVariable('SalomeAppConfig', gui_res_dir, separator=':')
