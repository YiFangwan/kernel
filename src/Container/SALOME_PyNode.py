#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

#  File   : SALOME_PyNode.py
#  Author : Christian CAREMOLI, EDF
#  Module : SALOME
#  $Header$
#
import sys,traceback
import linecache
import pickle
import Engines__POA
import SALOME__POA
import SALOME

class Generic(SALOME__POA.GenericObj):
  """A Python implementation of the GenericObj CORBA IDL"""
  def __init__(self,poa):
    self.poa=poa
    self.cnt=1

  def Register(self):
    #print("Register called : %d"%self.cnt)
    self.cnt+=1

  def UnRegister(self):
    #print("UnRegister called : %d"%self.cnt)
    self.cnt-=1
    if self.cnt <= 0:
      oid=self.poa.servant_to_id(self)
      self.poa.deactivate_object(oid)

  def Destroy(self):
    print("WARNING SALOME::GenericObj::Destroy() function is obsolete! Use UnRegister() instead.")
    self.UnRegister()

  def __del__(self):
    #print("Destuctor called")
    pass

class PyNode_i (Engines__POA.PyNode,Generic):
  """The implementation of the PyNode CORBA IDL"""
  def __init__(self, nodeName,code,poa,my_container):
    """Initialize the node : compilation in the local context"""
    Generic.__init__(self,poa)
    self.nodeName=nodeName
    self.code=code
    self.my_container=my_container._container
    linecache.cache[nodeName]=0,None,code.split('\n'),nodeName
    ccode=compile(code,nodeName,'exec')
    self.context={}
    self.context["my_container"] = self.my_container
    exec(ccode, self.context)

  def getContainer(self):
    return self.my_container

  def getCode(self):
    return self.code

  def getName(self):
    return self.nodeName

  def defineNewCustomVar(self,varName,valueOfVar):
    self.context[varName] = pickle.loads(valueOfVar)
    pass

  def executeAnotherPieceOfCode(self,code):
    """Called for initialization of container lodging self."""
    try:
      ccode=compile(code,self.nodeName,'exec')
      exec(ccode, self.context)
    except Exception:
      raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.BAD_PARAM,"","PyScriptNode (%s) : code to be executed \"%s\"" %(self.nodeName,code),0))

  def execute(self,funcName,argsin):
    """Execute the function funcName found in local context with pickled args (argsin)"""
    try:
      argsin,kws=pickle.loads(argsin)
      func=self.context[funcName]
      argsout=func(*argsin,**kws)
      argsout=pickle.dumps(argsout,-1)
      return argsout
    except Exception:
      exc_typ,exc_val,exc_fr=sys.exc_info()
      l=traceback.format_exception(exc_typ,exc_val,exc_fr)
      raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.BAD_PARAM,"".join(l),"PyNode: %s, function: %s" % (self.nodeName,funcName),0))

class SenderByte_i(SALOME__POA.SenderByte,Generic):
  def __init__(self,poa,bytesToSend):
    Generic.__init__(self,poa)
    self.bytesToSend = bytesToSend

  def getSize(self):
    return len(self.bytesToSend)

  def sendPart(self,n1,n2):
    return self.bytesToSend[n1:n2]

SALOME_FILE_BIG_OBJ_DIR = "SALOME_FILE_BIG_OBJ_DIR"
    
SALOME_BIG_OBJ_ON_DISK_THRES_VAR = "SALOME_BIG_OBJ_ON_DISK_THRES"

# default is 50 MB
SALOME_BIG_OBJ_ON_DISK_THRES_DFT = 50000000

from ctypes import c_int
TypeCounter = c_int

def GetSizeOfTCnt():
  return len( bytes(TypeCounter(0) ) )

def GetObjectFromFile(fname):
  with open(fname,"rb") as f:
    cntb = f.read( GetSizeOfTCnt() )
    cnt = TypeCounter.from_buffer_copy( cntb ).value
    obj = pickle.load(f)
  return obj,cnt

def DumpInFile(obj,fname):
  with open(fname,"wb") as f:
    f.write( bytes( TypeCounter(1) ) )
    f.write( obj )

def IncrRefInFile(fname):
  with open(fname,"rb") as f:
    cntb = f.read( GetSizeOfTCnt() )
  cnt = TypeCounter.from_buffer_copy( cntb ).value
  with open(fname,"rb+") as f:
    f.write( bytes( TypeCounter(cnt+1) ) )

