//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : SALOME_ParameterizedObject.hxx
//  Author : Alexandre SOLOVYOV
//  Module : SALOME
//
#ifndef __SALOME_PARAMETERIZED_OBJECT_H__
#define __SALOME_PARAMETERIZED_OBJECT_H__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Notebook)
#include <SALOME_GenericObj_i.hh>
#include <list>

class SALOME_ParameterizedObject: public POA_SALOME::ParameterizedObject, public SALOME::GenericObj_i
{
public:
  //! standard constructor
  SALOME_ParameterizedObject();
  
  //! standard destructor
  virtual ~SALOME_ParameterizedObject(); 

  
  //return object's entry
  virtual char* GetEntry() = 0;

  //validity status
  virtual CORBA::Boolean IsValid();

  //update management
  virtual void SetToUpdate();
  virtual void Update();

protected:
  void SetIsValid( bool is_valid );
  bool IsToUpdate();
  void ClearToUpdate();

private:
  bool myIsValid, myToUpdate;
};

#endif
