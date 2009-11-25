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

class Notebook:
    
    def __init__( self, Study ):
        self.myNotebook = Study.GetNotebook()

    def getNotebook( self ):
        return self.myNotebook

    def update( self ):
        self.myNotebook.Update( 0 )
        
    def set( self, variableName, variable ):
	"""
	Create (or modify) variable with name "variableName" 
	and value equal "theValue".
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
	Return value of the variable with name "variableName".
	"""
        aResult = None
        aParam = self.myNotebook.GetParameter( variableName )
        if aParam != None:

            if aParam.GetType() == SALOME.TReal:
                aResult = aParam.AsReal()

            elif aParam.GetType() == SALOME.TInteger:
                aResult = aParam.AsInteger()

            elif aParam.GetType() == SALOME.TBoolean:
                aResult = aParam.AsBoolean()

            elif aParam.GetType() == SALOME.TString:
                aResult = aParam.AsString()
                
        return aResult
    
    def isVariable( self, variableName ): 
	"""
	Return true if variable with name "variableName" 
	exists in the study, otherwise return false.
	"""
        aParam = self.myNotebook.GetParameter( variableName )
        return aParam != None

    def dump( self ):
        return self.myNotebook.Dump()

    def rename( self, oldname, newname ):
        self.myNotebook.Rename( oldname, newname )

    def remove( self, name ):
        self.myNotebook.Remove( name )


notebook = Notebook(salome.myStudy)

def Parameter( varName ):
    return notebook.getNotebook().GetParameter( varName )
