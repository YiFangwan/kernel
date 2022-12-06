#!/usr/bin/env python
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

#  File   : ExtensionBuilder.py
#  Author : Konstantin Leontev, Open Cascade
#
#  @package SalomeOnDemandTK
#  @brief Set of utility functions those help to build SALOME python extensions.

"""Build salome extension archive in tar.gz format.
"""

import tarfile
import os
import sys
import logging
import io
import json
import traceback

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

EXTNAME_KEY = 'name'
EXTDESCR_KEY = 'descr'
EXTDEPENDSON_KEY = 'dependsOn'
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
        logger.error(traceback.format_exc())


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
        logger.error(traceback.format_exc())
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
        logger.error(traceback.format_exc())


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
        logger.error(traceback.format_exc())
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

    logger.debug('Check if the filename %s exists and have an extension %s', filename, extension)

    # First do we even have something to check here
    if filename == '' or extension == '':
        logger.error('A filename and extension cannot be empty! Args: filename=%s, extension=%s',
            filename, extension)
        return False

    # Check if the filename matchs the provided extension
    _, ext = os.path.splitext(filename)
    #ext.replace('.', '')
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

    logger.debug('Filename %s exists and has, extension %s', filename, extension)
    return True


def isvalid_direname(dirname):
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


def create_salomex(salomexb, salomexd, env_py, top_repository):
    """
    Makes salome extension archive from provided in salomexb file directories.

    Args:
        salomexb - a path to the <extension>.salomexb file
        salomexd - a path to the <extension>.salomexd file
        env_py - a path to the <ext>_env.py file
        top_repository - a root directory for all the paths listed inside salomexb file

    Raises:
        Raises OSError exception.

    Returns:
        None.
    """

    logger.debug('Starting create an salome extension file')

    # Check if provided filenames are valid
    if  not isvalid_filename(salomexb, BFILE_EXT) or \
        not isvalid_filename(salomexd, DFILE_EXT) or \
        not isvalid_filename(env_py, PYFILE_EXT) or \
        not isvalid_direname(top_repository):
        return

    # Try to get info from salomexd file
    salome_ext_name = ''
    salomexd_content = read_salomexd(salomexd)
    if EXTNAME_KEY in salomexd_content and salomexd_content[EXTNAME_KEY]:
        salome_ext_name = salomexd_content[EXTNAME_KEY]
    else:
        # Now as a last resort we get a name right from salomexd filename
        # We need to decide if we can handle this case handsomely
        salome_ext_name = os.path.basename(salomexd)
        logger.warning('Cannot get a SALOME extension name from salomexd file! \
            Use salomexd file name as an extension name.')

    logger.debug('Set an extension name as: %s', salome_ext_name)

    try:
        with tarfile.open(salome_ext_name + '.' + ARCFILE_EXT, "w:gz") as ext:
            # Write all included files to the extension's dir

            # Get the file's matching pattern in the first place
            included_files_patterns = read_salomexb(salomexb)
            logger.debug('Included files pattern: %s', included_files_patterns)
            if not included_files_patterns:
                # We don't have any pattern, so we don't know what we must put inside an archive
                logger.error('Cannot create salomex file: \
                    a list of included files patterns is empty.')
                return

            # List of the files those actually written to the archive.
            # It goes to the salomexc file then.
            files_abs, files_rel = list_files_filter(top_repository, included_files_patterns)
            for (file_abs, file_rel) in zip(files_abs, files_rel):
                ext.add(file_abs, SALOME_EXTDIR + '/' + file_rel)

            # Write the control file - list of the files inside extension's dir
            included_files_data = list_tonewline_str(files_rel).encode('utf8')
            info = tarfile.TarInfo(salome_ext_name + '.' + CFILE_EXT)
            info.size = len(included_files_data)
            ext.addfile(info, io.BytesIO(included_files_data))

            # Write the description file as is
            ext.add(salomexd, os.path.basename(salomexd))

            # Write the env_py file as is
            ext.add(env_py, os.path.basename(env_py))

    except OSError:
        logger.error(traceback.format_exc())


if __name__ == '__main__':
    if len(sys.argv) == 5:
        arg_1, arg_2, arg_3, arg_4 = sys.argv[1:]
        create_salomex(arg_1, arg_2, arg_3, arg_4)
    else:
        logger.error('You must provide all the arguments!')
        logger.info(create_salomex.__doc__)
