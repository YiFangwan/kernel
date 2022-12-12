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

#  File   : extension_utilities.py
#  Author : Konstantin Leontev, Open Cascade
#
#  @package SalomeOnDemandTK
#  @brief Set of utility functions those help to build SALOME python extensions.

"""
Utilities and constants those help deal with salome extension files.
"""

import os
import logging
import json
from traceback import format_exc

# Setup logger's output
FORMAT = '%(funcName)s():%(lineno)s: %(message)s'
logging.basicConfig(format=FORMAT, level=logging.DEBUG)
logger = logging.getLogger()

SALOME_EXTDIR = '__SALOME_EXT__'
ARCFILE_EXT = 'salomex'
BFILE_EXT = 'salomexb'
CFILE_EXT = 'salomexc'
DFILE_EXT = 'salomexd'
PYFILE_EXT = 'py'
ENVPYFILE_SUF = '_env.py'

EXTNAME_KEY = 'name'
EXTDESCR_KEY = 'descr'
EXTDEPENDSON_KEY = 'depends_on'
EXTAUTHOR_KEY = 'author'
EXTCOMPONENT_KEY = 'components'


def create_salomexd(name, descr='', depends_on=None, author='', components=None):
    """
    Create a basic salomexd file from provided args.
    Current version is a json file with function args as the keys.

    Args:
        name - the name of the corresponding salome extension.
        depends_on - list of the modules that current extension depends on.
        author - an author of the extension.
        components - the names of the modules those current extension was built from.

    Raises:
        Raises OSError exception.

    Returns:
        None
    """

    logger.debug('Create salomexd file...')

    if depends_on is None:
        depends_on = []

    if components is None:
        components = []

    json_string = json.dumps(
        {
            EXTNAME_KEY: name,
            EXTDESCR_KEY: descr,
            EXTDEPENDSON_KEY: depends_on,
            EXTAUTHOR_KEY: author,
            EXTCOMPONENT_KEY: components
        },
        indent=4
    )

    try:
        with open(name + '.' + DFILE_EXT, "w", encoding="utf-8") as file:
            file.write(json_string)

    except OSError:
        logger.error(format_exc())


def read_salomexd(file_path):
    """
    Reads a content of a salomexd file. Current version is a json file.
    There's no check if the file_path is a valid salomexd file name.
    It's expected that user call isvalid_filename() in advance.

    Args:
        file_path - the path to the salomexd file.

    Raises:
        Raises OSError exception.

    Returns:
        A dictionary that represents the content of the salomexd file.
    """

    logger.debug('Read salomexd file %s', file_path)

    try:
        with open(file_path, 'r', encoding='UTF-8') as file:
            return json.load(file)

    except OSError:
        logger.error(format_exc())
        return {}


def create_salomexb(name, included):
    """
    Create a salomexb file from a list of included file names.
    For example:
    */*.py
    doc/*.html
    doc/*.jp

    Args:
        name - the name of the corresponding salome extension.
        included - list of the directories those must be included inside a salomex.

    Raises:
        Raises OSError exception.

    Returns:
        None
    """

    logger.debug('Create salomexb file...')

    try:
        with open(name + '.' + BFILE_EXT, "w", encoding="utf-8") as file:
            file.write('\n'.join(included[0:]))

    except OSError:
        logger.error(format_exc())


def read_salomexb(file_path):
    """
    Returns a content af a salomexb file as a list of strings.
    There's no check if the file_path is a valid salomexb file name.
    It's expected that user call isvalid_filename() in advance.

    Args:
        file_path - the path to the salomexb file.

    Raises:
        Raises OSError exception.

    Returns:
        List of strings - paths to the directories those must be included in
        corresponding salomex archive file.
    """

    logger.debug('Read salomexb file %s', file_path)

    try:
        with open(file_path, 'r', encoding='UTF-8') as file:
            return [line.rstrip() for line in file]

    except OSError:
        logger.error(format_exc())
        return []


def list_files(dir_path):
    """
    Returns the recursive list of relative paths to files as strings
    in the dir_path root directory and all subdirectories.

    Args:
        dir_path - the path to the directory where you search for files.

    Raises:
        Raises OSError exception.

    Returns:
        A list of relative paths to files inside the given directory.
    """

    files_list = []
    for root, _, files in os.walk(dir_path):
        for file in files:
            files_list += os.path.relpath(os.path.join(root, file), dir_path)

    return files_list


