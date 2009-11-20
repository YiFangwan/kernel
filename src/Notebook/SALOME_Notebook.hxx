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
//  File   : SALOME_Notebook.hxx
//  Author : Alexandre SOLOVYOV
//  Module : SALOME
//
#ifndef __SALOME_NOTEBOOK_H__
#define __SALOME_NOTEBOOK_H__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Notebook)
#include CORBA_SERVER_HEADER(SALOMEDS)
#include <SALOME_GenericObj_i.hh>
#include <string>
#include <list>
#include <map>

class SALOME_Parameter;

class SALOME_Notebook : public virtual POA_SALOME::Notebook, public virtual SALOME::GenericObj_i
{
public:
  SALOME_Notebook( PortableServer::POA_ptr thePOA, SALOMEDS::Study_ptr theStudy );

  virtual CORBA::Boolean AddDependency( SALOME::ParameterizedObject_ptr theObj, SALOME::ParameterizedObject_ptr theRef );
  virtual void RemoveDependency( SALOME::ParameterizedObject_ptr theObj, SALOME::ParameterizedObject_ptr theRef );
  virtual void ClearDependencies( SALOME::ParameterizedObject_ptr theObj );
  virtual void SetToUpdate( SALOME::ParameterizedObject_ptr theObj );
  virtual void Update();

  virtual CORBA::Boolean AddExpr( const char* theExpr );
  virtual CORBA::Boolean AddNameExpr( const char* theName, const char* theExpr );
  virtual CORBA::Boolean AddValue( const char* theName, CORBA::Double theValue );
  virtual void Remove( const char* theParamName );
  virtual SALOME::Parameter_ptr Param( const char* theParamName );
  virtual SALOME::StringList* Params();

  virtual void Save( const char* theFileName );
  virtual CORBA::Boolean Load( const char* theFileName );

  SALOME_Parameter* ParamPtr( const char* theParamName ) const;

protected:
  bool AddParam( SALOME_Parameter* theParam );
  bool AddDependencies( SALOME_Parameter* theParam );
  bool AddDependency( const std::string& theObjKey, const std::string& theRefKey );
  void ClearDependencies( const std::string& theObjKey );
  bool CheckParamName( const std::string& theParamName ) const;

private:
  std::string GetKey( SALOME::ParameterizedObject_ptr theObj );
  std::string GetKey( const std::string& theParamName );

  //! return list of objects that depend on object with given key
  std::list<std::string> GetAllDependingOn( const std::string& theKey );
  SALOME::ParameterizedObject_ptr FindObject( const std::string& theKey );

private:
  std::map< std::string, std::list<std::string> > myDeps;
  std::map< std::string, SALOME_Parameter* > myParams;

  friend class KeyHelper;
  class KeyHelper
  {
  public:
    KeyHelper( const std::string& theKey, SALOME_Notebook* theNotebook );
    std::string key() const;
    bool operator < ( const KeyHelper& theKH ) const;
    bool operator == ( const std::string& theKey ) const;

  private:
    std::string myKey;
    SALOME_Notebook* myNotebook;
  };
  std::list< KeyHelper > myToUpdate;

  SALOMEDS::Study_var myStudy;
};

#endif
