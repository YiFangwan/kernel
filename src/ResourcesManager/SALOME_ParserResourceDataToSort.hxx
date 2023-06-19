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
//  File   : SALOME_ParserResourceDataToSort.hxx
//  Author : Konstantin Leontev
//  Module : KERNEL
//$Header$
//
#ifndef SALOME_PARSER_RESOURCES_DATA_SORT
#define SALOME_PARSER_RESOURCES_DATA_SORT

#include "ResourcesManager_Defs.hxx"

#include <string>

#ifdef WIN32
#pragma warning(disable:4251) // Warning DLL Interface ...
#endif

class RESOURCESMANAGER_EXPORT ResourceDataToSort
{
public:
  std::string name;

  // Same as in resource definintion in idl/SALOME_ResourceContainer.idl file
  unsigned int mem_mb = 0;
  unsigned int cpu_clock = 0;
  unsigned int nb_node = 1;
  unsigned int nb_proc_per_node = 1;
  
  static unsigned int mem_mb_wanted;
  static unsigned int cpu_clock_wanted;
  static unsigned int nb_node_wanted;
  static unsigned int nb_proc_per_node_wanted;

  // An additional memeber to those above
  static unsigned int nb_proc_wanted;

public:
  ResourceDataToSort();
  ResourceDataToSort(const std::string& name,
                      unsigned int nb_node,
                      unsigned int nb_proc_per_node,
                      unsigned int cpu_clock,
                      unsigned int mem_mb);
  bool operator< (const ResourceDataToSort& other) const;
  void Print() const;

private:
  unsigned int GetNumberOfPoints() const;
};

#endif //SALOME_PARSER_RESOURCES_DATA_SORT
