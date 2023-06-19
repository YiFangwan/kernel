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
//  File   : SALOME_ParserResourcesTypeJob.hxx
//  Author : Konstantin Leontev
//  Module : KERNEL
//$Header$
//
#ifndef SALOME_PARSER_RESOURCES_TYPE_JOB
#define SALOME_PARSER_RESOURCES_TYPE_JOB

#include "SALOME_ParserResourcesType.hxx"

#include <map>

#ifdef WIN32
#pragma warning(disable:4251) // Warning DLL Interface ...
#endif

class RESOURCESMANAGER_EXPORT ParserResourcesTypeJob : public ParserResourcesType
{
public:
  using TypeMap = std::map<std::string, ParserResourcesTypeJob>;
  using TypeMapIt = std::map<std::string, ParserResourcesTypeJob>::iterator;

  ParserResourcesTypeJob();
  virtual ~ParserResourcesTypeJob();

  virtual bool isCanLaunchBatchJob() const override { return true; }
  virtual bool isCanRunContainers() const override { return false; }

  std::string getMpiImplTypeStr() const;
  void setMpiImplTypeStr(const std::string& mpiImplTypeStr);

  std::string getClusterInternalProtocolStr() const;
  void setClusterInternalProtocolStr(const std::string& internalProtocolTypeStr);

  // Same as in resource definintion in idl/SALOME_ResourceJob.idl file
  MpiImplType mpiImpl;
  AccessProtocolType iprotocol;
  std::string working_directory;

protected:
  virtual void print(std::ostream& os) const override;

  RESOURCESMANAGER_EXPORT friend std::ostream& operator<<(std::ostream& os,
                                                           const ParserResourcesTypeJob& rt);
};

#endif //SALOME_PARSER_RESOURCES_TYPE_JOB
