
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

    BINDIR = os.path.join(ext_root_dir, 'bin')
    LIBDIR = os.path.join(ext_root_dir, 'lib')
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
    # Seting SALOME prerequis and tools environment
    context.addToPath(BINDIR)
    context.addToLdLibraryPath(LIBDIR)
    context.addToPythonPath(BINDIR)
    context.addToPythonPath(LIBDIR)
    context.addToPythonPath(os.path.join(ext_root_dir, python_libdir))
    context.addToPythonPath(os.path.join(ext_root_dir, 'lib', 'python' + python_version))

    #[Python]
    context.setVariable(r"PYTHON_VERSION", python_version, overwrite=True)
    context.addToPath(ext_root_dir)
    context.setVariable(r"PYTHONBIN", os.path.join(BINDIR,'python3'), overwrite=True)
    
    #[gcc]
    context.addToLdLibraryPath(os.path.join(ext_root_dir, 'lib64'))

    #[openmpi]
    context.setVariable(r"MPIEXEC_EXECUTABLE", os.path.join(BINDIR, 'mpiexec'), overwrite=True)
    context.addToPath(os.path.join(ext_root_dir,'include'))

    #[qt]
    context.setVariable(r"QTDIR", ext_root_dir, overwrite=True)
    context.setVariable(r"QT5_ROOT_DIR", ext_root_dir, overwrite=True)
    context.setVariable(r"QT_PLUGIN_PATH", os.path.join(ext_root_dir,'plugins'), overwrite=True)
    context.setVariable(r"QT_QPA_PLATFORM_PLUGIN_PATH", os.path.join(ext_root_dir,'plugins'), overwrite=True)

    #[PyQt]
    context.addToPythonPath(os.path.join(ext_root_dir, 'sip',python_libdir))

    #[CAS]
    context.setVariable(r"CSF_ShadersDirectory", os.path.join( ext_root_dir, 'share/opencascade/resources/Shaders'), overwrite=True)
    context.setVariable(r"CSF_UnitsLexicon", os.path.join( ext_root_dir, 'share/opencascade/resources/UnitsAPI/Lexi_Expr.dat'), overwrite=True)
    context.setVariable(r"CSF_UnitsDefinition", os.path.join( ext_root_dir, 'share/opencascade/resources/UnitsAPI/Units.dat'), overwrite=True)
    context.setVariable(r"CSF_SHMessage", os.path.join( ext_root_dir, 'share/opencascade/resources/SHMessage'), overwrite=True)
    context.setVariable(r"CSF_XSMessage", os.path.join( ext_root_dir, 'share/opencascade/resources/XSMessage'), overwrite=True)
    context.setVariable(r"CSF_MDTVTexturesDirectory", os.path.join( ext_root_dir, 'share/opencascade/resources/Textures'), overwrite=True)
    context.setVariable(r"MMGT_REENTRANT", r"1", overwrite=True)
    context.setVariable(r"CSF_StandardDefaults", os.path.join( ext_root_dir, 'share/opencascade/resources/StdResource'), overwrite=True)
    context.setVariable(r"CSF_PluginDefaults", os.path.join( ext_root_dir, 'share/opencascade/resources/StdResource'), overwrite=True)
    context.addToPath(ext_root_dir)

    #[ParaView]
    context.setVariable(r"PV_PLUGIN_PATH", os.path.join(LIBDIR,'paraview-5.11'), overwrite=True)
    context.addToVariable(r"PV_PLUGIN_PATH", os.path.join(LIBDIR,'paraview-5.11','plugins'),separator=":")
    context.addToPythonPath(os.path.join(LIBDIR,'paraview-5.11', 'site-packages'))
    context.addToPythonPath(os.path.join(LIBDIR,'paraview-5.11', 'site-packages', 'vtk'))
    context.addToPath(os.path.join(ext_root_dir,'include', 'paraview-5.11'))
    context.addToLdLibraryPath(os.path.join(LIBDIR,'paraview-5.11'))
    context.addToPythonPath(os.path.join(LIBDIR,'paraview-5.11'))
    context.addToPythonPath(python_libdir)