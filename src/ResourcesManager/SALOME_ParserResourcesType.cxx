// Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "SALOME_ParserResourcesType.hxx"

#include <map>

namespace
{
  const std::map <BatchType, std::string> batchTypeMap
  { 
    { none, "none" }, 
    { pbs, "pbs" },
    { lsf, "lsf" },
    { sge, "sge" },
    { ccc, "ccc" },
    { slurm, "slurm" },
    { ll, "ll" },
    { vishnu, "vishnu" },
    { oar, "oar" },
    { coorm, "coorm" },
    { ssh_batch, "ssh_batch" },
  };

  const std::map <AccessProtocolType, std::string> protocolTypeMap
  { 
    { sh, "sh" }, 
    { rsh, "rsh" },
    { ssh, "ssh" },
    { srun, "srun" },
    { pbsdsh, "pbsdsh" },
    { blaunch, "blaunch" },
    { rsync, "rsync" }
  };
}

ParserResourcesType::ParserResourcesType() :
  protocol(ssh),
  batch(none)
{
}

ParserResourcesType::~ParserResourcesType()
{
}

std::string ParserResourcesType::getAccessProtocolTypeStr() const
{
  return protocolToString(protocol);
}

void ParserResourcesType::setAccessProtocolTypeStr(const std::string& protocolStr)
{
  protocol = stringToProtocol(protocolStr);
}

std::string ParserResourcesType::getBatchTypeStr() const
{
  const auto it = batchTypeMap.find(batch);
  if (it == batchTypeMap.end())
  {
    throw ResourcesException("Unknown batch type: " + std::to_string(batch));
  }

  return it->second;
}

void ParserResourcesType::setBatchTypeStr(const std::string& batchTypeStr)
{
  if (batchTypeStr.empty())
  {
    batch = none;
    return;
  }

  for (const auto& batchType : batchTypeMap)
  {
    if (batchType.second == batchTypeStr)
    {
      batch = batchType.first;
      return;
    }
  }

  throw ResourcesException("Unknown batch type " + batchTypeStr);
}

std::string ParserResourcesType::protocolToString(AccessProtocolType protocolType)
{
  const auto it = protocolTypeMap.find(protocolType);
  if (it == protocolTypeMap.end())
  {
    throw ResourcesException("Unknown protocol type: " + std::to_string(protocolType));
  }

  return it->second;
}

AccessProtocolType ParserResourcesType::stringToProtocol(const std::string& protocolStr)
{
  // Use sh protocol by default
  if (protocolStr.empty())
  {
    return sh;
  }

  for (const auto& protocolType : protocolTypeMap)
  {
    if (protocolType.second == protocolStr)
    {
      return protocolType.first;
    }
  }

  throw ResourcesException("Unknown protocol " + protocolStr);
}

void ParserResourcesType::print(std::ostream& os) const
{
  os << "name: " << name << '\n' <<
        "hostname: " << hostname << '\n' <<
        "Protocol: " << getAccessProtocolTypeStr() << '\n' <<
        "batch: " << getBatchTypeStr() << '\n' <<
        "username: " << username << '\n' <<
        "applipath: " << applipath << '\n' <<

        // TODO: all the stuff below wasn't declared in idl/SALOME_Resource.idl file
        // TODO: decide if we need these members in the base class
        "batchQueue: " << batchQueue << '\n' <<
        "userCommands: " << userCommands << '\n' <<
        "use: " << use << '\n';
}
