#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
# 
#  This library is free software; you can redistribute it and/or 
#  modify it under the terms of the GNU Lesser General Public 
#  License as published by the Free Software Foundation; either 
#  version 2.1 of the License. 
# 
#  This library is distributed in the hope that it will be useful, 
#  but WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
#  Lesser General Public License for more details. 
# 
#  You should have received a copy of the GNU Lesser General Public 
#  License along with this library; if not, write to the Free Software 
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
# 
#  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
#
#
#
#  File   : salome.py
#  Author : Paul RASCLE, EDF
#  Module : SALOME
#  $Header$

from salome_kernel import *
from salome_study import *
from salome_iapp import *

salome_initial=1
def salome_init():
    global salome_initial
    global orb, lcc, naming_service, cm
    global sg
    global myStudyManager, myStudyId, myStudy, myStudyName
    
    if salome_initial:
        salome_initial=0
        orb, lcc, naming_service, cm = salome_kernel_init()
        sg = salome_iapp_init()
        myStudyManager, myStudyId, myStudy, myStudyName =salome_study_init()

