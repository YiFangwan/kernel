#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2014  CEA/DEN, EDF R&D
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
# Author : Anthony Geay

import SALOME
import cPickle

class SALOMEGlobalVarMethodHelper:
    def __init__(self,varPtr,meth):
        assert(isinstance(varPtr,SALOME._objref_StringDataServer))
        self._var_ptr=varPtr
        self._meth=meth
        pass

    def __call__(self,*args):
        #print "__call__ %s : %s"%(self._meth,str(args))
        ret=self._var_ptr.invokePythonMethodOn(self._meth,cPickle.dumps(args,cPickle.HIGHEST_PROTOCOL))
        return cPickle.loads(ret)
    pass

class SALOMEGlobalVarHelper:
    def __init__(self,varPtr):
        assert(isinstance(varPtr,SALOME._objref_StringDataServer))
        self._var_ptr=varPtr
        pass
    
    def assign(self,elt):
        st=cPickle.dumps(elt,cPickle.HIGHEST_PROTOCOL)
        self._var_ptr.setValueOf(st)
        pass
        
    def __getattr__(self,*args):
        #print "__getattr__,%s"%(str(args))
        return SALOMEGlobalVarMethodHelper(self._var_ptr,args[0])

    def __str__(self):
        return self.local_copy().__str__()

    def __repr__(self):
        return self.local_copy().__repr__()

    def local_copy(self):
        return cPickle.loads(self._var_ptr.getValueOf())    
    pass
