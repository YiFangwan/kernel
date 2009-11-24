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
//  File   : SALOME_Parameter.cxx
//  Author : Alexandre SOLOVYOV
//  Module : SALOME
//

#include <SALOME_Parameter.hxx>
#include <SALOME_Notebook.hxx>
#include <Utils_SALOME_Exception.hxx>
#include <utilities.h>
#include <stdio.h>

SALOME_Parameter::SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theName, bool theValue )
: myNotebook( theNotebook ), myName( theName ), myResult( theValue ), myIsAnonymous( false ), myIsCalculable( false )
{
}

SALOME_Parameter::SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theName, int theValue )
: myNotebook( theNotebook ), myName( theName ), myResult( theValue ), myIsAnonymous( false ), myIsCalculable( false )
{
}

SALOME_Parameter::SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theName, double theValue )
: myNotebook( theNotebook ), myName( theName ), myResult( theValue ), myIsAnonymous( false ), myIsCalculable( false )
{
}

SALOME_Parameter::SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theName, const std::string& theData, bool isExpr )
: myNotebook( theNotebook ), myName( theName ), myIsAnonymous( false ), myIsCalculable( isExpr )
{
  if( isExpr )
  {
    myExpr.setExpression( theData );
    Update( SALOME::Notebook::_nil() );
  }
  else
    myResult = theData;
}

SALOME_Parameter::SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theExpr )
: myNotebook( theNotebook ), myName( theExpr ), myExpr( theExpr ), myIsAnonymous( true ), myIsCalculable( true )
{
  Update( SALOME::Notebook::_nil() );
}

SALOME_Parameter::~SALOME_Parameter()
{
}

char* SALOME_Parameter::GetEntry()
{
  return CORBA::string_dup( myName.c_str() );
}

char* SALOME_Parameter::GetComponent()
{
  return CORBA::string_dup( PARAM_COMPONENT.c_str() );
}

CORBA::Boolean SALOME_Parameter::IsValid()
{
  return myResult.isValid();
}

void SALOME_Parameter::SetParameters( SALOME::Notebook_ptr /*theNotebook*/, const SALOME::StringArray& /*theParameters*/ )
{
}

void SALOME_Parameter::Update( SALOME::Notebook_ptr /*theNotebook*/ )
{
  //printf( "Update of %s\n", GetEntry() );
  if( myIsCalculable )
  {
    //1. Set parameters values
    SALOME_StringList deps = myExpr.parser()->parameters();
    SALOME_StringList::const_iterator it = deps.begin(), last = deps.end();
    for( ; it!=last; it++ )
    {
      std::string aName = *it;
      SALOME_Parameter* aParam = myNotebook->GetParameterPtr( const_cast<char*>( aName.c_str() ) );
      if( aParam )
      {
        //printf( "\tset %s = %lf\n", aName.c_str(), aParam->AsReal() );
        myExpr.parser()->setParameter( aName, aParam->myResult );
      }
      else
      {
        myResult = SALOME_EvalVariant();
        return;
      }
    }

    //2. Calculate
    myResult = myExpr.calculate();
    //printf( "\tresult = %lf\n", AsReal() );
  }
}

void SALOME_Parameter::SetExpression( const char* theExpr )
{
  if( myIsAnonymous )
    myNotebook->AddExpression( theExpr );

  else
  {
    myExpr.setExpression( theExpr );
    myIsCalculable = true;
    myNotebook->SetToUpdate( _this() );
  }
}

void SALOME_Parameter::SetBoolean( CORBA::Boolean theValue )
{
  if( myIsAnonymous )
  {
  }
  else
  {
    myResult = theValue;
    myIsCalculable = false;
    myNotebook->SetToUpdate( _this() );
  }
}

void SALOME_Parameter::SetInteger( CORBA::Long theValue )
{
  if( myIsAnonymous )
  {
  }
  else
  {
    myResult = (int)theValue;
    myIsCalculable = false;
    myNotebook->SetToUpdate( _this() );
  }
}

void SALOME_Parameter::SetReal( CORBA::Double theValue )
{
  if( myIsAnonymous )
  {
  }
  else
  {
    myResult = theValue;
    myIsCalculable = false;
    myNotebook->SetToUpdate( _this() );
  }
}

void SALOME_Parameter::SetString( const char* theValue )
{
  if( myIsAnonymous )
  {
  }
  else
  {
    myResult = theValue;
    myIsCalculable = false;
    myNotebook->SetToUpdate( _this() );
  }
}

SALOME::ParamType SALOME_Parameter::GetType()
{
  switch( myResult.type() )
  {
  case SALOME_EvalVariant_Boolean:
    return SALOME::TBoolean;

  case SALOME_EvalVariant_Int:
  case SALOME_EvalVariant_UInt:
    return SALOME::TInteger;

  case SALOME_EvalVariant_Double:
    return SALOME::TReal;

  case SALOME_EvalVariant_String:
    return SALOME::TString;

  default:
    return SALOME::TUnknown;
  }
}

char* SALOME_Parameter::AsString()
{
  return CORBA::string_dup( myResult.toString().c_str() );
}

CORBA::Long SALOME_Parameter::AsInteger()
{
  bool ok;
  return myResult.toInt( &ok );
}

CORBA::Double SALOME_Parameter::AsReal()
{
  bool ok;
  return myResult.toDouble( &ok );
}

CORBA::Boolean SALOME_Parameter::AsBoolean()
{
  return myResult.toBool();
}

SALOME_StringList SALOME_Parameter::Dependencies() const
{
  return myIsCalculable ? myExpr.parser()->parameters() : SALOME_StringList();
}

std::string SALOME_Parameter::Save() const
{
  char buf[256];
  sprintf( buf, "%s %i %i?", myName.c_str(), (int)myIsAnonymous, (int)myIsCalculable );
  std::string aRes = buf;
  if( myIsCalculable )
    aRes += myExpr.expression();
  else
  {
    sprintf( buf, "%i %s", myResult.type(), myResult.toString().c_str() );
    aRes += buf;
  }
  return aRes;
}

SALOME_Parameter* SALOME_Parameter::Load( const std::string& theData )
{
  std::vector<std::string> aParts = SALOME_Notebook::Split( theData, "?", false );
  if( aParts.size() != 2 )
    return 0;

  char aName[256];
  int anAnonym, aCalc;
  if( sscanf( aParts[0].c_str(), "%s %i %i", aName, &anAnonym, &aCalc ) < 3 )
    return 0;

  printf( "load param: %s\n", aName );
  return 0;
}

bool SALOME_Parameter::IsAnonymous() const
{
  return myIsAnonymous;
}

bool SALOME_Parameter::IsCalculable() const
{
  return myIsCalculable;
}

std::string SALOME_Parameter::Expression() const
{
  return myExpr.expression();
}

void SALOME_Parameter::Substitute( const std::string& theName, const SALOME_EvalExpr& theExpr )
{
  if( !IsCalculable() )
    return;
  
  myExpr.substitute( theName, theExpr );
  if( IsAnonymous() )
  {
    std::string anOldName = myName;
    myName = myExpr.expression();
    myNotebook->UpdateAnonymous( anOldName.c_str(), this );
  }
}
