#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#  File   : salome_pynode.py
#  Author : Christian CAREMOLI, EDF
#  Module : SALOME
#  $Header$
#

"""
 When imported this module adds to CORBA proxy (from PyNode type) automatic pickle and unpickle
 of arguments and results when calling execute method. It also converts the SALOME exception into a standard python
 exception 
"""
import omniORB
import cPickle
import SALOME
import Engines

class SmartPyNode(Engines._objref_PyNode):
  def __init__(self):
    Engines._objref_PyNode.__init__(self)

  def execute(self,functionName,*args,**kws):
    try:
      args=cPickle.dumps((args,kws),-1)
      results=Engines._objref_PyNode.execute(self,functionName,args)
      x=cPickle.loads(results)
      return x
    except SALOME.SALOME_Exception, e:
      raise ValueError(e.details.text)

  def __getattr__(self,name):
    """ a trick to be able to call directly a remote method by its name : no need to use execute"""
    if name[0]== '_':
      raise AttributeError, name
    def afunc(*args,**kws):
      return self.execute(name,*args,**kws)
    return afunc

#Register the new proxy for PyNode
omniORB.registerObjref(Engines._objref_PyNode._NP_RepositoryId, SmartPyNode)
