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
//  File   : SALOME_Notebook.cxx
//  Author : Alexandre SOLOVYOV
//  Module : SALOME
//

#include <SALOME_Notebook.hxx>
#include <SALOME_Parameter.hxx>

SALOME_Notebook::SALOME_Notebook( SALOMEDS::Study_ptr theStudy )
{
  myStudy = SALOMEDS::Study::_duplicate( theStudy );
}

CORBA::Boolean SALOME_Notebook::AddDependency( SALOME::ParameterizedObject_ptr theObj, SALOME::ParameterizedObject_ptr theRef )
{
  return AddDependency( GetKey( theObj ), GetKey( theRef ) );
}

void SALOME_Notebook::RemoveDependency( SALOME::ParameterizedObject_ptr theObj, SALOME::ParameterizedObject_ptr theRef )
{
  std::string anObjKey = GetKey( theObj ), aRefKey = GetKey( theRef );
  myDeps[anObjKey].remove( aRefKey );
}

void SALOME_Notebook::ClearDependencies( SALOME::ParameterizedObject_ptr theObj )
{
  ClearDependencies( GetKey( theObj ) );
}

void SALOME_Notebook::SetToUpdate( SALOME::ParameterizedObject_ptr theObj )
{
  printf( "SetToUpdate: %s\n", GetKey( theObj ).c_str() );

  SALOME::Parameter_ptr aParam = SALOME::Parameter::_narrow( theObj );
  if( !CORBA::is_nil( aParam ) )
  {
    std::string anEntry = aParam->GetEntry();
    SALOME_Parameter* aParamPtr = myParams[anEntry];
    std::string aKey = GetKey( anEntry );
    ClearDependencies( aKey );
    AddDependencies( aParamPtr );
  }

  /*
  printf( "Dependencies:\n" );
  std::map< std::string, std::list<std::string> >::const_iterator mit = myDeps.begin(), mlast = myDeps.end();
  for( ; mit!=mlast; mit++ )
  {
    printf( "%s -> [ ", mit->first.c_str() );
    std::list<std::string>::const_iterator lit = mit->second.begin(), llast = mit->second.end();
    for( ; lit!=llast; lit++ )
      printf( "%s ", (*lit).c_str() );
    printf( "]\n" );
  }
  */

  std::string anObjKey = GetKey( theObj );
  std::list<std::string> aDeps = GetAllDependingOn( anObjKey );
  std::list<std::string>::const_iterator it = aDeps.begin(), last = aDeps.end();
  for( ; it!=last; it++ )
    if( find( myToUpdate.begin(), myToUpdate.end(), *it ) == myToUpdate.end() )
      myToUpdate.push_back( KeyHelper( *it, this ) );

  //myToUpdate.sort();
  std::list<KeyHelper>::iterator uit = myToUpdate.begin(), uit1, ulast = myToUpdate.end();
  for( ; uit!=ulast; uit++ )
    for( uit1=uit, uit1++; uit1!=ulast; uit1++ )
      if( *uit1 < *uit )
      {
        KeyHelper tmp = *uit1;
        *uit1 = *uit;
        *uit = tmp;
      }

  uit = myToUpdate.begin(); ulast = myToUpdate.end();
  for( ; uit!=ulast; uit++ )
    printf( "To update: %s\n", (*uit).key().c_str() );
}

void SALOME_Notebook::Update()
{
  //printf( "Update\n" );
  std::list< KeyHelper > aPostponedUpdate;
  std::list<KeyHelper>::const_iterator it = myToUpdate.begin(), last = myToUpdate.end();
  for( ; it!=last; it++ )
  {
    std::string aKey = (*it).key();
    SALOME::ParameterizedObject_ptr anObj = FindObject( aKey );
    if( CORBA::is_nil( anObj ) )
      aPostponedUpdate.push_back( *it );
    else
      anObj->Update();
  }
  myToUpdate = aPostponedUpdate;
}

CORBA::Boolean SALOME_Notebook::AddExpr( const char* theExpr )
{
  return AddParam( new SALOME_Parameter( this, theExpr ) );
}

CORBA::Boolean SALOME_Notebook::AddNameExpr( const char* theName, const char* theExpr )
{
  return AddParam( new SALOME_Parameter( this, theName, theExpr ) );
}

CORBA::Boolean SALOME_Notebook::AddValue( const char* theName, CORBA::Double theValue )
{
  return AddParam( new SALOME_Parameter( this, theName, theValue ) );
}

void SALOME_Notebook::Remove( const char* theParamName )
{
  std::string aKey = GetKey( theParamName );
  ClearDependencies( aKey );
  myDeps.erase( aKey );
  myParams.erase( theParamName );
}

SALOME::Parameter_ptr SALOME_Notebook::Param( const char* theParamName )
{
  //printf( "Param, name = %s\n", theParamName );
  return ParamPtr( theParamName )->_this();
}

SALOME_Parameter* SALOME_Notebook::ParamPtr( const char* theParamName ) const
{
  std::map< std::string, SALOME_Parameter* >::const_iterator it = myParams.find( theParamName );
  return it==myParams.end() ? 0 : it->second;
}

