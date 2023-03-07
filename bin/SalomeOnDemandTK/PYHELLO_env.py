#!/usr/bin/env python3
# -*- coding:utf-8 -*-

"""
Add a custom environment to the SALOME extension.
"""

import os
import sys


def init(context, root_dir):
    """
    Set an environment for an extension on start of SALOME app.

    Args:
        context - an SalomeContext object.
        root_dir - a path to __SALOME_EXT__ directory.

    Returns:
        None.
    """

    python_version = '{}.{}'.format(sys.version_info[0], sys.version_info[1]) 
    python_libdir = os.path.join('lib', 'python' + python_version, 'site-packages')

    #[PYHELLO]
    pyhello_root_dir = os.path.join(root_dir, 'PYHELLO')
    context.setVariable('PYHELLO_ROOT_DIR', pyhello_root_dir, overwrite=True)
    context.addToPath(os.path.join(pyhello_root_dir, 'bin', 'salome'))
    context.addToLdLibraryPath(os.path.join(pyhello_root_dir, 'lib', 'salome'))
    context.addToPythonPath(os.path.join(pyhello_root_dir, 'bin', 'salome'))
    context.addToPythonPath(os.path.join(pyhello_root_dir, 'lib', 'salome'))
    context.addToPythonPath(os.path.join(pyhello_root_dir, python_libdir, 'salome'))
    context.addToVariable('SALOME_MODULES', 'PYHELLO', separator=',')

    pyhello_res_dir = os.path.join(pyhello_root_dir, 'share', 'salome', 'resources', 'pyhello')
    context.addToVariable('SalomeAppConfig', pyhello_res_dir, separator=os.pathsep)
