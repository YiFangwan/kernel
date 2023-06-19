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

#include "SALOME_ParserResourcesTypeContainer.hxx"

ParserResourcesTypeContainer::ParserResourcesTypeContainer()
: ParserResourcesType()
{
}

ParserResourcesTypeContainer::~ParserResourcesTypeContainer()
{
}

void ParserResourcesTypeContainer::print(std::ostream& os) const
{
  ParserResourcesType::print(os);

  os << "nb_node: " << dataForSort.nb_node << '\n' <<
        "nb_proc_per_node: " << dataForSort.nb_proc_per_node << '\n' <<
        "cpu_clock: " << dataForSort.cpu_clock << '\n' <<
        "mem_mb: " << dataForSort.mem_mb << '\n' <<
        "OS: " << OS << '\n';

  for(unsigned int i = 0 ; i < componentList.size(); i++)
    os << "Component " << i + 1 << " called: " << componentList[i] << '\n';

  // TODO: all the stuff below wasn't declared in idl/SALOME_Resource.idl file
  std::list<ParserResourcesTypeContainer>::const_iterator it;
  for(it = ClusterMembersList.begin() ; it != ClusterMembersList.end() ; it++)
  {
    os << "Cluster member called: " << (*it).hostname << '\n';
  }

  os << "NbOfProc: " << nbOfProc << '\n';

  // TODO: do we need to ptint the ModulesList?
}

std::ostream& operator<<(std::ostream& os, const ParserResourcesTypeContainer& prt)
{
  prt.print(os);

  return os;
}