bool SALOME_Notebook::AddParam( SALOME_Parameter* theParam )
{
  std::string anEntry = theParam->GetEntry();
  //printf( "Add param: %s\n", anEntry.c_str() );

  std::map< std::string, SALOME_Parameter* >::const_iterator it = myParams.find( anEntry );
  if( it!=myParams.end() )
  {
    delete it->second;
    ClearDependencies( GetKey( anEntry ) );
  }

  bool ok = AddDependencies( theParam );
  //printf( "Add param: %s, Result = %i\n\n", anEntry.c_str(), ok );

  if( !ok )
  {
    //printf( "Removed\n" );
    Remove( anEntry.c_str() );
  }

  return ok;
}

bool SALOME_Notebook::AddDependencies( SALOME_Parameter* theParam )
{
  //printf( "Dependencies search\n" );
  std::string anEntry = theParam->GetEntry();
  std::string aParamKey = GetKey( anEntry );
  myParams[anEntry] = theParam;
  SALOME_StringList aDeps = theParam->Dependencies();
  SALOME_StringList::const_iterator dit = aDeps.begin(), dlast = aDeps.end();
  bool ok = true;
  for( ; dit!=dlast && ok; dit++ )
  {
    std::string aKey = GetKey( *dit );
    ok = AddDependency( aParamKey, aKey );
    //printf( "add dep to %s, res = %i\n", aKey.c_str(), ok );
  }
  return ok;
}

bool SALOME_Notebook::AddDependency( const std::string& theObjKey, const std::string& theRefKey )
{
  std::list<std::string> aDeps = GetAllDependingOn( theObjKey );
  if( find( aDeps.begin(), aDeps.end(), theRefKey ) != aDeps.end () )
    return false; //after creation a cyclic dependency could appear

  std::list<std::string>& aList = myDeps[theObjKey];
  bool ok = find( aList.begin(), aList.end(), theRefKey ) == aList.end();
  if( ok )
    aList.push_back( theRefKey );

  return ok;
}

void SALOME_Notebook::ClearDependencies( const std::string& theObjKey )
{
  //printf( "Clear dependencies: %s\n", theObjKey.c_str() );
  myDeps.erase( theObjKey );
}

std::string SALOME_Notebook::GetKey( SALOME::ParameterizedObject_ptr theObj )
{
  return std::string( theObj->GetComponent() ) + "#" + theObj->GetEntry();
}

std::string SALOME_Notebook::GetKey( const std::string& theParamName )
{
  return PARAM_COMPONENT + "#" + theParamName;
}

std::list<std::string> SALOME_Notebook::GetAllDependingOn( const std::string& theKey )
{
  std::list<std::string> aDeps, aCurrents, aNewCurrents;
  aCurrents.push_back( theKey );
  aDeps.push_back( theKey );
  while( aCurrents.size() > 0 )
  {
    aNewCurrents.clear();
    std::list<std::string>::const_iterator cit = aCurrents.begin(), clast = aCurrents.end();
    for( ; cit!=clast; cit++ )
    {
      //printf( "Check of %s:\n", (*cit).c_str() );
      std::map< std::string, std::list<std::string> >::const_iterator dit = myDeps.begin(), dlast = myDeps.end();
      for( ; dit!=dlast; dit++ )
      {
        std::string k = dit->first;
        //printf( "\t%s\n", k.c_str() );
        if( find( dit->second.begin(), dit->second.end(), *cit ) != dit->second.end() &&
            find( aDeps.begin(), aDeps.end(), k ) == aDeps.end() )
        {
          //printf( "\t\tadd\n" );
          aNewCurrents.push_back( k );
          aDeps.push_back( k );
        }
      }
    }

    aCurrents = aNewCurrents;
  }
  return aDeps;
}

SALOME::ParameterizedObject_ptr SALOME_Notebook::FindObject( const std::string& theKey )
{
  int aPos = theKey.find( "#" );
  std::string aComponent = theKey.substr( 0, aPos ), anEntry = theKey.substr( aPos+1, theKey.length()-aPos-1 );
  if( aComponent == PARAM_COMPONENT )
    return Param( anEntry.c_str() );
  else
    return SALOME::ParameterizedObject::_narrow( myStudy->FindObjectByInternalEntry( aComponent.c_str(), anEntry.c_str() ) );
}

SALOME_Notebook::KeyHelper::KeyHelper( const std::string& theKey, SALOME_Notebook* theNotebook )
: myKey( theKey ), myNotebook( theNotebook )
{
}

std::string SALOME_Notebook::KeyHelper::key() const
{
  return myKey;
}

bool SALOME_Notebook::KeyHelper::operator < ( const KeyHelper& theKH ) const
{
  bool ok;
  const std::list<std::string> &aList1 = myNotebook->myDeps[myKey], &aList2 = myNotebook->myDeps[theKH.myKey];
  if( find( aList1.begin(), aList1.end(), theKH.myKey ) != aList1.end() )
    ok = false;
  else if( find( aList2.begin(), aList2.end(), myKey ) != aList2.end() )
    ok = true;
  else
    ok = myKey < theKH.myKey;

  //printf( "%s < %s ? %i\n", myKey.c_str(), theKH.myKey.c_str(), ok );
  return ok;
}

bool SALOME_Notebook::KeyHelper::operator == ( const std::string& theKey ) const
{
  return myKey == theKey;
}
