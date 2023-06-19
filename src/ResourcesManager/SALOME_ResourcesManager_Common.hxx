// Copyright (C) 2007-2023  CEA, EDF, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef __SALOME_RESOURCESMANAGER_COMMON_HXX__
#define __SALOME_RESOURCESMANAGER_COMMON_HXX__

#ifdef WIN32
# if defined SALOMERESOURCESMANAGER_EXPORTS || defined SalomeResourcesManager_EXPORTS
#  define SALOMERESOURCESMANAGER_EXPORT __declspec( dllexport )
# else
#  define SALOMERESOURCESMANAGER_EXPORT __declspec( dllimport )
# endif
#else
# define SALOMERESOURCESMANAGER_EXPORT
#endif

#include <SALOMEconfig.h>
// #include CORBA_CLIENT_HEADER(SALOME_Resource)
// #include CORBA_CLIENT_HEADER(SALOME_ResourceJob)
// #include CORBA_CLIENT_HEADER(SALOME_ResourceContainer)
#include CORBA_CLIENT_HEADER(SALOME_ResourcesManager)

#include "ResourcesManager.hxx"

// Functions for CPP <-> CORBA conversions
// All those functions are thread-safe
SALOMERESOURCESMANAGER_EXPORT resourceParamsJob resourceParametersJob_CORBAtoCPP(const Engines::ResourceParametersJob& params);
SALOMERESOURCESMANAGER_EXPORT resourceParamsContainer resourceParametersContainer_CORBAtoCPP(const Engines::ResourceParametersContainer& params);
SALOMERESOURCESMANAGER_EXPORT Engines::ResourceParametersJob_var resourceParametersJob_CPPtoCORBA(const resourceParamsJob& params);
SALOMERESOURCESMANAGER_EXPORT Engines::ResourceParametersContainer_var resourceParametersContainer_CPPtoCORBA(const resourceParamsContainer& params);

SALOMERESOURCESMANAGER_EXPORT ResourceList resourceList_CORBAtoCPP(const Engines::ResourceList& resList);
SALOMERESOURCESMANAGER_EXPORT Engines::ResourceList_var resourceList_CPPtoCORBA(const ResourceList& resList);

SALOMERESOURCESMANAGER_EXPORT ParserResourcesTypeJob resourceDefinitionJob_CORBAtoCPP(const Engines::ResourceDefinitionJob& resDef);
SALOMERESOURCESMANAGER_EXPORT ParserResourcesTypeContainer resourceDefinitionContainer_CORBAtoCPP(const Engines::ResourceDefinitionContainer& resDef);
SALOMERESOURCESMANAGER_EXPORT Engines::ResourceDefinitionJob_var resourceDefinitionJob_CPPtoCORBA(const ParserResourcesTypeJob& resource);
SALOMERESOURCESMANAGER_EXPORT Engines::ResourceDefinitionContainer_var resourceDefinitionContainer_CPPtoCORBA(const ParserResourcesTypeContainer& resource);
#endif
