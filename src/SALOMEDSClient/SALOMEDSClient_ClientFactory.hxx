// Copyright (C) 2006  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
//  File   : SALOMEDSClient_ClientFactory.hxx
//  Author : Sergey RUIN
//  Module : SALOME
//  $Header:

#ifndef SALOMEDSClient_ClientFactory_HeaderFile
#define SALOMEDSClient_ClientFactory_HeaderFile

#include "SALOMEDSClient_definitions.hxx" 
#include "SALOMEDSClient_SObject.hxx"
#include "SALOMEDSClient_SComponent.hxx"
#include "SALOMEDSClient_Study.hxx"
#include "SALOMEDSClient_StudyManager.hxx"

#include <string>

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)

class ClientFactory {

public:
  /*!
   *  Returns a client SObject wrapper that corresponds %theSObject
   */
  static _PTR(SObject) SObject(SALOMEDS::SObject_ptr theSObject);

  /*!
   *  Returns a client SComponent wrapper that corresponds %theSComponent
   */
  static _PTR(SComponent) SComponent(SALOMEDS::SComponent_ptr theSComponent);

  /*!
   *  Returns a client Study wrapper that corresponds %theStudy
   */
  static _PTR(Study) Study(SALOMEDS::Study_ptr theStudy);

  /*!
   *  Returns a client StudyManager wrapper
   */
  static _PTR(StudyManager) StudyManager();

  /*!
   *  Creates and returns a client StudyManager wrapper
   */
  static _PTR(StudyManager) createStudyManager(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa);

  
};

#endif
