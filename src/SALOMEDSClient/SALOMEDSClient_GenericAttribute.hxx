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
//  File   : SALOMEDSClient__GenericAttribute_i.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef _GENERICCLIENT_ATTRIBUTE_HXX_
#define _GENERICCLIENT_ATTRIBUTE_HXX_

#include <string> 
#include <exception>

//SALOMEDSClient headers 
#include "SALOMEDSClient_SObject.hxx"

class LockProtection : exception
{};


class SALOMEDS_GenericAttribute: 
{
public:
  
  void CheckLocked() throw (LockProtection) = 0;
  string Store() = 0;
  void Restore(const strin&) = 0;
  string Type() = 0;
  SALOMEDSClient_SObject* GetSObject() = 0;
};

#endif
