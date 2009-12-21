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
std::string arg( const std::string& theStr, const std::string& theArg1 );
std::string arg( const std::string& theStr, const std::string& theArg1, const std::string& theArg2 );
std::vector<std::string> split( const std::string& theData, const std::string& theSeparator, bool theIsKeepEmpty );
void save( FILE* theFile, const std::string& theData );
void save( FILE* theFile, int theData );
bool load( FILE* theFile, std::string& theData );
bool load( FILE* theFile, int& theData );



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
  virtual void RemoveObject( SALOME::ParameterizedObject_ptr theObject );
  virtual void Remove( const char* theParamName );
  virtual void Rename( const char* theOldName, const char* theNewName );
  virtual SALOME::Parameter_ptr GetParameter( const char* theParamName );
  virtual SALOME::StringArray* Parameters();
  virtual SALOME::StringArray* AbsentParameters( const char* theExpr );

  virtual SALOME::Parameter_ptr Calculate( const char* theExpr );

  virtual CORBA::Boolean       Save( const char* theFileName );
  virtual CORBA::Boolean       Load( const char* theFileName );
  virtual char*                DumpPython();
  virtual char*                Dump();
  virtual SALOME::StringArray* GetObjectParameters( const char* theComponent, const char* theEntry );
  virtual SALOME::StringArray* GetParameters( const char* theParamName );
  virtual SALOME::StringArray* GetAttributeParameters( const char* theStringAttribute );

  virtual SALOMEDS::Study_ptr  GetStudy();

  SALOME_Parameter* GetParameterPtr( const char* theParamName ) const;
  void              Update( bool theOnlyParameters );
  void              UpdateAnonymous( const std::string& theOldName, const std::string& theNewName );
  int               GetNewId();
  bool              HasDependency( const std::string& theObjKey, const std::string& theRefKey ) const;
  void              ParseOldStyleParam( const std::string& theName, const std::string& theType, const std::string& theValue );
  void              ParseOldStyleObject( const std::string& theComponent, const std::string& theEntry, const std::string& theData );
  void              RebuildLinks();

private:
  void AddParameter( SALOME_Parameter* theParam, bool theAddDependencies = true );
  void AddDependencies( SALOME_Parameter* theParam );
  void AddDependency( const std::string& theObjKey, const std::string& theRefKey );
  void ClearDependencies( const std::string& theObjKey, SALOME::DependenciesType theType );
  bool CheckParamName( const std::string& theParamName, std::string& theMsg ) const;
  SALOME::StringArray* GenerateList( const std::list<std::string>& theList ) const;
  std::string GetComponent( const std::string& theKey, std::string& theEntry ) const;
  void ParseDependencies( FILE* theFile, const std::string& theFirstLine );
  std::string GetKey( SALOME::ParameterizedObject_ptr theObj ) const;
  std::string GetKey( const std::string& theComponent, const std::string& theEntry ) const;
  std::string GetKey( const std::string& theParamName ) const;
  std::list<std::string> GetAllDependingOn( const std::string& theKey ) const;
  SALOME::ParameterizedObject_ptr FindObject( const std::string& theKey ) const;
  void ThrowError( const std::string& theErrorMsg ) const;
  bool CanUpdate( SALOME::ParameterizedObject_ptr theObj ) const;
  std::list<std::string> Parameters( bool theWithAnonymous ) const;

private:
  class KeyHelper
  {
  public:
    KeyHelper( const std::string& theKey, const SALOME_Notebook* theNotebook );
    std::string key() const;
    bool operator < ( const KeyHelper& theKH ) const;
    bool operator == ( const std::string& theKey ) const;

  private:
    std::string myKey;
    const SALOME_Notebook* myNotebook;
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
  SubstitutionInfo CreateSubstitution( const std::string& theName, const std::string& theExpr, bool theIsRename ) const;
  void AddSubstitution( const std::string& theName, const std::string& theExpr, bool theIsRename );
  bool Save( FILE* theFile, const SubstitutionInfo& theSubstitution ) const;
  SubstitutionInfo Load( FILE* theFile, const std::string& theFirstLine ) const;

private:
  std::map< std::string, std::list<std::string> > myDependencies;
  std::map< std::string, SALOME_Parameter* > myParameters;
  std::list< KeyHelper > myToUpdate;
  std::list<SubstitutionInfo> mySubstitutions;
  std::list< std::string > myEntriesToRebuild;
  SALOMEDS::Study_var myStudy;
  SALOME::Parameter_var myTmpParam;
  Utils_Mutex myMutex;
  bool myUpdateOnlyParameters;
  int myMaxId;
};

#endif