def list_files_filter(dir_path, filter_patterns):
    """
    Returns the recursive list of relative paths to files as strings
    in the dir_path root directory and all subdirectories.

    Args:
        dir_path - the path to the directory where you search for files.
        filter_patterns - list of expressions for matching file names.

    Returns:
        files_abs - a list of absolute paths to selected files.
        files_rel - a list of relative paths to selected files.
    """

    logger.debug('Get list of files to add into archive...')

    files_abs = []
    files_rel = []

    for root, _, files in os.walk(dir_path):
        for file in files:
            for pattern in filter_patterns:
                filename_abs = os.path.join(root, file)
                filename_rel = os.path.relpath(filename_abs, dir_path)

                if filename_rel.startswith(pattern):
                    logger.debug('File name %s matches pattern %s', filename_rel, pattern)
                    files_abs.append(filename_abs)
                    files_rel.append(filename_rel)

    return files_abs, files_rel


def list_files_ext(dir_path, ext):
    """
    Returns a list of abs paths to files with a given extension
    in the dir_path directory.

    Args:
        dir_path - the path to the directory where you search for files.
        ext - a given extension.

    Returns:
        A list of absolute paths to selected files.
    """

    logger.debug('Get list of files with extension %s...', ext)

    dot_ext = '.' + ext
    return [os.path.join(dir_path, f) for f in os.listdir(dir_path) if f.endswith(dot_ext)]


def list_tonewline_str(str_list):
    """
    Converts the given list of strings to a newline separated string.

    Args:
        dir_path - the path to the directory where you search for files.

    Returns:
        A newline separated string.
    """
    return '\n'.join(file for file in str_list)


def isvalid_filename(filename, extension):
    """
    Checks if a given filename is valid in a sense that it exists and have a given extension.

    Args:
        filename - the name of the file to check.
        extension - expected file name extension.

    Returns:
        True if the given filename is valid for given extension.
    """

    logger.debug('Check if the filename %s exists and has an extension %s', filename, extension)

    # First do we even have something to check here
    if filename == '' or extension == '':
        logger.error('A filename and extension cannot be empty! Args: filename=%s, extension=%s',
            filename, extension)
        return False

    # Check if the filename matchs the provided extension
    _, ext = os.path.splitext(filename)
    ext = ext.lstrip('.')
    if ext != extension:
        logger.error('The filename %s doesnt have a valid extension! \
            The valid extension must be: %s, but get: %s',
            filename, extension, ext)
        return False

    # Check if the file base name is not empty
    base_name = os.path.basename(filename)
    if base_name == '':
        logger.error('The file name %s has an empty base name!', filename)
        return False

    # Check if a file with given filename exists
    if not os.path.isfile(filename):
        logger.error('The filename %s is not an existing regular file!', filename)
        return False

    logger.debug('Filename %s exists and has an extension %s', filename, extension)
    return True


def isvalid_dirname(dirname):
    """
    Checks if a given directory name exists.

    Args:
        dirname - the name of the directory to check.

    Returns:
        True if the given dirname is valid.
    """

    logger.debug('Check if the dirname %s exists', dirname)

    # First do we even have something to check here
    if dirname == '':
        logger.error('A dirname argument cannot be empty! dirname=%s', dirname)
        return False

    # Check if a file with given filename exists
    if not os.path.isdir(dirname):
        logger.error('The dirname %s is not an existing regular file!', dirname)
        return False

    logger.debug('Directory %s exists', dirname)
    return True


def list_dependants(install_dir, salomex_name):
    """
    Checks if we have installed extensions those depend on a given extension.

    Args:
        install_dir - path to SALOME install root directory.
        salomex_name - a name of salome extension to check.

    Returns:
        True if the given extension has dependants.
    """

    logger.debug('Check if there are other extensions that depends on %s...', salomex_name)
    dependants = []
    salomexd_files = list_files_ext(install_dir, DFILE_EXT)

    for salomexd_file in salomexd_files:
        logger.debug('Check dependencies for %s...', salomexd_file)
        salomexd_content = read_salomexd(salomexd_file)

        if EXTDEPENDSON_KEY in salomexd_content and salomexd_content[EXTDEPENDSON_KEY]:
            depends_on = salomexd_content[EXTDEPENDSON_KEY]
            logger.debug('List of dependencies: %s', depends_on)

            if salomex_name in depends_on:
                dependant_name = None
                if EXTNAME_KEY in salomexd_content and salomexd_content[EXTNAME_KEY]:
                    dependant_name = salomexd_content[EXTNAME_KEY]
                else:
                    logger.warning('%s file doesn\'t have %s key! '
                        'Get an extension name from the filename.',
                        salomexd_file, EXTNAME_KEY)
                    dependant_name, _ = os.path.splitext(os.path.basename(salomexd_file))

                dependants.append(dependant_name)

    if len(dependants) > 0:
        logger.debug('An extension %s has followed extensions those depend on it: %s',
            salomex_name, dependants)

    return dependants


def is_empty_dir(directory):
    """
    Checks if the given directory is empty.

    Args:
        directory - path to directory to check.

    Returns:
        True if the given directory is empty.
    """

    return not next(os.scandir(directory), None)
