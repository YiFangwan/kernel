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

class WrappedType(object):
    def __init__(self,varPtr,isTemporaryVar=False):
        assert(isinstance(varPtr,SALOME._objref_PickelizedPyObjRdWrServer))
        self._var_ptr=varPtr
        if not isTemporaryVar:
            self._var_ptr.Register()
        self._is_temp=isTemporaryVar
        pass

    def local_copy(self):
        return cPickle.loads(self._var_ptr.fetchSerializedContent())

    def __str__(self):
        return self.local_copy().__str__()

    def __repr__(self):
        return self.local_copy().__repr__()

    def __reduce__(self):
        return (self._wrapped_type,(self.local_copy(),))

    def assign(self,elt):
        st=cPickle.dumps(elt,cPickle.HIGHEST_PROTOCOL)
        self._var_ptr.setSerializedContent(st)
        pass

    def __del__(self):
        self._var_ptr.UnRegister()
        pass
    pass

class List(WrappedType):
    def __init__(self,varPtr,isTemporaryVar=False):
        WrappedType.__init__(self,varPtr,isTemporaryVar)
        self._wrapped_type=list
        pass

    def __getitem__(self,*args):
        ret=Caller(self._var_ptr,"__getitem__")
        return ret(*args)

    def __setitem__(self,*args):
        ret=Caller(self._var_ptr,"__setitem__")
        return ret(*args)
    
    def __delitem__(self,*args):
        ret=Caller(self._var_ptr,"__delitem__")
        return ret(*args)

    def append(self,*args):
        ret=Caller(self._var_ptr,"append")
        return ret(*args)

    def extend(self,*args):
        ret=Caller(self._var_ptr,"extend")
        return ret(*args)

    def insert(self,*args):
        ret=Caller(self._var_ptr,"insert")
        return ret(*args)

    def pop(self,*args):
        ret=Caller(self._var_ptr,"pop")
        return ret(*args)

    def remove(self,*args):
        ret=Caller(self._var_ptr,"remove")
        return ret(*args)

    def reverse(self,*args):
        ret=Caller(self._var_ptr,"reverse")
        return ret(*args)

    def sort(self,*args):
        ret=Caller(self._var_ptr,"sort")
        return ret(*args)

    #
    
    def count(self,*args):
        return self.local_copy().count(*args)

    def index(self,*args):
        return self.local_copy().index(*args)

    def __len__(self):
        return len(self.local_copy())
    pass

class Tuple(WrappedType):
    def __init__(self,varPtr,isTemporaryVar=False):
        WrappedType.__init__(self,varPtr,isTemporaryVar)
        self._wrapped_type=tuple
        pass

    def __getitem__(self,*args):
        ret=Caller(self._var_ptr,"__getitem__")
        return ret(*args)
    
    # work on local copy

    def count(self,*args):
        return self.local_copy().count(*args)

    def index(self,*args):
        return self.local_copy().index(*args)
    
    def __len__(self):
        return len(self.local_copy())

    pass

class Dict(WrappedType):
    def __init__(self,varPtr,isTemporaryVar=False):
        WrappedType.__init__(self,varPtr,isTemporaryVar)
        self._wrapped_type=dict
        pass

    def __getitem__(self,*args):
        ret=Caller(self._var_ptr,"__getitem__")
        return ret(*args)

    def __setitem__(self,*args):
        ret=Caller(self._var_ptr,"__setitem__")
        return ret(*args)

    def __delitem__(self,*args):
        ret=Caller(self._var_ptr,"__delitem__")
        return ret(*args)
    
    def clear(self,*args):
        ret=Caller(self._var_ptr,"clear")
        return ret(*args)

    def get(self,*args):
        ret=Caller(self._var_ptr,"get")
        return ret(*args)

    def items(self,*args):
        ret=Caller(self._var_ptr,"items")
        return ret(*args)

    def pop(self,*args):
        ret=Caller(self._var_ptr,"pop")
        return ret(*args)

    def popitem(self,*args):
        ret=Caller(self._var_ptr,"popitem")
        return ret(*args)

    def setdefault(self,*args):
        ret=Caller(self._var_ptr,"setdefault")
        return ret(*args)

    def update(self,*args):
        ret=Caller(self._var_ptr,"update")
        return ret(*args)

    def values(self,*args):
        ret=Caller(self._var_ptr,"values")
        return ret(*args)
 
    # work on local copy

    def __contains__(self,*args):
        return self.local_copy().__contains__(*args)

    def has_key(self,*args):
        return self.local_copy().has_key(*args)

    def keys(self,*args):
        return self.local_copy().keys(*args)

    def copy(self,*args):
        return self.local_copy().copy(*args)

    def __len__(self):
        return len(self.local_copy())

    pass

