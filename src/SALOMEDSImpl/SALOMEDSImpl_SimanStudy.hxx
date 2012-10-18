// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

//  File   : SALOMEDSImpl_SimanStudy.hxx
//  Author : Mikhail PONIKAROV
//  Module : SALOME
//
#ifndef __SALOMEDSIMPL_SIMANSTUDY_I_H__
#define __SALOMEDSIMPL_SIMANSTUDY_I_H__

// std C++ headers
#include <iostream>
#include <string>

//SALOMEDSImpl headers
#include "SALOMEDSImpl_Defines.hxx"
#include "SALOMEDSImpl_Study.hxx"

class SimanIO_Configuration;

class SALOMEDSIMPL_EXPORT SALOMEDSImpl_SimanStudy
{
private:
  SALOMEDSImpl_Study*      _study; // referenced study
  std::string              _studyId; // SIMAN ID
  std::string              _scenarioId; // SIMAN ID
  std::string              _userId; // SIMAN ID
  
  SimanIO_Configuration* _checkedOut; // pointer to information about checked out data
  std::map<int, std::map<std::string, int> > _filesId; // map from document id to component identifiers to SIMAN file ID

public:

  virtual void CheckOut(SALOMEDSImpl_Study* theTarget);
  virtual void CheckIn(const std::string theModuleName);
  virtual SALOMEDSImpl_Study* getReferencedStudy();
  virtual std::string StudyId();
  virtual void StudyId(const std::string theId);
  virtual std::string ScenarioId();
  virtual void ScenarioId(const std::string theId);
  virtual std::string UserId();
  virtual void UserId(const std::string theId);

  //! standard constructor
  SALOMEDSImpl_SimanStudy();
  
  //! standard destructor
  virtual ~SALOMEDSImpl_SimanStudy(); 
};
#endif
