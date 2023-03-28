
#!/usr/bin/env python3
# -*- coding:utf-8 -*-

"""
SALOME application environment.
"""

import os
import sys
from SalomeOnDemandTK.extension_query import ext_info_dict


def init(context, ext_root_dir):
    """
    Set an environment for SALOME app.

    Args:
        context - an SalomeContext object.
        ext_root_dir - a path to __SALOME_EXT__ directory.

    Returns:
        None.
    """

    python_version = '{}.{}'.format(sys.version_info[0], sys.version_info[1])
    python_libdir = os.path.join('lib', 'python' + python_version, 'site-packages')

    reinitialise_paths = True

    #[SALOME_COMMON]

    out_dir_Path=os.path.dirname(os.path.realpath(__file__))

    # Seting SALOME modules environment
    modules = ext_info_dict(out_dir_Path)
    for module in modules:
        print
        for component in modules[module][2].split(', '):
            context.setVariable('%s_ROOT_DIR'%component, ext_root_dir, overwrite=True)
    context.addToPath(os.path.join(ext_root_dir, 'bin', 'salome'))
    context.addToLdLibraryPath(os.path.join(ext_root_dir, 'lib', 'salome'))
    context.addToPythonPath(os.path.join(ext_root_dir, 'bin', 'salome'))
    context.addToPythonPath(os.path.join(ext_root_dir, 'lib', 'salome'))
    context.addToPythonPath(os.path.join(ext_root_dir, python_libdir, 'salome'))

    # Seting SALOME prerequis and tools environment
    context.addToPath(os.path.join(ext_root_dir, 'bin'))
    context.addToLdLibraryPath(os.path.join(ext_root_dir, 'lib'))
    context.addToPythonPath(os.path.join(ext_root_dir, 'bin'))
    context.addToPythonPath(os.path.join(ext_root_dir, 'lib'))
    context.addToPythonPath(os.path.join(ext_root_dir, python_libdir))

    res_dir = os.path.join(ext_root_dir, 'share', 'salome', 'resources', 'gui')
    if reinitialise_paths:
        context.setVariable('SalomeAppConfig', res_dir, overwrite=True)
    else:
        context.addToVariable('SalomeAppConfig', res_dir, separator=os.pathsep)
