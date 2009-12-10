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
#include <SALOME_EvalParser.hxx>
#include <utilities.h>
#include <stdio.h>

SALOME_Parameter::SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theName, bool theValue )
: myNotebook( theNotebook ), myName( theName ), myResult( theValue ), myIsAnonymous( false ), myIsCalculable( false )
{
  SetId();
}

SALOME_Parameter::SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theName, int theValue )
: myNotebook( theNotebook ), myName( theName ), myResult( theValue ), myIsAnonymous( false ), myIsCalculable( false )
{
  SetId();
}

SALOME_Parameter::SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theName, double theValue )
: myNotebook( theNotebook ), myName( theName ), myResult( theValue ), myIsAnonymous( false ), myIsCalculable( false )
{
  SetId();
}

SALOME_Parameter::SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theName, const std::string& theData, bool isExpr )
: myNotebook( theNotebook ), myName( theName ), myIsAnonymous( false ), myIsCalculable( isExpr )
{
  if( isExpr )
    InternalSetExpression( theData, false );
  else
    myResult = theData;
}

SALOME_Parameter::SALOME_Parameter( SALOME_Notebook* theNotebook, const std::string& theExpr )
: myNotebook( theNotebook ), myName( theExpr ), myExpr( theExpr ), myIsAnonymous( true ), myIsCalculable( true )
{
  SetId();
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

SALOME::StringArray* SALOME_Parameter::GetParameters()
{
  return 0;
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
        //printf( "\tset %s = %lf\n", aName.c_str(), aParam->AsReal() );
        myExpr.parser()->setParameter( aName, aParam->myResult );
      else
        ThrowError( false, arg( "Parameter '%1' is not declared yet", aName ) );
    }

    //2. Calculate
    myResult = myExpr.calculate();
    AnalyzeError();
    //printf( "\tresult = %lf\n", AsReal() );
  }
}

void SALOME_Parameter::Update()
{
  myNotebook->SetToUpdate( _this() );
  myNotebook->Update( true );
}

void SALOME_Parameter::SetExpression( const char* theExpr )
{
  if( myIsAnonymous )
    myNotebook->AddExpression( theExpr );
  else
    InternalSetExpression( theExpr, true );
}

void SALOME_Parameter::SetBoolean( CORBA::Boolean theValue )
{
  if( !myIsAnonymous )
  {
    myResult = theValue;
    myIsCalculable = false;
    Update();
  }
}

void SALOME_Parameter::SetInteger( CORBA::Long theValue )
{
  if( !myIsAnonymous )
  {
    myResult = (int)theValue;
    myIsCalculable = false;
    Update();
  }
}

void SALOME_Parameter::SetReal( CORBA::Double theValue )
{
  if( !myIsAnonymous )
  {
    myResult = theValue;
    myIsCalculable = false;
    Update();
  }
}

