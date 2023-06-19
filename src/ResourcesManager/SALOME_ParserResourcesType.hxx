// Copyright (C) 2023  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See https://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  SALOME ResourcesCatalog : implementation of catalog resources parsing (SALOME_ModuleCatalog.idl)
//  File   : SALOME_ParserResourcesType.hxx
//  Author : Konstantin Leontev
//  Module : KERNEL
//$Header$
//
#ifndef SALOME_PARSER_RESOURCES_TYPE
#define SALOME_PARSER_RESOURCES_TYPE

#include "SALOME_ParserResourcesTypeCommon.hxx"

#include <vector>
#include <list>
#include <iostream>

#ifdef WIN32
#pragma warning(disable:4251) // Warning DLL Interface ...
#endif

class RESOURCESMANAGER_EXPORT ParserResourcesType
{
public:
  ParserResourcesType();
  virtual ~ParserResourcesType();

  // Implementation must return true for ParserResourcesTypeJob
  virtual bool isCanLaunchBatchJob() const = 0;
  // Implementation must return true for ParserResourcesTypeContainer
  virtual bool isCanRunContainers() const = 0;

  std::string getAccessProtocolTypeStr() const;
  void setAccessProtocolTypeStr(const std::string& protocolTypeStr);

  std::string getBatchTypeStr() const;
  void setBatchTypeStr(const std::string& batchTypeStr);

  // Same as in resource definintion in idl/SALOME_Resource.idl file
  std::string name;
  std::string hostname;
  AccessProtocolType protocol;
  std::string username;
  std::string applipath;
  BatchType batch;
  
  // TODO: all the members below wasn't declared in idl/SALOME_Resource.idl file
  // TODO: decide if we need these members in the base class 
  std::string batchQueue;
  std::string userCommands;
  std::string use;

protected:
  static std::string protocolToString(AccessProtocolType protocol);
  static AccessProtocolType stringToProtocol(const std::string& protocolStr);

  virtual void print(std::ostream& os) const;
};

#endif //SALOME_PARSER_RESOURCES_TYPE
