#!/usr/bin/env python3
# -*- coding:utf-8 -*-
# Copyright (C) 2022  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See https://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

#  File   : runSalomeOnDemand.py
#  Author : Konstantin Leontev, Open Cascade
#
## @package runSalomeOnDemand
# \brief Module that provides services to launch SALOME with custom set of modules
#

"""Run SALOME app in the context of adding modules as extensions.
"""

import os
import sys
import importlib.util
from pathlib import Path
import salomeContext
from SalomeOnDemandTK.extension_utilities import logger, \
    DFILE_EXT, SALOME_EXTDIR, \
    list_files_ext, find_envpy


def module_from_filename(filename):
    """
    Create and execute a module by filename.

    Args:
        filename - a given python filename.

    Returns:
        Module.
    """

    # Get the module from the filename
    basename = os.path.basename(filename)
    module_name, _ = os.path.splitext(basename)

    spec = importlib.util.spec_from_file_location(module_name, filename)
    if not spec:
        logger.error('Could not get a spec for %s file!')
        return None

    module = importlib.util.module_from_spec(spec)
    if not module:
        logger.error('Could not get a module for %s file!')
        return None

    sys.modules[module_name] = module

    logger.debug('Execute %s module', module_name)
    spec.loader.exec_module(module)

    return module


def set_ext_env(app_name='', version=''):
    """
    Set an environment to start SALOME as a set of extensions.

    Args:
        app_name - an application's name.
        version - a version of an application.

    Returns:
        None.
    """

    logger.debug('Set an env for app: %s, version: %s...', app_name, version)

    # Get the root directory
    levels_up = 4
    app_root = str(Path(__file__).resolve().parents[levels_up - 1])
    logger.debug('App root: %s', app_root)

    # Find and source all _env.py files for installed extensions
    installed_ext = list_files_ext(app_root, DFILE_EXT)
    logger.debug('Installed extensions: %s', installed_ext)
    if not installed_ext:
        logger.debug('There are not any extensions in %s!', app_root)
        return

    # Execute env file as a module
    context = salomeContext.SalomeContext(None)
    ext_root = os.path.join(app_root, SALOME_EXTDIR)
    for salomexd in installed_ext:
        salomexd_name = os.path.basename(salomexd)
        ext_name, _ = os.path.splitext(salomexd_name)

        # Find env file
        ext_envpy = find_envpy(app_root, ext_name)
        if not ext_envpy:
            continue

        # Get a module
        envpy_module = module_from_filename(ext_envpy)
        if not envpy_module:
            continue

        # Set env if we have something to set
        if hasattr(envpy_module, 'init'):
            envpy_module.init(context, ext_root)


if __name__ == "__main__":
    if len(sys.argv) == 3:
        arg_1, arg_2 = sys.argv[1:]
        set_ext_env(arg_1, arg_2)
    else:
        logger.error('You must provide all the arguments!')
        logger.info(set_ext_env.__doc__)
