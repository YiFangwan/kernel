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
import salomeContext
from SalomeOnDemandTK.extension_utilities import logger, \
    DFILE_EXT, \
    list_files_ext, set_selext_env, get_app_root


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
    app_root = get_app_root()

    # Find and source all _env.py files for installed extensions
    installed_ext = list_files_ext(app_root, DFILE_EXT)
    logger.debug('Installed extensions: %s', installed_ext)
    if not installed_ext:
        logger.debug('There are not any extensions in %s!', app_root)
        return

    # Set the root dir as env variable
    context = salomeContext.SalomeContext(None)
    context.setVariable('SALOME_APPLICATION_DIR', app_root, overwrite=True)

    # Execute env file as a module
    for salomexd in installed_ext:
        salomexd_name = os.path.basename(salomexd)
        ext_name, _ = os.path.splitext(salomexd_name)

        set_selext_env(app_root, ext_name, context)


if __name__ == "__main__":
    if len(sys.argv) == 3:
        arg_1, arg_2 = sys.argv[1:]
        set_ext_env(arg_1, arg_2)
    else:
        logger.error('You must provide all the arguments!')
        logger.info(set_ext_env.__doc__)
