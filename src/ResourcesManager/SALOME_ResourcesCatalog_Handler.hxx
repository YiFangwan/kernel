// Copyright (C) 2007-2023  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  SALOME ResourcesCatalog : implementation of catalog resources parsing (SALOME_ModuleCatalog.idl)
//  File   : SALOME_ResourcesCatalog_Handler.hxx
//  Author : Estelle Deville
//  Module : SALOME
//$Header$
//
#ifndef SALOME_RESOURCES_CATALOG_HANDLER
#define SALOME_RESOURCES_CATALOG_HANDLER

#include "ResourcesManager_Defs.hxx"

#include "SALOME_ParserResourcesTypeJob.hxx"
#include "SALOME_ParserResourcesTypeContainer.hxx"

#include <libxml/parser.h>

class RESOURCESMANAGER_EXPORT SALOME_ResourcesCatalog_Handler
{
public :
  SALOME_ResourcesCatalog_Handler() = delete;
  SALOME_ResourcesCatalog_Handler(
    ParserResourcesTypeJob::TypeMap& resourcesJob,
    ParserResourcesTypeContainer::TypeMap& resourcesContainer
    );

  virtual ~SALOME_ResourcesCatalog_Handler();

  const ParserResourcesTypeJob::TypeMap& getResourcesJob() const;
  const ParserResourcesTypeContainer::TypeMap& getResourcesContainer() const;

  void readXmlDoc(xmlDocPtr theDoc);
  void writeXmlDoc(xmlDocPtr theDoc) const;

  static std::string readAttr(xmlNodePtr node, const xmlChar* attr);

protected:
  void readNodeResourcesJob(xmlNodePtr node);
  void readNodeResourcesContainer(xmlNodePtr node);

  bool readMachineJob(xmlNodePtr node, ParserResourcesTypeJob& resource) const;
  bool readMachineContainer(xmlNodePtr node, ParserResourcesTypeContainer& resource) const;

  void readComponent(xmlNodePtr node, ParserResourcesTypeContainer& resource) const;
  void readModule(xmlNodePtr node, ParserResourcesTypeContainer& resource) const;

  // void readNodeCluster(xmlNodePtr node);
  // bool readNodeClusterMember(xmlNodePtr node, ParserResourcesTypeContainer& resource);

  void writeNodeResourcesJob(xmlNodePtr node) const;
  void writeNodeResourcesContainer(xmlNodePtr node) const;

  void writeMachineJob(xmlNodePtr node, const ParserResourcesTypeJob& resource) const;
  void writeMachineContainer(xmlNodePtr node, const ParserResourcesTypeContainer& resource) const;

  void writeComponents(xmlNodePtr node, const ParserResourcesTypeContainer& resource) const;

  // bool ProcessCluster(xmlNodePtr cluster_descr, ParserResourcesType& resource);
  // bool ProcessMember(xmlNodePtr member_descr, ParserResourcesType& resource);

private :
  std::string previous_component_name;

  ParserResourcesTypeJob::TypeMap& resourcesJob;
  ParserResourcesTypeContainer::TypeMap& resourcesContainer;
};

#endif // SALOME_RESOURCES_CATALOG_HANDLER
