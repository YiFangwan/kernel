// Copyright (C) 2008  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : SALOMEDSImpl_GenericVariable.cxx
//  Author : Roman NIKOLAEV, Open CASCADE S.A.S.
//  Module : SALOME

#include "SALOMEDSImpl_GenericVariable.hxx"
#include "SALOMEDSImpl_Attributes.hxx"
#include "SALOMEDSImpl_Study.hxx"

#include <string>


using namespace std;

//============================================================================
/*! Function : SALOMEDSImpl_GenericVariable
 *  Purpose  : 
 */
//============================================================================
SALOMEDSImpl_GenericVariable::
SALOMEDSImpl_GenericVariable(SALOMEDSImpl_GenericVariable::VariableTypes theType,
                             const string& theName): 
  _type(theType),
  _name(theName)
{}

//============================================================================
/*! Function : ~SALOMEDSImpl_GenericVariable
 *  Purpose  : 
 */
//============================================================================
SALOMEDSImpl_GenericVariable::~SALOMEDSImpl_GenericVariable()
{}

//============================================================================
/*! Function : Type
 *  Purpose  : 
 */
//============================================================================
SALOMEDSImpl_GenericVariable::VariableTypes SALOMEDSImpl_GenericVariable::Type() const
{
  return _type;
}

//============================================================================
/*! Function : Name
 *  Purpose  : 
 */
//============================================================================
string SALOMEDSImpl_GenericVariable::Name() const
{
  return _name;
}

//============================================================================
/*! Function : setType
 *  Purpose  : 
 */
//============================================================================
void SALOMEDSImpl_GenericVariable::setType(const SALOMEDSImpl_GenericVariable::VariableTypes theType)
{
  _type = theType;
}

//============================================================================
/*! Function : setName
 *  Purpose  : 
 */
//============================================================================
void SALOMEDSImpl_GenericVariable::setName(const std::string& theName)
{
  _name = theName;
}

//============================================================================
/*! Function : CheckLocked
 *  Purpose  : 
 */
//============================================================================
void SALOMEDSImpl_GenericVariable::CheckLocked()
{
  DF_Label aLabel = DF_Label();
  if(aLabel.IsNull()) return;

  SALOMEDSImpl_Study* aStudy = SALOMEDSImpl_Study::GetStudy(aLabel);
  if(!aStudy) return;
  if(aStudy->IsLocked()) {
    aStudy->_errorCode = "LockProtection";
    throw LockProtection("LockProtection");
  }                                         
}

//============================================================================
/*! Function : SetModifyFlag
 *  Purpose  : 
 */
//============================================================================
void SALOMEDSImpl_GenericVariable::SetModifyFlag()
{
  DF_Label aLabel = DF_Label();
  if(aLabel.IsNull()) return; 
  
  SALOMEDSImpl_Study* aStudy = SALOMEDSImpl_Study::GetStudy(aLabel);
  if(aStudy) aStudy->Modify();
}


//============================================================================
/*! Function : String2VariableType
 *  Purpose  : 
 */
//============================================================================
SALOMEDSImpl_GenericVariable::VariableTypes SALOMEDSImpl_GenericVariable::String2VariableType(const string& theStrType)
{
  return(SALOMEDSImpl_GenericVariable::VariableTypes)atoi((char*)theStrType.c_str());
}

//============================================================================
/*! Function : Save
 *  Purpose  : 
 */
//============================================================================
string SALOMEDSImpl_GenericVariable::Save() const
{
  return string();
}


//============================================================================
/*! Function : SaveToScript
 *  Purpose  : 
 */
//============================================================================
string SALOMEDSImpl_GenericVariable::SaveToScript() const
{
  return string();
}
//============================================================================
/*! Function : SaveType
 *  Purpose  : 
 */
//============================================================================
string SALOMEDSImpl_GenericVariable::SaveType() const
{
  return string();
}

//============================================================================
/*! Function : Load
 *  Purpose  : 
 */
//============================================================================
void SALOMEDSImpl_GenericVariable::Load(const string& theStrValue)
{
}
