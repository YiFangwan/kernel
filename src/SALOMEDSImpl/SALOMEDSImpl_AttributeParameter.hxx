// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/
//
//  File   : SALOMEDSImpl_AttributeIOR.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef _SALOMEDSImpl_AttributeParameter_HeaderFile
#define _SALOMEDSImpl_AttributeParameter_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <TDF_Attribute.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF_Label.hxx> 
#include "SALOMEDSImpl_GenericAttribute.hxx"

#include <vector>
#include <string>
#include <map>

class Standard_GUID;
class Handle(TDF_Attribute);
class Handle(TDF_RelocationTable);


DEFINE_STANDARD_HANDLE( SALOMEDSImpl_AttributeParameter, SALOMEDSImpl_GenericAttribute )

enum Parameter_Types {PT_INTEGER, PT_REAL, PT_BOOLEAN, PT_STRING, PT_REALARRAY, PT_INTARRAY, PT_STRARRAY};

/*!
 Class: SALOMEDSImpl_AttributeParameter
 Description : AttributeParameter is a universal container of basic types 
*/

class SALOMEDSImpl_AttributeParameter : public SALOMEDSImpl_GenericAttribute 
{

private:
  
  std::map<int, int>                     _ints;
  std::map<int, double>                  _reals;
  std::map<int, std::string>             _strings;
  std::map<int, bool>                    _bools;
  std::map< int, vector<double> >        _realarrays;
  std::map< int, vector<int> >           _intarrays;
  std::map< int, vector<std::string> >   _strarrays;

public:
  Standard_EXPORT static const Standard_GUID& GetID() ;

  Standard_EXPORT  SALOMEDSImpl_AttributeParameter():SALOMEDSImpl_GenericAttribute("AttributeParameter") {}
  Standard_EXPORT  static Handle(SALOMEDSImpl_AttributeParameter) Set (const TDF_Label& L);


  Standard_EXPORT void SetInt(const int theID, const int& theValue);
  Standard_EXPORT int GetInt(const int theID);

  Standard_EXPORT void SetReal(const int theID, const double& theValue);
  Standard_EXPORT double GetReal(const int theID);

  Standard_EXPORT void SetString(const int theID, const TCollection_AsciiString& theValue);
  Standard_EXPORT TCollection_AsciiString GetString(const int theID);
  
  Standard_EXPORT void SetBool(const int theID, const bool& theValue);
  Standard_EXPORT bool GetBool(const int theID);
  
  Standard_EXPORT void SetRealArray(const int theID, const std::vector<double>& theArray);
  Standard_EXPORT std::vector<double> GetRealArray(const int theID);
  
  Standard_EXPORT void SetIntArray(const int theID, const std::vector<int>& theArray);
  Standard_EXPORT std::vector<int> GetIntArray(const int theID);

  Standard_EXPORT void SetStrArray(const int theID, const std::vector<std::string>& theArray);
  Standard_EXPORT std::vector<std::string> GetStrArray(const int theID);

  Standard_EXPORT bool IsSet(const int theID, const Parameter_Types theType);
  
  Standard_EXPORT bool RemoveID(const int theID, const Parameter_Types theType);

  Standard_EXPORT Handle(SALOMEDSImpl_AttributeParameter) GetFather();
  Standard_EXPORT bool HasFather();
  Standard_EXPORT bool IsRoot();

  Standard_EXPORT std::vector<int> GetIDs(const Parameter_Types theType);

  Standard_EXPORT void Clear();

  Standard_EXPORT  virtual TCollection_AsciiString Save();
  Standard_EXPORT  virtual void Load(const TCollection_AsciiString& theValue); 

  Standard_EXPORT  const Standard_GUID& ID() const;
  Standard_EXPORT  void Restore(const Handle(TDF_Attribute)& with) ;
  Standard_EXPORT  Handle_TDF_Attribute NewEmpty() const;
  Standard_EXPORT  void Paste(const Handle(TDF_Attribute)& into,const Handle(TDF_RelocationTable)& RT) const;
  
  Standard_EXPORT ~SALOMEDSImpl_AttributeParameter() {}

public:
  DEFINE_STANDARD_RTTI( SALOMEDSImpl_AttributeParameter )
};

#endif
