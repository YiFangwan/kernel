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
//  File   : SALOME_Parameter.hxx
//  Author : Alexandre SOLOVYOV
//  Module : SALOME
//
#ifndef __SALOME_PARAMETER_H__
#define __SALOME_PARAMETER_H__

#include <SALOME_ParameterizedObject.hxx>

class SALOME_Parameter: public POA_SALOME::Parameter, public SALOME_ParameterizedObject
{
public:
  //! standard constructor
  SALOME_Parameter( bool val );
  SALOME_Parameter( int val );
  SALOME_Parameter( double val );
  SALOME_Parameter( const char* val );
  
  //! standard destructor
  virtual ~SALOME_Parameter();

  virtual char* GetEntry();

  virtual void SetValue( const char* expr );

  virtual SALOME::ParamType GetType();

  virtual char*          AsString();
  virtual CORBA::Long    AsInteger();
  virtual CORBA::Double  AsDouble();
  virtual CORBA::Boolean AsBoolean();

protected:
  virtual SALOME_Parameter* Calculate();
  virtual bool GetBoolean( const char* expr, bool& val );

private:
  int myInt;
  double myDouble;
  char* myStr;
  bool myBool;
  SALOME::ParamType myType;
};

#endif
