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

#include "SALOME_ParserResourcesTypeJob.hxx"

namespace
{
  const std::map <MpiImplType, std::string> mpiImplTypeMap
  { 
    { nompi, "no mpiImpl" }, 
    { lam, "lam" },
    { mpich1, "mpich1" },
    { mpich2, "mpich2" },
    { openmpi, "openmpi" },
    { ompi, "ompi" },
    { slurmmpi, "slurmmpi" },
    { prun, "prun" }
  };
}

ParserResourcesTypeJob::ParserResourcesTypeJob()
: ParserResourcesType(),
  iprotocol(ssh),
  mpiImpl(nompi)
{
}

ParserResourcesTypeJob::~ParserResourcesTypeJob()
{
}

std::string ParserResourcesTypeJob::getMpiImplTypeStr() const
{
  const auto it = mpiImplTypeMap.find(mpiImpl);
  if (it == mpiImplTypeMap.end())
  {
    throw ResourcesException("Unknown MPI implementation type");
  }

  return it->second;
}

void ParserResourcesTypeJob::setMpiImplTypeStr(const std::string& mpiImplTypeStr)
{
  // Consider an empty string as nompi type
  if (mpiImplTypeStr.empty())
  {
    mpiImpl = nompi;
    return;
  }

  for (const auto& mpi : mpiImplTypeMap)
  {
    if (mpi.second == mpiImplTypeStr)
    {
      mpiImpl = mpi.first;
      return;
    }
  }

  throw ResourcesException("Unknown MPI implementation type " + mpiImplTypeStr);
}

std::string ParserResourcesTypeJob::getClusterInternalProtocolStr() const
{
  return protocolToString(iprotocol);
}


void ParserResourcesTypeJob::setClusterInternalProtocolStr(const std::string& internalProtocolTypeStr)
{
  iprotocol = stringToProtocol(internalProtocolTypeStr);
}

void ParserResourcesTypeJob::print(std::ostream& os) const
{
  ParserResourcesType::print(os);

  os << "iprotocol: " << getClusterInternalProtocolStr() << '\n' <<
        "mpiImpl: " << getMpiImplTypeStr() << '\n' <<
        "working_directory: " << working_directory << '\n';
}

std::ostream& operator<<(std::ostream& os, const ParserResourcesTypeJob& prt)
{
  prt.print(os);

  return os;
}
