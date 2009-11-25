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
#include <Utils_Mutex.hxx>
#include <string>
#include <list>
#include <map>
#include <vector>

class SALOME_Parameter;

class SALOME_Notebook : public virtual POA_SALOME::Notebook, public virtual SALOME::GenericObj_i
{
public:
  SALOME_Notebook( PortableServer::POA_ptr thePOA, SALOMEDS::Study_ptr theStudy );

  virtual void AddDependency( SALOME::ParameterizedObject_ptr theObj, SALOME::ParameterizedObject_ptr theRef );
  virtual void RemoveDependency( SALOME::ParameterizedObject_ptr theObj, SALOME::ParameterizedObject_ptr theRef );
  virtual void ClearDependencies( SALOME::ParameterizedObject_ptr theObj, SALOME::DependenciesType theType );
  virtual void SetToUpdate( SALOME::ParameterizedObject_ptr theObj );
  virtual void Update();

  virtual void AddExpression( const char* theExpr );
  virtual void AddNamedExpression( const char* theName, const char* theExpr );
  virtual void AddBoolean( const char* theName, CORBA::Boolean theValue );
  virtual void AddInteger( const char* theName, CORBA::Long theValue );
  virtual void AddReal( const char* theName, CORBA::Double theValue );
  virtual void AddString( const char* theName, const char* theValue );
  virtual void Remove( const char* theParamName );
  virtual void Rename( const char* theOldName, const char* theNewName );
  virtual SALOME::Parameter_ptr GetParameter( const char* theParamName );
  virtual SALOME::StringArray* Parameters();
  virtual SALOME::StringArray* AbsentParameters();

  virtual CORBA::Boolean Save( const char* theFileName );
  virtual CORBA::Boolean Load( const char* theFileName );
  virtual char*          DumpPython();
  virtual char*          Dump();

  SALOME_Parameter* GetParameterPtr( const char* theParamName ) const;
  void UpdateAnonymous( const std::string& theOldName, const std::string& theNewName );

  static std::vector<std::string> Split( const std::string& theData, const std::string& theSeparator, bool theIsKeepEmpty );

protected:
  void AddParameter( SALOME_Parameter* theParam );
  void AddDependencies( SALOME_Parameter* theParam );
  void AddDependency( const std::string& theObjKey, const std::string& theRefKey );
  void ClearDependencies( const std::string& theObjKey, SALOME::DependenciesType theType );
  bool CheckParamName( const std::string& theParamName, std::string& theMsg ) const;
  SALOME::StringArray* GenerateList( const std::list<std::string>& theList ) const;
  std::string GetComponent( const std::string& theKey, std::string& theEntry ) const;
  void ParseDependencies( const std::string& theData );

private:
  std::string GetKey( SALOME::ParameterizedObject_ptr theObj );
  std::string GetKey( const std::string& theParamName );
  std::list<std::string> GetAllDependingOn( const std::string& theKey );
  SALOME::ParameterizedObject_ptr FindObject( const std::string& theKey );
  void throwError( const std::string& theErrorMsg );

private:
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

  typedef struct
  {
    std::string myName, myExpr;
    std::list< KeyHelper > myObjects;
    bool myIsRename;
  } SubstitutionInfo;

private:
  void Sort( std::list< KeyHelper >& theList ) const;
  bool Substitute( SubstitutionInfo& theSubstitution );
  SubstitutionInfo CreateSubstitution( const std::string& theName, const std::string& theExpr, bool theIsRename );
  void AddSubstitution( const std::string& theName, const std::string& theExpr, bool theIsRename );

private:
  std::map< std::string, std::list<std::string> > myDependencies;
  std::map< std::string, SALOME_Parameter* > myParameters;
  std::list< KeyHelper > myToUpdate;
  std::list<SubstitutionInfo> mySubstitutions;
  SALOMEDS::Study_var myStudy;
  Utils_Mutex myMutex;
};

#endif
