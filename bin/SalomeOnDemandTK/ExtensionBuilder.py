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
import os
import sys
import logging
import io
import json
import fnmatch

# Setup logger's output
FORMAT = '%(funcName)s():%(lineno)s: %(message)s'
logging.basicConfig(format=FORMAT, level=logging.DEBUG)
logger = logging.getLogger()

'''Salome Extension Directory inside any extension's archive'''
salomeExtDir = '__SALOME_EXT__'
arcFileNameExt = 'salomex'
bFileNameExt = 'salomexb'
cFileNameExt = 'salomexc'
dFileNameExt = 'salomexd'

extNameKey = 'name'
extDescrKey = 'descr'
extDependsOnKey = 'dependsOn'
extAuthorKey = 'author'
extComponentsKey = 'components'


def createSalomexdFile(name, descr = '', dependsOn = [], author = '', components = []):
    '''
        Create a basic salomexd file from provided args.
    '''

    logger.debug('Create salomexd file...')

    jsonString = json.dumps(
        {
        extNameKey: name,
        extDescrKey: descr,
        extDependsOnKey: dependsOn,
        extAuthorKey: author,
        extComponentsKey: components
        },
        indent = 4
        )

    try:
        with open(name + '.' + dFileNameExt, "w") as file:
            file.write(jsonString)

    except Exception:
        import traceback  
        logger.error(traceback.format_exc())


def createSalomexbFile(name, included):
    '''
        Create a salomexb file from a list of included file names.
        For example:
        */*.py
        doc/*.html
        doc/*.jp
    '''

    logger.debug('Create salomexb file...')

    try:
        with open(name + '.' + bFileNameExt, "w") as file:
            file.write('\n'.join(included[0:]))

    except Exception:
        import traceback  
        logger.error(traceback.format_exc())    


def readSalomexbFile(filePath):
    '''
        Returns a content af a salomexb file as a list of strings
    '''

    logger.debug('Read salomexb file %s', filePath)

    try:
        with open(filePath, 'r', encoding='UTF-8') as file:
            return [line.rstrip() for line in file]

    except Exception:
        import traceback  
        logger.error(traceback.format_exc())     
        return []


def readJsonFile(filePath):
    '''
        Returns a content af a json file as a dictionary
    '''
    
    logger.debug('Read json file %s', filePath)

    try:
        with open(filePath) as jsonFile:
            return json.load(jsonFile)

    except Exception:
        import traceback  
        logger.error(traceback.format_exc())     
        return {}


def listFilesRel(dirPath):
    '''
        Returns the recursive list of relative paths to files as strings
        in the dirPath root directory and all subdirectories.
    '''

    filesList = []
    for root, dirs, files in os.walk(dirPath):
        for file in files:
            filesList += os.path.relpath(os.path.join(root, file), dirPath)

    return filesList


def fileListToNewLineStr(fileList):
    '''
        Returns the list of paths as a newline separated string
    '''
    return '\n'.join(file for file in fileList)


def createSalomexFile(sourceDirPath, salomexbPath, salomexdPath):
    '''
        Makes salome extension archive from a classic module install directory
    '''

    logger.debug('Starting create an salome extension file')

    if sourceDirPath == '':
        logger.error('sourceDirPath argument cannot be empty!')
        return

    extName = ''

    # Try to get info from salomexd file
    salomexdContent = readJsonFile(salomexdPath)
    if extNameKey in salomexdContent and salomexdContent[extNameKey]:
        extName = salomexdContent[extNameKey]

    if extName == '':
        extName = os.path.basename(sourceDirPath)

    try:
        with tarfile.open(extName + '.' + arcFileNameExt, "w:gz") as ext:
            # Write all included files to the extension's dir

            # Get the file's matching pattern in the first place
            includedFilesPattern = readSalomexbFile(salomexbPath)
            if not includedFilesPattern:
                # We don't have any pattern, so include all the files
                includedFilesPattern = ['*']

            logger.debug('Included files pattern: %s', includedFilesPattern)

            # List of the files those actually written to the archive.
            # It goes to the salomexc file then.
            files = []

            def fileFilter(tarinfo):
                logger.debug('File name: %s', tarinfo.name)

                # Skip dir itself, otherwise it excludes all the files inside
                if tarinfo.isdir():
                    return tarinfo

                for f in includedFilesPattern:
                    if fnmatch.fnmatch(tarinfo.name, f):
                        logger.debug('File name %s matches pattern %s', tarinfo.name, f)                     
                        files.append(tarinfo.name)
                        return tarinfo
                
                return None

            ext.add(sourceDirPath, salomeExtDir, recursive = True, filter = fileFilter)

            # Write the control file - list of the files inside extension's dir
            extDirListData = fileListToNewLineStr(files).encode('utf8')
            info = tarfile.TarInfo(extName + '.' + cFileNameExt)
            info.size=len(extDirListData)
            ext.addfile(info, io.BytesIO(extDirListData))

            # Write the description file as is
            ext.add(salomexdPath)

    except Exception:
        import traceback  
        logger.error(traceback.format_exc())


if __name__ == '__main__':
    createSalomexFile(sys.argv[1:])
