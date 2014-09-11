#! /usr/bin/env python
#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import optparse

def generate_sources( options, args ):
    pass

# -----------------------------------------------------------------------------

if __name__ == '__main__':

    parser = optparse.OptionParser( usage = "usage: python app-quickstart.py [options]" )

    parser.add_option('-p',
                      "--prefix",
                      metavar="</Path/to/the/sources/of/application>",
                      type="string",
                      action="store",
                      dest="prefix",
                      default='.',
                      help="Where the application's sources will be generated")

    parser.add_option('-m',
                      "--modules",
                      metavar="<module1,module2,...>",
                      type="string",
                      action="append",
                      dest="modules",
                      default='KERNEL,GUI',
                      help="List of the application's modules")

    (options, args) = parser.parse_args()

    generate_sources( options, args )