class Float(WrappedType):
    def __init__(self,varPtr,isTemporaryVar=False):
        WrappedType.__init__(self,varPtr,isTemporaryVar)
        self._wrapped_type=float
        pass

    def __iadd__(self,*args):
        ret=Caller(self._var_ptr,"__add__")
        return ret(*args)

    def __isub__(self,*args):
        ret=Caller(self._var_ptr,"__sub__")
        return ret(*args)

    def __imul__(self,*args):
        ret=Caller(self._var_ptr,"__mul__")
        return ret(*args)

    def __idiv__(self,*args):
        ret=Caller(self._var_ptr,"__div__")
        return ret(*args)

    def __add__(self,*args):
        ret=Caller(self._var_ptr,"__add__")
        return ret(*args)

    def __sub__(self,*args):
        ret=Caller(self._var_ptr,"__sub__")
        return ret(*args)

    def __mul__(self,*args):
        ret=Caller(self._var_ptr,"__mul__")
        return ret(*args)

    def __div__(self,*args):
        ret=Caller(self._var_ptr,"__div__")
        return ret(*args)
    
    def __pow__(self,*args):
        ret=Caller(self._var_ptr,"__pow__")
        return ret(*args)

    def as_integer_ratio(self,*args):
        ret=Caller(self._var_ptr,"as_integer_ratio")
        return ret(*args)

    def conjugate(self,*args):
        ret=Caller(self._var_ptr,"conjugate")
        return ret(*args)

    def fromhex(self,*args):
        ret=Caller(self._var_ptr,"fromhex")
        return ret(*args)

    def hex(self,*args):
        ret=Caller(self._var_ptr,"hex")
        return ret(*args)

    def imag(self,*args):
        ret=Caller(self._var_ptr,"imag")
        return ret(*args)

    def is_integer(self,*args):
        ret=Caller(self._var_ptr,"is_integer")
        return ret(*args)

    def real(self,*args):
        ret=Caller(self._var_ptr,"real")
        return ret(*args)
    pass

class Int(WrappedType):
    def __init__(self,varPtr,isTemporaryVar=False):
        WrappedType.__init__(self,varPtr,isTemporaryVar)
        self._wrapped_type=int
        pass

    def __iadd__(self,*args):
        ret=Caller(self._var_ptr,"__add__")
        return ret(*args)

    def __isub__(self,*args):
        ret=Caller(self._var_ptr,"__sub__")
        return ret(*args)

    def __imul__(self,*args):
        ret=Caller(self._var_ptr,"__mul__")
        return ret(*args)

    def __idiv__(self,*args):
        ret=Caller(self._var_ptr,"__div__")
        return ret(*args)

    def __add__(self,*args):
        ret=Caller(self._var_ptr,"__add__")
        return ret(*args)

    def __sub__(self,*args):
        ret=Caller(self._var_ptr,"__sub__")
        return ret(*args)

    def __mul__(self,*args):
        ret=Caller(self._var_ptr,"__mul__")
        return ret(*args)

    def __div__(self,*args):
        ret=Caller(self._var_ptr,"__div__")
        return ret(*args)

    def __pow__(self,*args):
        ret=Caller(self._var_ptr,"__pow__")
        return ret(*args)

    def bit_length(self,*args):
        ret=Caller(self._var_ptr,"bit_length")
        return ret(*args)

    def conjugate(self,*args):
        ret=Caller(self._var_ptr,"conjugate")
        return ret(*args)

    def denominator(self,*args):
        ret=Caller(self._var_ptr,"denominator")
        return ret(*args)

    def imag(self,*args):
        ret=Caller(self._var_ptr,"imag")
        return ret(*args)
    
    def numerator(self,*args):
        ret=Caller(self._var_ptr,"numerator")
        return ret(*args)

    def real(self,*args):
        ret=Caller(self._var_ptr,"real")
        return ret(*args)
    pass

