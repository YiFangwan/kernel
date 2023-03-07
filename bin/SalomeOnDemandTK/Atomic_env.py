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

    #[ATOMIC]
    atomic_root_dir = os.path.join(root_dir, 'ATOMIC')
    context.setVariable('ATOMIC_ROOT_DIR', atomic_root_dir, overwrite=True)
    context.addToPath(os.path.join(atomic_root_dir, 'bin', 'salome'))
    context.addToLdLibraryPath(os.path.join(atomic_root_dir, 'lib', 'salome'))
    context.addToVariable('SALOME_MODULES', 'ATOMIC', separator=',')

    atomic_res_dir = os.path.join(atomic_root_dir, 'share', 'salome', 'resources', 'atomic')
    context.addToVariable('SalomeAppConfig', atomic_res_dir, separator=os.pathsep)
