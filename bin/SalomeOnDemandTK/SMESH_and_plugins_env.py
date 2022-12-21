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

    #[SMESH]
    smesh_root_dir = os.path.join(root_dir, 'SMESH')
    context.setVariable('SMESH_ROOT_DIR', smesh_root_dir, overwrite=True)
    context.addToPath(os.path.join(smesh_root_dir, 'bin', 'salome'))
    context.addToLdLibraryPath(os.path.join(smesh_root_dir, 'lib', 'salome'))
    context.addToPythonPath(os.path.join(smesh_root_dir, 'bin', 'salome'))
    context.addToPythonPath(os.path.join(smesh_root_dir, 'lib', 'salome'))
    context.addToPythonPath(os.path.join(smesh_root_dir, python_libdir, 'salome'))
    if reinitialise_paths:
        context.setVariable('SMESH_MeshersList', 'StdMeshers', overwrite=True)
    else:
        context.addToVariable('SMESH_MeshersList', 'StdMeshers', separator=':')

    smesh_res_dir = os.path.join(smesh_root_dir, 'share', 'salome', 'resources', 'smesh')
    context.addToPath('SalomeAppConfig', smesh_res_dir, separator=':')

    #[gmsh]
    gmsh_root_dir = os.path.join(root_dir, 'gmsh')
    context.setVariable('GMSHHOME', gmsh_root_dir, overwrite=True)
    context.setVariable('GMSH_ROOT_DIR', gmsh_root_dir, overwrite=True)
    context.addToLdLibraryPath(os.path.join(gmsh_root_dir, 'lib'))

    #[NETGENPLUGIN]
    netgenplugin_root_dir = os.path.join(root_dir, 'NETGENPLUGIN')
    context.setVariable('NETGENPLUGIN_ROOT_DIR', netgenplugin_root_dir, overwrite=True)
    context.addToPath(os.path.join(netgenplugin_root_dir, 'bin', 'salome'))
    context.addToLdLibraryPath(os.path.join(netgenplugin_root_dir, 'lib', 'salome'))
    context.addToPythonPath(os.path.join(netgenplugin_root_dir, 'bin', 'salome'))
    context.addToPythonPath(os.path.join(netgenplugin_root_dir, 'lib', 'salome'))
    context.addToPythonPath(os.path.join(netgenplugin_root_dir, python_libdir, 'salome'))
    context.addToVariable('SMESH_MeshersList', 'NETGENPlugin', separator=':')

    netgenplugin_res_dir = os.path.join(netgenplugin_root_dir,
        'share', 'salome', 'resources', 'netgenplugin')
    context.addToPath('SalomeAppConfig', netgenplugin_res_dir, separator=':')