class String(WrappedType):
    def __init__(self,varPtr,isTemporaryVar=False):
        WrappedType.__init__(self,varPtr,isTemporaryVar)
        self._wrapped_type=int
        pass

    def __add__(self,*args):
        ret=Caller(self._var_ptr,"__add__")
        return ret(*args)

    def __iadd__(self,*args):
        ret=Caller(self._var_ptr,"__add__")
        return ret(*args)

    def __getitem__(self,*args):
        ret=Caller(self._var_ptr,"__getitem__")
        return ret(*args)

    def capitalize(self,*args):
        ret=Caller(self._var_ptr,"capitalize")
        return ret(*args)

    def center(self,*args):
        ret=Caller(self._var_ptr,"center")
        return ret(*args)

    def count(self,*args):
        ret=Caller(self._var_ptr,"count")
        return ret(*args)

    def decode(self,*args):
        ret=Caller(self._var_ptr,"decode")
        return ret(*args)

    def encode(self,*args):
        ret=Caller(self._var_ptr,"encode")
        return ret(*args)

    def endswith(self,*args):
        ret=Caller(self._var_ptr,"endswith")
        return ret(*args)

    def expandtabs(self,*args):
        ret=Caller(self._var_ptr,"expandtabs")
        return ret(*args)

    def find(self,*args):
        ret=Caller(self._var_ptr,"find")
        return ret(*args)

    def format(self,*args):
        ret=Caller(self._var_ptr,"format")
        return ret(*args)

    def index(self,*args):
        ret=Caller(self._var_ptr,"index")
        return ret(*args)

    def isalnum(self,*args):
        ret=Caller(self._var_ptr,"isalnum")
        return ret(*args)

    def isalpha(self,*args):
        ret=Caller(self._var_ptr,"isalpha")
        return ret(*args)

    def isdigit(self,*args):
        ret=Caller(self._var_ptr,"isdigit")
        return ret(*args)

    def islower(self,*args):
        ret=Caller(self._var_ptr,"islower")
        return ret(*args)

    def isspace(self,*args):
        ret=Caller(self._var_ptr,"isspace")
        return ret(*args)

    def istitle(self,*args):
        ret=Caller(self._var_ptr,"istitle")
        return ret(*args)

    def isupper(self,*args):
        ret=Caller(self._var_ptr,"isupper")
        return ret(*args)

    def join(self,*args):
        ret=Caller(self._var_ptr,"join")
        return ret(*args)

    def ljust(self,*args):
        ret=Caller(self._var_ptr,"ljust")
        return ret(*args)

    def lower(self,*args):
        ret=Caller(self._var_ptr,"lower")
        return ret(*args)

    def lstrip(self,*args):
        ret=Caller(self._var_ptr,"lstrip")
        return ret(*args)

    def partition(self,*args):
        ret=Caller(self._var_ptr,"partition")
        return ret(*args)

    def replace(self,*args):
        ret=Caller(self._var_ptr,"replace")
        return ret(*args)

    def rfind(self,*args):
        ret=Caller(self._var_ptr,"rfind")
        return ret(*args)

    def rindex(self,*args):
        ret=Caller(self._var_ptr,"rindex")
        return ret(*args)

    def rjust(self,*args):
        ret=Caller(self._var_ptr,"rjust")
        return ret(*args)

    def rpartition(self,*args):
        ret=Caller(self._var_ptr,"rpartition")
        return ret(*args)

    def rsplit(self,*args):
        ret=Caller(self._var_ptr,"rsplit")
        return ret(*args)

    def rstrip(self,*args):
        ret=Caller(self._var_ptr,"rstrip")
        return ret(*args)

    def split(self,*args):
        ret=Caller(self._var_ptr,"split")
        return ret(*args)

    def splitlines(self,*args):
        ret=Caller(self._var_ptr,"splitlines")
        return ret(*args)

    def startswith(self,*args):
        ret=Caller(self._var_ptr,"startswith")
        return ret(*args)

    def strip(self,*args):
        ret=Caller(self._var_ptr,"strip")
        return ret(*args)

    def swapcase(self,*args):
        ret=Caller(self._var_ptr,"swapcase")
        return ret(*args)

    def title(self,*args):
        ret=Caller(self._var_ptr,"title")
        return ret(*args)

    def translate(self,*args):
        ret=Caller(self._var_ptr,"translate")
        return ret(*args)

    def upper(self,*args):
        ret=Caller(self._var_ptr,"upper")
        return ret(*args)

    def zfill(self,*args):
        ret=Caller(self._var_ptr,"zfill")
        return ret(*args)

    def __len__(self):
        return len(self.local_copy())
    pass

class Caller:
    def __init__(self,varPtr,meth):
        assert(isinstance(varPtr,SALOME._objref_PickelizedPyObjRdWrServer))
        self._var_ptr=varPtr
        self._meth=meth
        pass

    def __call__(self,*args):
        ret=self._var_ptr.invokePythonMethodOn(self._meth,cPickle.dumps(args,cPickle.HIGHEST_PROTOCOL))
        return GetHandlerFromRef(ret,True)
    pass

PyHandlerTypeMap={int:Int,float:Float,str:String,list:List,tuple:Tuple,dict:Dict}

def GetHandlerFromRef(objCorba,isTempVar=False):
    """ Returns a client that allows to handle a remote corba ref of a global var easily.
    """
    assert(isinstance(objCorba,SALOME._objref_PickelizedPyObjRdWrServer))
    v=cPickle.loads(objCorba.fetchSerializedContent())
    if v is None:
        return None
    return PyHandlerTypeMap[v.__class__](objCorba,isTempVar)

def GetHandlerFromName(scopeName,varName):
    import salome
    dsm=salome.naming_service.Resolve("/DataServerManager")
    d2s=dsm.giveADataScopeCalled(scopeName)
    return GetHandlerFromRef(d2s.retrieveVar(varName),False)
