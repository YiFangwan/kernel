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
//  File   : SALOMEDS_AttributeParameter.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDS_AttributeParameter_HeaderFile
#define SALOMEDS_AttributeParameter_HeaderFile

#include "SALOMEDSClient_AttributeParameter.hxx"
#include "SALOMEDS_GenericAttribute.hxx"
#include "SALOMEDSImpl_AttributeParameter.hxx"

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

class SALOMEDS_AttributeParameter: public SALOMEDS_GenericAttribute, public SALOMEDSClient_AttributeParameter
{
public:  
  SALOMEDS_AttributeParameter(const Handle(SALOMEDSImpl_AttributeParameter)& theAttr);
  SALOMEDS_AttributeParameter(SALOMEDS::AttributeParameter_ptr theAttr);
  ~SALOMEDS_AttributeParameter();

  virtual void SetInt(const int theID, const int& theValue);
  virtual int GetInt(const int theID);

  virtual void SetReal(const int theID, const double& theValue);
  virtual double GetReal(const int theID);

  virtual void SetString(const int theID, const std::string& theValue);
  virtual std::string GetString(const int theID);
  
  virtual void SetBool(const int theID, const bool& theValue);
  virtual bool GetBool(const int theID);
  
  virtual void SetRealArray(const int theID, const std::vector<double>& theArray);
  virtual std::vector<double> GetRealArray(const int theID);
  
  virtual void SetIntArray(const int theID, const std::vector<int>& theArray);
  virtual std::vector<int> GetIntArray(const int theID);

  virtual void SetStrArray(const int theID, const std::vector<std::string>& theArray);
  virtual std::vector<std::string> GetStrArray(const int theID);

  virtual bool IsSet(const int theID, const int theType);
  
  virtual bool RemoveID(const int theID, const int theType);

  virtual _PTR(AttributeParameter) GetFather();
  virtual bool HasFather();
  virtual bool IsRoot();
  
  virtual void Clear();

  virtual std::vector<int> GetIDs(const int theType);
};

#endif