void SALOME_Parameter::SetString( const char* theValue )
{
  if( !myIsAnonymous )
  {
    myResult = theValue;
    myIsCalculable = false;
    Update();
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
  int aRes = myResult.toInt( &ok );
  if( !ok )
    ThrowTypeError( "SALOME_Parameter::AsInteger(): " );
  return aRes;
}

CORBA::Double SALOME_Parameter::AsReal()
{
  bool ok;
  double aRes = myResult.toDouble( &ok );
  if( !ok )
    ThrowTypeError( "SALOME_Parameter::AsReal(): " );
  return aRes;
}

CORBA::Boolean SALOME_Parameter::AsBoolean()
{
  return myResult.toBool();
}

SALOME_StringList SALOME_Parameter::Dependencies() const
{
  return myIsCalculable ? myExpr.parser()->parameters() : SALOME_StringList();
}

void SALOME_Parameter::Save( FILE* theFile ) const
{
  save( theFile, myName );
  save( theFile, myIsAnonymous );
  save( theFile, myIsCalculable );
  if( myIsCalculable )
    save( theFile, myExpr.expression() );
  else
  {
    save( theFile, (int)myResult.type() );
    save( theFile, myResult.toString() );
  }
}

SALOME_Parameter* SALOME_Parameter::Load( SALOME_Notebook* theNotebook, FILE* theFile, const std::string& theFirstLine )
{
  std::string aName = theFirstLine;
  int isAnonymous, isCalculable;
  load( theFile, isAnonymous );
  load( theFile, isCalculable );
  if( isCalculable )
  {
    std::string anExpr;
    load( theFile, anExpr );
    return isAnonymous ? new SALOME_Parameter( theNotebook, anExpr ) : new SALOME_Parameter( theNotebook, aName, anExpr, true );
  }
  else
  {
    int aResType;
    std::string aValue;
    load( theFile, aResType );
    load( theFile, aValue );
    switch( aResType )
    {
    case SALOME_EvalVariant_Boolean:
      return new SALOME_Parameter( theNotebook, aName, aValue=="true" );

    case SALOME_EvalVariant_Int:
    case SALOME_EvalVariant_UInt:
    {
      int value;
      if( sscanf( aValue.c_str(), "%i", &value ) == 1 )
        return new SALOME_Parameter( theNotebook, aName, value );
    }

    case SALOME_EvalVariant_Double:
    {
      double value;
      if( sscanf( aValue.c_str(), "%lf", &value ) == 1 )
        return new SALOME_Parameter( theNotebook, aName, value );
    }

    case SALOME_EvalVariant_String:
      return new SALOME_Parameter( theNotebook, aName, aValue, false );
    }
  }
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

char* SALOME_Parameter::GetExpression( CORBA::Boolean theForceConvert )
{
  const char* aRes;
  if( IsCalculable() )
    aRes = myExpr.expression().c_str();
  else
    aRes = theForceConvert ? myResult.toString().c_str() : "";
  return CORBA::string_dup( aRes );
}

void SALOME_Parameter::Substitute( const std::string& theName, const SALOME_EvalExpr& theExpr )
{
  if( myName == theName )
  {
    myName = theExpr.expression();
    return;
  }

  if( !IsCalculable() )
    return;

  myExpr.substitute( theName, theExpr );
  if( IsAnonymous() )
  {
    std::string aNewName = myExpr.expression();
    myNotebook->UpdateAnonymous( myName, aNewName );
    myName = aNewName;
  }
}

void SALOME_Parameter::ThrowTypeError( const std::string& theMsg )
{
  std::string aMsg = theMsg;
  aMsg += "type is ";
  switch( GetType() )
  {
  case SALOME::TBoolean:
    aMsg += "boolean";
    break;
  case SALOME::TInteger:
    aMsg += "integer";
    break;
  case SALOME::TReal:
    aMsg += "real";
    break;
  case SALOME::TString:
    aMsg += "string";
    break;
  default:
    aMsg += "unknown";
    break;
  }

  SALOME::TypeError anError;
  anError.Reason = CORBA::string_dup( aMsg.c_str() );
  throw anError;
}

void SALOME_Parameter::InternalSetExpression( const std::string& theExpr, bool isUpdateByNotebook )
{
  std::string anOldExpr = myExpr.expression();
  bool anOldCalc = myIsCalculable;
  try
  {
    myExpr.setExpression( theExpr );
    myIsCalculable = true;
    AnalyzeError();
    if( isUpdateByNotebook )
      Update();
    else
      Update( SALOME::Notebook::_nil() );
    SetId();
  }
  catch( SALOME::ExpressionError anError )
  {
    myExpr.setExpression( anOldExpr );
    myIsCalculable = anOldCalc;
    Update();
    throw anError;
  }
}

void SALOME_Parameter::AnalyzeError()
{
  if( !myIsCalculable )
    return;

  std::string aMsg;
  SALOME_EvalExprError anError = myExpr.parser()->error();
  bool isCalcError = true;
  switch( anError )
  {
  case EvalExpr_OperandsNotMatch:
    aMsg = "Types of arguments are invalid";
    break;
  case EvalExpr_InvalidResult:
    aMsg = "Invalid result";
    break;
  case EvalExpr_InvalidOperation:
    aMsg = "Invalid operation";
    isCalcError = false;
    break;
  case EvalExpr_OperationsNull:
    aMsg = "Internal error";
    isCalcError = false;
    break;
  case EvalExpr_InvalidToken:
    aMsg = "Invalid token";
    isCalcError = false;
    break;
  case EvalExpr_CloseExpected:
    aMsg = "A closing bracket is expecting";
    isCalcError = false;
    break;
  case EvalExpr_ExcessClose:
    aMsg = "There is an excess closing bracket";
    isCalcError = false;
    break;
  case EvalExpr_BracketsNotMatch:
    aMsg = "Brackets of different types";
    isCalcError = false;
    break;
  case EvalExpr_StackUnderflow:
    aMsg = "Stack underflow (missing arguments of operation?)";
    isCalcError = false;
    break;
  case EvalExpr_ExcessData:
    aMsg = "Several expressions";
    isCalcError = false;
    break;
  }

  if( aMsg.length() > 0 )
    ThrowError( isCalcError, aMsg );
}

void SALOME_Parameter::ThrowError( bool isCalc, const std::string& theMsg )
{
  if( isCalc )
  {
    SALOME::CalculationError anError;
    anError.Reason = CORBA::string_dup( theMsg.c_str() );
    throw anError;
  }
  else
  {
    SALOME::ExpressionError anError;
    anError.Reason = CORBA::string_dup( theMsg.c_str() );
    throw anError;
  }
}

void SALOME_Parameter::SetId()
{
  myId = myNotebook->GetNewId();
}

int SALOME_Parameter::GetId() const
{
  return myId;
}
