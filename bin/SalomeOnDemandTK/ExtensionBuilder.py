#  -*- coding: iso-8859-1 -*-
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
## @package SalomeOnDemandTK
#  @brief Set of utility functions those help to build SALOME python extensions.


import tarfile
import os.path
import sys
import logging
import io
import pathlib

# Setup logger's output
FORMAT = '%(funcName)s():%(lineno)s: %(message)s'
logging.basicConfig(format=FORMAT, level=logging.DEBUG)
logger = logging.getLogger()

"""Salome Extension Directory inside any extension's archive"""
salomeExtDir = '__SALOME_EXT__'
salomexcName = 'salomexc'

def listFilesAbs(dirPath):
    """
        Returns the recursive list of absolut paths to files (as pathlib.Path objects)
        in the dirPath root directory and all subdirectories.
    """

    files = []

    for path in pathlib.Path(dirPath).iterdir():
        if path.is_file():
            files.append(path)
        else:
            files += listFilesAbs(path)

    return files

def listFilesRel(dirPath):
    """
        Returns the recursive list of relative paths to files (as pathlib.Path objects)
        in the dirPath root directory and all subdirectories.
    """

    filesAbs = listFilesAbs(dirPath)
    return [file.relative_to(dirPath) for file in filesAbs]

def listFiles(dirPath, isRel = True):
    """
        Returns the recursive list of paths to files (as a list of strings)
        in the dirPath root directory and all subdirectories.
    """

    pathList = listFilesRel(dirPath) if isRel else listFilesAbs(dirPath)
    return [str(path) for path in pathList]

def fileListToNewLineStr(fileList):
    """
        Returns the list of paths as a newline separated string
    """
    return '\n'.join(file for file in fileList)

def salomeExtFromInstallDir(sourceDirPath, extName = ''):
    """
        Makes salome extension archive from a classic module install directory
    """

    logger.debug('salomeExtFromInstallDir() call')

    sourceDirName = os.path.basename(sourceDirPath)
    if extName == '':
        extName = sourceDirName

    try:
        with tarfile.open(extName, "w:gz") as ext:
            ext.add(sourceDirPath, salomeExtDir)

            # Write control file - list of files inside extension's dir
            files = listFiles(sourceDirPath)
            extDirListData = fileListToNewLineStr(files).encode('utf8')
            info = tarfile.TarInfo(salomexcName)
            info.size=len(extDirListData)
            ext.addfile(info, io.BytesIO(extDirListData))

    except Exception:
        import traceback  
        logger.error(traceback.format_exc())

if __name__ == '__main__':
    salomeExtFromInstallDir(sys.argv[1])
