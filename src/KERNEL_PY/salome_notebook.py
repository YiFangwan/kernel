#  -*- coding: iso-8859-1 -*-
#  Copyright (C) 2008  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#
#
#  File   : salome_notebook.py
#  Author : Roman NIKOLAEV, Alexandre SOLOVYOV, Open CASCADE S.A.S.
#  Module : SALOME
#  $Header:

"""
Module salome_notebook gives access to Salome Notebook.
"""

import salome
import SALOME

class Parameter:
    """
    A wrapper class for the CORBA Parameter object.
    """

    def __init__( self, aName, aNoteBook = None ):
        """
        Constructor
        """
        aName = aName.strip()
        if not aName:
            raise "Empty parameter name"
        if not aNoteBook:
            aNoteBook = Notebook( salome.myStudy )
        self.myNotebook = aNoteBook
        self.myParameter = self.myNotebook.getNotebook().GetParameter( aName )
        if not self.myParameter:
            raise "Parameter '%s' is not defined in the study" % aName
        pass
    
    def name( self ):
        """
        Get notebook variable name
        """
        return self.myParameter.GetEntry()
    
    def value( self ):
        """
        Get notebook variable value
        """
        if self.myParameter.GetType() == SALOME.TReal:
            return self.myParameter.AsReal()
            
        elif self.myParameter.GetType() == SALOME.TInteger:
            return self.myParameter.AsInteger()
            
        elif self.myParameter.GetType() == SALOME.TBoolean:
            return self.myParameter.AsBoolean()
            
        elif self.myParameter.GetType() == SALOME.TString:
            return self.myParameter.AsString()
            
        return None
    
    pass # end of Parameter class
        
class Notebook:
    """
    A wrapper class for the CORBA Notebook object.
    """
    
    def __init__( self, Study ):
        """
        Constructor
        """
        self.myNotebook = Study.GetNotebook()

    def getNotebook( self ):
        """
        Get access to the internal CORBA Notebook reference
        """
        return self.myNotebook

    def update( self ):
        """
        Update notebook (re-calculate all dependencies)
        """
        self.myNotebook.Update()
        
    def set( self, variableName, variable ):
	"""
	Create (or modify) variable with name 'variableName' 
	and value equal 'theValue'.
	"""

        aParam = self.myNotebook.GetParameter( variableName )
        if type( variable ) == float:
            if aParam == None:
                self.myNotebook.AddReal( variableName, variable )
            else:
                aParam.SetReal( variable )
                
        elif type( variable ) == str:
            if aParam == None:
                self.myNotebook.AddNamedExpression( variableName, variable )
            else:
                aParam.SetExpression( variable )
            
        elif type( variable ) == int:
            if aParam == None:
                self.myNotebook.AddInteger( variableName, variable )
            else:
                aParam.SetInteger( variable )
            
        elif type( variable ) == bool:
            if aParam == None:
                self.myNotebook.AddBoolean( variableName, variable )
            else:
                aParam.SetBoolean( variable )
            
    def get( self, variableName ):
	"""
	Return value of the variable with name 'variableName'.
	"""
        aResult = None
        try:
            aParam = Parameter( variableName, self )
            aResult = aParam.value()
            pass
        except:
            pass
        return aResult
    
    def isVariable( self, variableName ): 
	"""
	Return True if variable with name 'variableName'
	exists in the study, otherwise return False.
	"""
        aParam = self.myNotebook.GetParameter( variableName )
        return aParam != None

    def dump( self ):
        """
        Dump current contents of the notebook.
        """
        return self.myNotebook.Dump()

    def rename( self, oldname, newname ):
        """
        Rename notebook variable 'oldname' to 'newname'
        """
        self.myNotebook.Rename( oldname, newname )

    def remove( self, variableName ):
        """
        Remove notebook variable with the given 'variableName' name
        """
        self.myNotebook.Remove( variableName )

    pass # end of Notebook class

###
# global notebook variable, initialized by the currently active study
###
notebook = Notebook(salome.myStudy)
