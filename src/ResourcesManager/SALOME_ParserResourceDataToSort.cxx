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

#include "SALOME_ParserResourceDataToSort.hxx"

#include <iostream>

#define NULL_VALUE 0

unsigned int ResourceDataToSort::mem_mb_wanted = NULL_VALUE;
unsigned int ResourceDataToSort::cpu_clock_wanted = NULL_VALUE;
unsigned int ResourceDataToSort::nb_node_wanted = NULL_VALUE;
unsigned int ResourceDataToSort::nb_proc_per_node_wanted = NULL_VALUE;

unsigned int ResourceDataToSort::nb_proc_wanted = NULL_VALUE;


ResourceDataToSort::ResourceDataToSort()
{}

ResourceDataToSort::ResourceDataToSort(const std::string& name,
                                       unsigned int nb_node,
                                       unsigned int nb_proc_per_node,
                                       unsigned int cpu_clock,
                                       unsigned int mem_mb):
    name(name),
    nb_node(nb_node),
    nb_proc_per_node(nb_proc_per_node),
    cpu_clock(cpu_clock),
    mem_mb(mem_mb)
{}

//! Method used by list::sort to sort the resources used in SALOME_ResourcesManager::GetResourcesFitting
bool ResourceDataToSort::operator< (const ResourceDataToSort& other) const
{
  unsigned int nbPts = GetNumberOfPoints();
  return nbPts < other.GetNumberOfPoints();
}

unsigned int ResourceDataToSort::GetNumberOfPoints() const
{
  unsigned int ret = 0;

  //priority 0 : Nb of proc
  if (nb_proc_wanted != NULL_VALUE)
  {
    unsigned int nb_proc = nb_node * nb_proc_per_node;
    if (nb_proc == nb_proc_wanted)
      ret += 30000;
    else if (nb_proc > nb_proc_wanted)
      ret += 20000;
    else
      ret += 10000;
  }

  //priority 1 : Nb of nodes
  if (nb_node_wanted != NULL_VALUE)
  {
    if (nb_node == nb_node_wanted)
      ret += 3000;
    else if (nb_node > nb_node_wanted)
      ret += 2000;
    else
      ret += 1000;
  }

  //priority 2 : Nb of proc by node
  if (nb_proc_per_node_wanted != NULL_VALUE)
  {
    if (nb_proc_per_node == nb_proc_per_node_wanted)
      ret += 300;
    else if (nb_proc_per_node > nb_proc_per_node_wanted)
      ret += 200;
    else
      ret += 100;
  }

  //priority 3 : Cpu freq
  if (cpu_clock_wanted != NULL_VALUE)
  {
    if (cpu_clock == cpu_clock_wanted)
      ret += 30;
    else if (cpu_clock > cpu_clock_wanted)
      ret += 20;
    else
      ret += 10;
  }

  //priority 4 : memory
  if (mem_mb_wanted != NULL_VALUE)
  {
    if (mem_mb == mem_mb_wanted)
      ret += 3;
    else if (mem_mb > mem_mb_wanted)
      ret += 2;
    else
      ret += 1;
  }

  //RES_MESSAGE("[GetNumberOfPoints] points number for resource: " << name << " " << ret);
  return ret;
}

//! Method used for debug
void ResourceDataToSort::Print() const
{
  std::cout << "\nResourceDataToSort:\n" << 
    "name: " << name << '\n' <<
    "mem_mb: " << mem_mb << '\n' <<
    "cpu_clock: " << cpu_clock << '\n' <<
    "nb_node: " << nb_node << '\n' <<
    "nb_proc_per_node: " << nb_proc_per_node << '\n';
}
