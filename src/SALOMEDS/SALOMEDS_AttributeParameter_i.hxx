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
//  File   : SALOMEDS_AttributeParameter_i.hxx
//  Author : Sergey RUIN
//  Module : SALOME
//  $Header:

#ifndef SALOMEDS_AttributeParameter_i_HeaderFile
#define SALOMEDS_AttributeParameter_i_HeaderFile

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#include "SALOMEDS_GenericAttribute_i.hxx"
#include "SALOMEDSImpl_AttributeParameter.hxx"

/*!
 Class: SALOMEDS_AttributeParameter
 Description : AttributeParameter is a universal container of basic types 
*/

class SALOMEDS_AttributeParameter_i: public virtual POA_SALOMEDS::AttributeParameter,
				     public virtual SALOMEDS_GenericAttribute_i 
{
public:
  
  SALOMEDS_AttributeParameter_i(const Handle(SALOMEDSImpl_AttributeParameter)& theAttr, CORBA::ORB_ptr orb) 
    :SALOMEDS_GenericAttribute_i(theAttr, orb) {}; 

  ~SALOMEDS_AttributeParameter_i() {};


  virtual void SetInt(CORBA::Long theID, CORBA::Long theValue);
  virtual CORBA::Long GetInt(CORBA::Long theID);

  virtual void SetReal(CORBA::Long theID, const CORBA::Double theValue);
  virtual CORBA::Double GetReal(CORBA::Long theID);

  virtual void SetString(CORBA::Long theID, const char* theValue);
  virtual char* GetString(CORBA::Long theID);
  
  virtual void SetBool(CORBA::Long theID, CORBA::Boolean theValue);
  virtual CORBA::Boolean GetBool(CORBA::Long theID);
  
  virtual void SetRealArray(CORBA::Long theID, const SALOMEDS::DoubleSeq& theArray);
  virtual SALOMEDS::DoubleSeq* GetRealArray(CORBA::Long theID);

  virtual void SetIntArray(CORBA::Long theID, const SALOMEDS::LongSeq& theArray);
  virtual SALOMEDS::LongSeq* GetIntArray(CORBA::Long theID);

  virtual void SetStrArray(CORBA::Long theID, const SALOMEDS::StringSeq& theArray);
  virtual SALOMEDS::StringSeq* GetStrArray(CORBA::Long theID);
  
  virtual CORBA::Boolean IsSet(CORBA::Long theID, CORBA::Long theType);
  
  virtual CORBA::Boolean RemoveID(CORBA::Long theID, CORBA::Long theType);

  virtual SALOMEDS::AttributeParameter_ptr GetFather();
  virtual CORBA::Boolean HasFather();
  virtual CORBA::Boolean IsRoot();

  virtual void Clear();

  virtual SALOMEDS::LongSeq* GetIDs(CORBA::Long theType);

};




#endif