def DecrRefInFile(fname):
  import os
  with open(fname,"rb") as f:
    cntb = f.read( GetSizeOfTCnt() )
  cnt = TypeCounter.from_buffer_copy( cntb ).value
  #
  if cnt == 1:
    os.unlink( fname )
  else:
    with open(fname,"rb+") as f:
        f.write( bytes( TypeCounter(cnt-1) ) )

def GetBigObjectOnDiskThreshold():
  import os
  if SALOME_BIG_OBJ_ON_DISK_THRES_VAR in os.environ:
    return int( os.environ[SALOME_BIG_OBJ_ON_DISK_THRES_VAR] )
  else:
    return SALOME_BIG_OBJ_ON_DISK_THRES_DFT

def GetBigObjectDirectory():
  import os
  if SALOME_FILE_BIG_OBJ_DIR not in os.environ:
    raise RuntimeError("An object of size higher than limit detected and no directory specified to dump it in file !")
  return os.path.expanduser( os.path.expandvars( os.environ[SALOME_FILE_BIG_OBJ_DIR] ) )

def GetBigObjectFileName():
  """
  Return a filename in the most secure manner (see tempfile documentation)
  """
  import tempfile
  with tempfile.NamedTemporaryFile(dir=GetBigObjectDirectory(),prefix="mem_",suffix=".pckl") as f:
    ret = f.name
  return ret

class BigObjectOnDiskBase:
  def __init__(self, fileName, objSerialized):
    """
    :param fileName: the file used to dump into.
    :param objSerialized: the object in pickeled form
    :type objSerialized: bytes
    """
    self._filename = fileName
    # attribute _destroy is here to tell client side or server side
    # only client side can be with _destroy set to True. server side due to risk of concurrency
    # so pickled form of self must be done with this attribute set to False.
    self._destroy = False
    self.__dumpIntoFile(objSerialized)

  def getDestroyStatus(self):
    return self._destroy

  def incrRef(self):
    if self._destroy:
      IncrRefInFile( self._filename )
    else:
      # should never happen !
      RuntimeError("Invalid call to incrRef !")

  def decrRef(self):
    if self._destroy:
      DecrRefInFile( self._filename )
    else:
      # should never happen !
      RuntimeError("Invalid call to decrRef !")

  def unlinkOnDestructor(self):
    self._destroy = True

  def doNotTouchFile(self):
    """
    Method called slave side. The life cycle management of file is client side not slave side.
    """
    self._destroy = False

  def __del__(self):
    if self._destroy:
      DecrRefInFile( self._filename )

  def getFileName(self):
    return self._filename
  
  def __dumpIntoFile(self, objSerialized):
    DumpInFile( objSerialized, self._filename )

  def get(self):
    obj, _ = GetObjectFromFile( self._filename )
    return obj

  def __float__(self):
    return float( self.get() )
    
  def __int__(self):
    return int( self.get() )
    
  def __str__(self):
    obj = self.get()
    if isinstance(obj,str):
        return obj
    else:
        raise RuntimeError("Not a string")
      
class BigObjectOnDisk(BigObjectOnDiskBase):
  def __init__(self, fileName, objSerialized):
    BigObjectOnDiskBase.__init__(self, fileName, objSerialized)
    
class BigObjectOnDiskListElement(BigObjectOnDiskBase):
  def __init__(self, pos, length, fileName):
    self._filename = fileName
    self._destroy = False
    self._pos = pos
    self._length = length

  def get(self):
    fullObj = BigObjectOnDiskBase.get(self)
    return fullObj[ self._pos ]
    
  def __getitem__(self, i):
    return self.get()[i]

  def __len__(self):
    return len(self.get())
    
class BigObjectOnDiskSequence(BigObjectOnDiskBase):
  def __init__(self, length, fileName, objSerialized):
    BigObjectOnDiskBase.__init__(self, fileName, objSerialized)
    self._length = length

  def __getitem__(self, i):
    return BigObjectOnDiskListElement(i, self._length, self.getFileName())

  def __len__(self):
    return self._length

class BigObjectOnDiskList(BigObjectOnDiskSequence):
  def __init__(self, length, fileName, objSerialized):
    BigObjectOnDiskSequence.__init__(self, length, fileName, objSerialized)
    
