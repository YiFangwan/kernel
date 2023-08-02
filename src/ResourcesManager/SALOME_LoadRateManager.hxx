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
// See https://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef __SALOME_LOADRATEMANAGER_HXX__
#define __SALOME_LOADRATEMANAGER_HXX__

#include "ResourcesManager_Defs.hxx"
#include "SALOME_ParserResourcesTypeContainer.hxx"

class RESOURCESMANAGER_EXPORT LoadRateManager
{
  public:
    virtual std::string Find(const std::vector<std::string>& /*hosts*/, const ParserResourcesTypeContainer::TypeMap& /*resList*/) { return ""; }
};

class RESOURCESMANAGER_EXPORT LoadRateManagerFirst : public LoadRateManager
{
  public:
    virtual std::string Find(const std::vector<std::string>& hosts,
                             const ParserResourcesTypeContainer::TypeMap& resList);
};

class RESOURCESMANAGER_EXPORT LoadRateManagerCycl : public LoadRateManager
{
  public:
    virtual std::string Find(const std::vector<std::string>& hosts,
                             const ParserResourcesTypeContainer::TypeMap& resList);
};

class RESOURCESMANAGER_EXPORT LoadRateManagerAltCycl : public LoadRateManager
{
  public:
    virtual std::string Find(const std::vector<std::string>& hosts,
                             const ParserResourcesTypeContainer::TypeMap& resList);
  protected:
    std::map<std::string,int> _numberOfUses;
};

#endif
