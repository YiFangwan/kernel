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
#include <Utils_SALOME_Exception.hxx>
#include <utilities.h>
#include <stdio.h>

SALOME_Parameter::SALOME_Parameter( bool val )
{
  myStr = 0;
  myBool = val;
  myType = SALOME::TBoolean;
}

SALOME_Parameter::SALOME_Parameter( int val )
{
  myStr = 0;
  myInt = val;
  myType = SALOME::TInteger;
}

SALOME_Parameter::SALOME_Parameter( double val )
{
  myStr = 0;
  myDouble = val;
  myType = SALOME::TReal;
}

SALOME_Parameter::SALOME_Parameter( const char* val )
{
  SetValue( val );
}

SALOME_Parameter::~SALOME_Parameter()
{
  free( myStr );
}

bool SALOME_Parameter::GetBoolean( const char* expr, bool& val )
{
  bool res = true;
  if( strcmp( expr, "true" )==0 || strcmp( expr, "TRUE" )==0 )
    val = true;

  else if( strcmp( expr, "false" )==0 || strcmp( expr, "FALSE" )==0 )
    val = false;

  else
    res = false;

  return res;
}

void SALOME_Parameter::SetValue( const char* expr )
{
  if( sscanf( expr, "%lf", &myDouble ) == 1 )
    myType = SALOME::TReal;

  else if( sscanf( expr, "%i", &myInt ) == 1 )
    myType = SALOME::TInteger;

  else if( GetBoolean( expr, myBool ) )
    myType = SALOME::TBoolean;

  else if( true /* Expression check */ )
  {
    myType = SALOME::TExpression;
    myStr = strdup( expr );
  }
  
  else
  {
    myType = SALOME::TString;
    myStr = strdup( expr );
  }

  SetIsValid( true );
}

SALOME::ParamType SALOME_Parameter::GetType()
{
  return myType;
}

char* SALOME_Parameter::AsString()
{
  if( !IsValid() )
    throw SALOME_Exception( LOCALIZED( "Parameter is invalid" ) );

  char buf[256]; const char* res;
  switch( myType )
  {
  case SALOME::TBoolean:
    res = myBool ? "true" : "false";
    break;

  case SALOME::TInteger:
    sprintf( buf, "%i", myInt );
    res = buf;
    break;

  case SALOME::TReal:
    sprintf( buf, "%lf", myInt );
    res = buf;
    break;

  case SALOME::TExpression:
  case SALOME::TString:
    res = myStr;
    break;
  }

  return strdup( res );
}

CORBA::Long SALOME_Parameter::AsInteger()
{
  if( !IsValid() )
    throw SALOME_Exception( LOCALIZED( "Parameter is invalid" ) );

  CORBA::Long res = 0;
  switch( myType )
  {
  case SALOME::TBoolean:
    res = (int)myBool;
    break;

  case SALOME::TInteger:
    res = myInt;
    break;

  case SALOME::TReal:
    res = (int)myDouble;
    break;

  case SALOME::TExpression:
    res = Calculate()->AsInteger();
    break;

  case SALOME::TString:
    {
      int val;
      if( sscanf( myStr, "%i", &val ) == 0 )
        throw SALOME_Exception( LOCALIZED( "Parameter can not be converted into Integer" ) );
      res = val;
      break;
    }
  }

  return res;
}

CORBA::Double SALOME_Parameter::AsDouble()
{
  if( !IsValid() )
    throw SALOME_Exception( LOCALIZED( "Parameter is invalid" ) );

  CORBA::Double res = 0;
  switch( myType )
  {
  case SALOME::TBoolean:
    res = (double)myBool;
    break;

  case SALOME::TInteger:
    res = (double)myInt;
    break;

  case SALOME::TReal:
    res = myDouble;
    break;

  case SALOME::TExpression:
    res = Calculate()->AsDouble();
    break;

  case SALOME::TString:
    {
      double val;
      if( sscanf( myStr, "%lf", &val ) == 0 )
        throw SALOME_Exception( LOCALIZED( "Parameter can not be converted into Real" ) );
      res = val;
      break;
    }
  }

  return res;
}

CORBA::Boolean SALOME_Parameter::AsBoolean()
{
  bool res;
  switch( myType )
  {
  case SALOME::TBoolean:
    res = myBool;
    break;

  case SALOME::TInteger:
    res = (bool)myInt;
    break;

  case SALOME::TReal:
    res = (bool)myDouble;
    break;

  case SALOME::TExpression:
    res = Calculate()->AsBoolean();
    break;

  case SALOME::TString:
    {
      if( !GetBoolean( myStr, res ) )
        throw SALOME_Exception( LOCALIZED( "Parameter can not be converted into Boolean" ) );

      break;
    }
  }
  return res;
}

SALOME_Parameter* SALOME_Parameter::Calculate()
{
  return myType == SALOME::TExpression ? this : this;
}

char* SALOME_Parameter::GetEntry()
{
  return "";
}