class BigObjectOnDiskTuple(BigObjectOnDiskSequence):
  def __init__(self, length, fileName, objSerialized):
    BigObjectOnDiskSequence.__init__(self, length, fileName, objSerialized)

def SpoolPickleObject( obj ):
  import pickle
  pickleObjInit = pickle.dumps( obj , pickle.HIGHEST_PROTOCOL )
  if len(pickleObjInit) < GetBigObjectOnDiskThreshold():
    return pickleObjInit
  else:
    if isinstance( obj, list):
      proxyObj = BigObjectOnDiskList( len(obj), GetBigObjectFileName() , pickleObjInit )
    elif isinstance( obj, tuple):
      proxyObj = BigObjectOnDiskTuple( len(obj), GetBigObjectFileName() , pickleObjInit )
    else:
      proxyObj = BigObjectOnDisk( GetBigObjectFileName() , pickleObjInit )
    pickleProxy = pickle.dumps( proxyObj , pickle.HIGHEST_PROTOCOL )
    return pickleProxy

def UnProxyObjectSimple( obj ):
  """
  Method to be called in Remote mode. Alterate the obj _status attribute. 
  Because the slave process does not participate in the reference counting
  """
  if isinstance(obj,BigObjectOnDiskBase):
    obj.doNotTouchFile()
    return obj.get()
  elif isinstance( obj, list):
    retObj = []
    for elt in obj:
      retObj.append( UnProxyObjectSimple(elt) )
    return retObj
  else:
    return obj

def UnProxyObjectSimpleLocal( obj ):
  """
  Method to be called in Local mode. Do not alterate the PyObj counter
  """
  if isinstance(obj,BigObjectOnDiskBase):
    return obj.get()
  elif isinstance( obj, list):
    retObj = []
    for elt in obj:
      retObj.append( UnProxyObjectSimpleLocal(elt) )
    return retObj
  else:
    return obj
    
class SeqByteReceiver:
  # 2GB limit to trigger split into chunks
  CHUNK_SIZE = 2000000000
  def __init__(self,sender):
    self._obj = sender
  def __del__(self):
    self._obj.UnRegister()
    pass
  def data(self):
    size = self._obj.getSize()
    if size <= SeqByteReceiver.CHUNK_SIZE:
      return self.fetchOneShot( size )
    else:
      return self.fetchByChunks( size )
  def fetchOneShot(self,size):
    return self._obj.sendPart(0,size)
  def fetchByChunks(self,size):
      """
      To avoid memory peak parts over 2GB are sent using EFF_CHUNK_SIZE size.
      """
      data_for_split_case = bytes(0)
      EFF_CHUNK_SIZE = SeqByteReceiver.CHUNK_SIZE // 8
      iStart = 0 ; iEnd = EFF_CHUNK_SIZE
      while iStart!=iEnd and iEnd <= size:
        part = self._obj.sendPart(iStart,iEnd)
        data_for_split_case = bytes(0).join( [data_for_split_case,part] )
        iStart = iEnd; iEnd = min(iStart + EFF_CHUNK_SIZE,size)
      return data_for_split_case

