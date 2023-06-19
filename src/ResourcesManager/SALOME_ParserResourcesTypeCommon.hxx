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
//  File   : SALOME_ParserResourcesTypeCommon.hxx
//  Author : Konstantin Leontev
//  Module : KERNEL
//$Header$
//
#ifndef SALOME_PARSER_RESOURCES_TYPE_COMMON
#define SALOME_PARSER_RESOURCES_TYPE_COMMON

#include "ResourcesManager_Defs.hxx"

#include <string>

#ifdef WIN32
#pragma warning(disable:4251) // Warning DLL Interface ...
#endif

enum AccessProtocolType { sh, rsh, ssh, srun, pbsdsh, blaunch, rsync };

enum BatchType { none, pbs, lsf, sge, ccc, ll, slurm, vishnu, oar, coorm, ssh_batch };

enum MpiImplType { nompi, lam, mpich1, mpich2, openmpi, ompi, slurmmpi, prun };

class RESOURCESMANAGER_EXPORT ResourcesException
{
public:
  const std::string msg;

  ResourcesException(const std::string m) : msg(m) {}
};

#endif //SALOME_PARSER_RESOURCES_TYPE_COMMON
