#!/usr/bin/env python3
# -*- coding:utf-8 -*-
# Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#  File   : extension_unpacker.py
#  Author : Konstantin Leontev, Open Cascade
#
#  @package SalomeOnDemandTK
#  @brief Set of utility to unpack SALOME python extensions.

"""Set of utility to unpack SALOME python extensions.
"""

import tarfile
import os
import sys
import json
from traceback import format_exc

from .extension_utilities import logger, \
    DFILE_EXT, ARCFILE_EXT, EXTDEPENDSON_KEY, \
    isvalid_filename, isvalid_dirname

def unpack_salomex(salome_root, salomex):
    """
    Unpack a given salome extension archive into SALOME install root.

    Args:
        salome_root - path to SALOME install root directory.
        salomex - a given salomex file to unpack.

    Raises:
        OSError exception.
        KeyError exception if a file can not be found in the archive.

    Returns:
        None.
    """

    logger.debug('Starting unpack a salome extension file')

    # Check if provided filenames are valid
    if  not isvalid_dirname(salome_root) or \
        not isvalid_filename(salomex, ARCFILE_EXT):
        return

    try:
        with tarfile.open(salomex) as ext:
            salome_ext_name, _ = os.path.splitext(os.path.basename(salomex))

            # Read a list of dependencies, so let's check if they are present in salome_root
            logger.debug('Try to read %s.%s file...', salome_ext_name, DFILE_EXT)
            salomexd_mem = ext.getmember(salome_ext_name + '.' + DFILE_EXT)
            salomexd_file = ext.extractfile(salomexd_mem)
            salomexd_content = json.load(salomexd_file)

            logger.debug('Check dependencies...')
            if EXTDEPENDSON_KEY in salomexd_content and salomexd_content[EXTDEPENDSON_KEY]:
                depends_on = salomexd_content[EXTDEPENDSON_KEY]

                # Check every module if it's in the salome_root
                for depends in depends_on:
                    depends_filename = os.path.join(salome_root, depends + '.' + DFILE_EXT)
                    if not os.path.isfile(depends_filename):
                        logger.error('Cannot find %s for a module that extension depends on!',
                            depends_filename)
                        return

            # Unpack archive in the salome_root
            logger.debug('Extract all the files into %s...', salome_root)
            ext.extractall(salome_root)

            logger.debug('SALOME extension %s was installed.', salome_ext_name)

    except (OSError, KeyError):
        logger.error(format_exc())


if __name__ == '__main__':
    if len(sys.argv) == 3:
        arg_1, arg_2 = sys.argv[1:] # pylint: disable=unbalanced-tuple-unpacking
        unpack_salomex(arg_1, arg_2)
    else:
        logger.error('You must provide all the arguments!')
        logger.info(unpack_salomex.__doc__)
