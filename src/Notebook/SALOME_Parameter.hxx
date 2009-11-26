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

#include <SALOMEconfig.h>
#include <SALOME_EvalExpr.hxx>
#include <SALOME_EvalVariant.hxx>
#include CORBA_SERVER_HEADER(SALOME_Notebook)
#include <SALOME_GenericObj_i.hh>
#include <string>

class SALOME_Notebook;

const std::string PARAM_COMPONENT = "<params>";

class SALOME_Parameter : public virtual POA_SALOME::Parameter, public virtual SALOME::GenericObj_i
{
public:
  //! standard constructor
  SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theName, bool theValue );
  SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theName, int theValue );
  SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theName, double theValue );
  SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theName, const std::string& theData, bool isExpr );
  SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theExpr );
  
  //! standard destructor
  virtual ~SALOME_Parameter();

  virtual char* GetEntry();
  virtual char* GetComponent();
  virtual CORBA::Boolean IsValid();

  virtual void                 Update( SALOME::Notebook_ptr theNotebook );
  virtual void                 SetParameters( SALOME::Notebook_ptr theNotebook, const SALOME::StringArray& theParameters );
  virtual SALOME::StringArray* GetParameters();

  virtual void SetExpression( const char* theExpr );
  virtual void SetBoolean( CORBA::Boolean theValue );
  virtual void SetInteger( CORBA::Long theValue );
  virtual void SetReal( CORBA::Double theValue );
  virtual void SetString( const char* theValue );

  virtual SALOME::ParamType GetType();

  virtual char*          AsString();
  virtual CORBA::Long    AsInteger();
  virtual CORBA::Double  AsReal();
  virtual CORBA::Boolean AsBoolean();

  virtual char* GetExpression( CORBA::Boolean theForceConvert );

  void Save( FILE* theFile ) const;
  static SALOME_Parameter* Load( SALOME_Notebook* theNotebook, FILE* theFile, const std::string& theFirstLine );

  bool IsAnonymous() const;
  bool IsCalculable() const;

  SALOME_StringList Dependencies() const;
  void Substitute( const std::string& theName, const SALOME_EvalExpr& theExpr );
  int  GetId() const;

private:
  void ThrowTypeError( const std::string& theMsg );
  void ThrowError( bool isCalc, const std::string& theMsg );
  void InternalSetExpression( const std::string& theExpr );
  void AnalyzeError();
  void SetId();

private:
  SALOME_Notebook* myNotebook;
  std::string myName;
  SALOME_EvalExpr myExpr;
  SALOME_EvalVariant myResult;
  bool myIsAnonymous, myIsCalculable;
  int myId;
};

#endif
