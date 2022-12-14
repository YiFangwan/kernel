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

#  File   : extension_query.py
#  Author : Konstantin Leontev, Open Cascade
#
#  @package SalomeOnDemandTK
#  @brief An utility package that will allow you to know the size of an extension
# and generate a dependency tree.

"""
An utility package that will allow you to know the size of an extension
and generate a dependency tree.
"""

import os
import sys
from traceback import format_exc

from .extension_utilities import logger, \
    SALOME_EXTDIR, DFILE_EXT, EXTDEPENDSON_KEY, \
    isvalid_dirname, find_salomexc, list_files_ext, read_salomexd


def size_to_str(size, format_in_bytes=False):
    """
    Returns a string describing a size.

    Args:
        size - the size to represent as a string.
        format_in_bytes - if True, size is expressed in bytes,
            otherwise human readable units are used.

    Returns:
        The size as a string with units.
    """

    if format_in_bytes:
        return '%s' % size

    __units__ = ["", "Ki", "Mi", "Gi", "Ti"]

    kilo = 1024.0
    prefix_index = 0
    prefix_index_max = len(__units__) - 1
    while (size > kilo and prefix_index < prefix_index_max):
        prefix_index += 1
        size /= kilo

    unit = __units__[prefix_index]
    return '%.2f %sB' % (size, unit)


def dir_size(directory):
    """
    Calculate a total size of the given directory.

    Args:
        directory - the given directory

    Returns:
        Size of the directory.
    """

    logger.debug('Get the size of %s', directory)

    total_size = 0
    for root, _, files in os.walk(directory):
        for file in files:
            itempath = os.path.join(root, file)
            if os.path.islink(itempath):
                continue

            total_size += os.path.getsize(itempath)

    logger.debug('The size of %s: %s', directory, total_size)
    return total_size


def dir_size_str(directory, format_in_bytes=False):
    """
    Calculate a total size of the given directory and format as a string.

    Args:
        directory - the given directory
        format_in_bytes - if True, size is expressed in bytes,
            otherwise human readable units are used.

    Returns:
        Size of the directory as a formatted string.
    """

    size = dir_size(directory)
    size_str = size_to_str(size, format_in_bytes)

    logger.debug('The size of %s: %s', directory, size_str)
    return size_str


def size_bylist(root_dir, salomexc):
    """
    Calcualate the total size of files listed in the given salomexc file.

    Args:
        root_dir - a root dir for listed files
        salomexc - file that contents a list of files.

    Returns:
        The total size of listed files.
    """

    logger.debug('Calcualate the total size of files inside %s listed in %s...',
        root_dir, salomexc)

    try:
        with open(salomexc, 'r', encoding='UTF-8') as file:
            total_size = 0
            for line in file:
                path_to_file = os.path.join(root_dir, line.strip())
                #logger.debug('Check the file %s...', path_to_file)

                if os.path.isfile(path_to_file):
                    size = os.path.getsize(path_to_file)
                    total_size += size
                    logger.debug('%s size: %s', path_to_file, size)

                elif os.path.islink(path_to_file):
                    logger.debug('%s is link. Skip.', path_to_file)
                    continue

                elif os.path.isdir(path_to_file):
                    logger.warning('Directories are not expected to be listed in %s file! '
                        'Skip.',
                        salomexc)
                    continue

                else:
                    logger.warning('Unexpected path %s '
                        'is not a file, link or directory. Skip.',
                        path_to_file)

            return total_size

    except OSError:
        logger.error(format_exc())

    return None


def ext_size(install_dir, salomex_name):
    """
    Calculate a total size of a salome extension from SALOME install root.

    Args:
        salome_root - path to SALOME install root directory.
        salomex_name - a name of the given salome extension.

    Returns:
        Size of the directory in bytes.
    """

    # Check if provided dirname is valid
    if not isvalid_dirname(install_dir):
        return None

    # Check if an extension with this name is installed
    salomexc = find_salomexc(install_dir, salomex_name)
    if not salomexc:
        logger.error('Cannot calculate the size of %s extension!',
            salomex_name)
        return None

    # Calculate the size
    return size_bylist(os.path.join(install_dir, SALOME_EXTDIR), salomexc)


def ext_size_str(install_dir, salomex_name, format_in_bytes=False):
    """
    Calculate a total size of the given extension and format as a string.

    Args:
        install_dir - directory where the given extension is installed.
        salomex_name - the given extension's name.
        format_in_bytes - if True, size is expressed in bytes,
            otherwise human readable units are used.

    Returns:
        Size of the extension as a formatted string.
    """

    size = ext_size(install_dir, salomex_name)
    if size is None:
        return ''

    size_str = size_to_str(size, format_in_bytes)

    logger.debug('The size of %s: %s', salomex_name, size_str)
    return size_str


def dependency_tree(directory):
    r"""
    Create a dependency tree for all salome extensions
    installed in the given directory.

    Args:
        directory - the given directory

    Returns:
        A dictionary like that for extensions A, B, C, D and E:
          A
         /|\
        / B D
        \/ \/
        C   E

        { 'A': ['B', 'C', 'D'], 'B': ['C', 'E'], 'C': [], 'D': ['E'], 'E': [] }.
    """

    logger.debug('Build dependency tree for extensions in %s', directory)

    tree = {}
    salomexd_files = list_files_ext(directory, DFILE_EXT)
    logger.debug('There are %s extensions in %s', len(salomexd_files), directory)

    for salomexd_file in salomexd_files:
        ext_name, _ = os.path.splitext(os.path.basename(salomexd_file))

        logger.debug('Check dependencies for %s...', salomexd_file)
        salomexd_content = read_salomexd(salomexd_file)

        if EXTDEPENDSON_KEY in salomexd_content:
            depends_on = salomexd_content[EXTDEPENDSON_KEY]
            logger.debug('List of dependencies: %s', depends_on)

            tree[ext_name] = depends_on

    logger.debug('Dependency tree: %s', tree)
    return tree


if __name__ == '__main__':
    if len(sys.argv) == 2:
        dir_size_str(sys.argv[1])
        dependency_tree(sys.argv[1])
    elif len(sys.argv) == 3:
        arg_1, arg_2 = sys.argv[1:] # pylint: disable=unbalanced-tuple-unpacking
        ext_size_str(arg_1, arg_2)
    else:
        logger.error('You must provide all the arguments!')
        logger.info(dir_size_str.__doc__)
        logger.info(dependency_tree.__doc__)
        logger.info(ext_size_str.__doc__)