class PyScriptNode_i (Engines__POA.PyScriptNode,Generic):
  """The implementation of the PyScriptNode CORBA IDL that executes a script"""
  def __init__(self, nodeName,code,poa,my_container):
    """Initialize the node : compilation in the local context"""
    Generic.__init__(self,poa)
    self.nodeName=nodeName
    self.code=code
    self.my_container=my_container._container
    linecache.cache[nodeName]=0,None,code.split('\n'),nodeName
    self.ccode=compile(code,nodeName,'exec')
    self.context={}
    self.context["my_container"] = self.my_container

  def getContainer(self):
    return self.my_container

  def getCode(self):
    return self.code

  def getName(self):
    return self.nodeName

  def defineNewCustomVar(self,varName,valueOfVar):
    self.context[varName] = pickle.loads(valueOfVar)
    pass

  def executeAnotherPieceOfCode(self,code):
    """Called for initialization of container lodging self."""
    try:
      ccode=compile(code,self.nodeName,'exec')
      exec(ccode, self.context)
    except Exception:
      raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.BAD_PARAM,"","PyScriptNode (%s) : code to be executed \"%s\"" %(self.nodeName,code),0))

  def assignNewCompiledCode(self,codeStr):
    try:
      self.code=codeStr
      self.ccode=compile(codeStr,self.nodeName,'exec')
    except Exception:
      raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.BAD_PARAM,"","PyScriptNode.assignNewCompiledCode (%s) : code to be executed \"%s\"" %(self.nodeName,codeStr),0))

  def execute(self,outargsname,argsin):
    """Execute the script stored in attribute ccode with pickled args (argsin)"""
    try:
      argsname,kws=pickle.loads(argsin)
      self.context.update(kws)
      exec(self.ccode, self.context)
      argsout=[]
      for arg in outargsname:
        if arg not in self.context:
          raise KeyError("There is no variable %s in context" % arg)
        argsout.append(self.context[arg])
      argsout=pickle.dumps(tuple(argsout),-1)
      return argsout
    except Exception:
      exc_typ,exc_val,exc_fr=sys.exc_info()
      l=traceback.format_exception(exc_typ,exc_val,exc_fr)
      raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.BAD_PARAM,"".join(l),"PyScriptNode: %s, outargsname: %s" % (self.nodeName,outargsname),0))

  def executeFirst(self,argsin):
    """ Same than first part of self.execute to reduce memory peak."""
    import time
    try:
      data = None
      if True: # to force call of SeqByteReceiver's destructor
        argsInPy = SeqByteReceiver( argsin )
        data = argsInPy.data()
      _,kws=pickle.loads(data)
      for elt in kws:
        # fetch real data if necessary
        kws[elt] = UnProxyObjectSimple( kws[elt] )
      self.context.update(kws)
    except Exception:
      exc_typ,exc_val,exc_fr=sys.exc_info()
      l=traceback.format_exception(exc_typ,exc_val,exc_fr)
      raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.BAD_PARAM,"".join(l),"PyScriptNode:First %s" % (self.nodeName),0))

  def executeSecond(self,outargsname):
    """ Same than second part of self.execute to reduce memory peak."""
    try:
      exec(self.ccode, self.context)
      argsout=[]
      for arg in outargsname:
        if arg not in self.context:
          raise KeyError("There is no variable %s in context" % arg)
        argsout.append(self.context[arg])
      ret = [ ]
      for arg in argsout:
        # the proxy mecanism is catched here
        argPickle = SpoolPickleObject( arg )
        retArg = SenderByte_i( self.poa,argPickle )
        id_o = self.poa.activate_object(retArg)
        retObj = self.poa.id_to_reference(id_o)
        ret.append( retObj._narrow( SALOME.SenderByte ) )
      return ret
    except Exception:
      exc_typ,exc_val,exc_fr=sys.exc_info()
      l=traceback.format_exception(exc_typ,exc_val,exc_fr)
      raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.BAD_PARAM,"".join(l),"PyScriptNode:Second %s, outargsname: %s" % (self.nodeName,outargsname),0))

  def listAllVarsInContext(self):
      import re
      pat = re.compile("^__([a-z]+)__$")
      return [elt for elt in self.context if not pat.match(elt)]
      
  def removeAllVarsInContext(self):
      for elt in self.listAllVarsInContext():
        del self.context[elt]

  def getValueOfVarInContext(self,varName):
    try:
      return pickle.dumps(self.context[varName],-1)
    except Exception:
      exc_typ,exc_val,exc_fr=sys.exc_info()
      l=traceback.format_exception(exc_typ,exc_val,exc_fr)
      raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.BAD_PARAM,"".join(l),"PyScriptNode: %s" %self.nodeName,0))
    pass
  
  def assignVarInContext(self, varName, value):
    try:
      self.context[varName][0] = pickle.loads(value)
    except Exception:
      exc_typ,exc_val,exc_fr=sys.exc_info()
      l=traceback.format_exception(exc_typ,exc_val,exc_fr)
      raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.BAD_PARAM,"".join(l),"PyScriptNode: %s" %self.nodeName,0))
    pass

  def callMethodOnVarInContext(self, varName, methodName, args):
    try:
      return pickle.dumps( getattr(self.context[varName][0],methodName)(*pickle.loads(args)),-1 )
    except Exception:
      exc_typ,exc_val,exc_fr=sys.exc_info()
      l=traceback.format_exception(exc_typ,exc_val,exc_fr)
      raise SALOME.SALOME_Exception(SALOME.ExceptionStruct(SALOME.BAD_PARAM,"".join(l),"PyScriptNode: %s" %self.nodeName,0))
    pass
