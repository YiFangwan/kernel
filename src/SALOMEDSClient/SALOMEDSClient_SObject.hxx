//  SALOME SALOMEDS : data structure of SALOME and sources of Salome data server 
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : SALOMEDSClient_SObject.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDSCLIENT_SOBJECT_I_H__
#define __SALOMEDSCLIENT_SOBJECT_I_H__

// std C++ headers
#include <vector>
#include <string>

//SALOMEDSClient headers
#include <SALOMEDSClient_GenericAttribute.hxx>
#include <SALOMEDSClient_SComponent.hxx>
//#include <SALOMEDSClient_Study.hxx>

class SALOMEDSClient_SObject
{

public:
  
  virtual string GetID()  = 0;
  virtual SALOMEDSClient_SComponent* GetFatherComponent() = 0;
  virtual SALOMEDSClient_SObject*    GetFather() = 0;
  virtual bool FindAttribute(SALOMEDSClient_GenericAttribute* anAttribute, const strin& aTypeOfAttribute) = 0;
  virtual bool ReferencedObject(SALOMEDSClient_SObject* theObject) = 0;
  virtual bool FindSubObject(long theTag, SALOMEDSClient_SObject* theObject) = 0;
  //virtual SALOMEDSClient_Study* GetStudy() = 0;
  virtual string Name() = 0;
  virtual void  Name(const string& theName)  = 0;
  virtual vector<SALOMEDSClient_GenericAttribute*> GetAllAttributes() = 0;
  virtual string GetName() = 0;
  virtual string GetComment() = 0;
  virtual string GetIOR() = 0;
  virtual short Tag() = 0;
  virtual short Depth() = 0;
};
#endif
