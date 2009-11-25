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
#include <SALOME_EvalParser.hxx>

std::string arg( const std::string& theStr, const std::string& theArg1 )
{
  std::string aRes = theStr;
  while( true )
  {
    int aPos = aRes.find( "%1" );
    if( aPos >= 0 )
      aRes.replace( aPos, 2, theArg1 );
    else
      break;
  }
  return aRes;
}

std::string arg( const std::string& theStr, const std::string& theArg1, const std::string& theArg2 )
{
  std::string aRes = theStr;
  while( true )
  {
    int aPos = aRes.find( "%1" );
    if( aPos >= 0 )
      aRes.replace( aPos, 2, theArg1 );
    else
    {
      aPos = aRes.find( "%2" );
      if( aPos >= 0 )
        aRes.replace( aPos, 2, theArg1 );
      else
        break;
    }
  }
  return aRes;
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
  const std::list<std::string> &aList1 = myNotebook->myDependencies[myKey], &aList2 = myNotebook->myDependencies[theKH.myKey];
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






SALOME_Notebook::SALOME_Notebook( PortableServer::POA_ptr thePOA, SALOMEDS::Study_ptr theStudy )
: SALOME::GenericObj_i( thePOA )
{
  myStudy = SALOMEDS::Study::_duplicate( theStudy );
}

void SALOME_Notebook::AddDependency( SALOME::ParameterizedObject_ptr theObj, SALOME::ParameterizedObject_ptr theRef )
{
  AddDependency( GetKey( theObj ), GetKey( theRef ) );
}

void SALOME_Notebook::RemoveDependency( SALOME::ParameterizedObject_ptr theObj, SALOME::ParameterizedObject_ptr theRef )
{
  //Utils_Locker lock( &myMutex );

  std::string anObjKey = GetKey( theObj ), aRefKey = GetKey( theRef );
  std::map< std::string, std::list<std::string> >::iterator it = myDependencies.find( anObjKey );
  if( it == myDependencies.end() )
    throwError( arg( "Dependency between '%1' and '%2' is not found", theObj->GetEntry(), theObj->GetEntry() ) );
  else
    it->second.remove( aRefKey );
}

void SALOME_Notebook::ClearDependencies( SALOME::ParameterizedObject_ptr theObj, SALOME::DependenciesType theType )
{
  ClearDependencies( GetKey( theObj ), theType );
}

void SALOME_Notebook::SetToUpdate( SALOME::ParameterizedObject_ptr theObj )
{
  //Utils_Locker lock( &myMutex );

  //printf( "SetToUpdate: %s\n", GetKey( theObj ).c_str() );

  SALOME::Parameter_var aParam = SALOME::Parameter::_narrow( theObj );
  if( !CORBA::is_nil( aParam ) )
  {
    std::string anEntry = aParam->GetEntry();
    SALOME_Parameter* aParamPtr = myParameters[anEntry];
    std::string aKey = GetKey( anEntry );
    ClearDependencies( aKey, SALOME::All );
    AddDependencies( aParamPtr );
  }

  /*
  printf( "Dependencies:\n" );
  std::map< std::string, std::list<std::string> >::const_iterator mit = myDependencies.begin(), mlast = myDependencies.end();
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

  Sort( myToUpdate );

  /*
  uit = myToUpdate.begin(); ulast = myToUpdate.end();
  for( ; uit!=ulast; uit++ )
    printf( "To update: %s\n", (*uit).key().c_str() );
  */
}

void SALOME_Notebook::Update()
{
  //Utils_Locker lock( &myMutex );

  //1. Simple recompute
  std::list< KeyHelper > aPostponedUpdate;
  std::list<KeyHelper>::const_iterator it = myToUpdate.begin(), last = myToUpdate.end();
  for( ; it!=last; it++ )
  {
    std::string aKey = (*it).key();
    //printf( "key = %s\n", aKey.c_str() );
    SALOME::ParameterizedObject_ptr anObj = FindObject( aKey );
    if( CORBA::is_nil( anObj ) )
      aPostponedUpdate.push_back( *it );
    else
      anObj->Update( _this() );
  }
  myToUpdate = aPostponedUpdate;

  //2. Substitutions
  std::list<SubstitutionInfo>::iterator sit = mySubstitutions.begin(), slast = mySubstitutions.end(), sremove;
  for( ; sit!=slast; )
  {
    if( Substitute( *sit ) )
    {
      sremove = sit;
      sit++;
      mySubstitutions.erase( sremove );
    }
    else
      sit++;
  }
}

bool SALOME_Notebook::Substitute( SubstitutionInfo& theSubstitution )
{
  std::list< KeyHelper > aPostponedUpdate;
  std::string
    anOldName = theSubstitution.myName,
    aNewName =  theSubstitution.myExpr;
  bool isRename = theSubstitution.myIsRename;
  SALOME_EvalExpr anExpr;
  anExpr.setExpression( theSubstitution.myExpr );

  std::list<KeyHelper>::const_iterator it = theSubstitution.myObjects.begin(), last = theSubstitution.myObjects.end();
  for( ; it!=last; it++ )
  {
    SALOME::ParameterizedObject_ptr anObj = FindObject( it->key() );
    if( CORBA::is_nil( anObj ) )
    {
      aPostponedUpdate.push_back( *it );
      continue;
    }

    SALOME::Parameter_var aParam = SALOME::Parameter::_narrow( anObj );
    if( CORBA::is_nil( aParam ) )
    {
      //it is real object
      SALOME::StringArray_var aParams = anObj->GetParameters();
      int n = aParams->length();
      //printf( "get params: %i\n", n );
      for( int i=0; i<n; i++ )
      {
        //printf( "\t%s\n", aParams[i].in() );
        if( anOldName == aParams[i].in() )
        {
          if( !isRename )
            AddExpression( aNewName.c_str() );
          aParams[i] = CORBA::string_dup( aNewName.c_str() );
        }
      }
      anObj->SetParameters( _this(), aParams );
    }
    else
    {
      std::string
        aName    = aParam->GetEntry(),
        anOldKey = GetKey( aName ),
        aNewKey  = GetKey( aNewName );
      SALOME_Parameter* aParamPtr = myParameters[aName];

      aParamPtr->Substitute( anOldName, aNewName );
      if( aName == anOldName )
      {
        //it is renamed parameter

        //1. update parameters map
        if( isRename )
        {
          std::list<std::string> aDeps = myDependencies[anOldKey];
          myDependencies.erase( anOldKey );
          myDependencies[aNewKey] = aDeps;
          
          myParameters.erase( anOldName );
          myParameters[aNewName] = aParamPtr;
        }
        else
        {
          ClearDependencies( anOldKey, SALOME::All );
          myParameters.erase( aName );
        }

        //2. update dependencies map
        std::map< std::string, std::list<std::string> >::iterator it = myDependencies.begin(), last = myDependencies.end();
        for( ; it!=last; it++ )
        {
          std::list<std::string>::iterator dit = it->second.begin(), dlast = it->second.end(), dremove;
          for( ; dit!=dlast; )
            if( *dit == anOldKey )
            {
              if( isRename )
              {
                *dit = aNewKey;
                dit++;
              }
              else
              {
                dremove = dit;
                dit++;
                it->second.erase( dremove );
              }
            }
            else
              dit++;
        }
      }
    }
  }
  theSubstitution.myObjects = aPostponedUpdate;
  return theSubstitution.myObjects.size() == 0;
}

void SALOME_Notebook::AddExpression( const char* theExpr )
{
  AddParameter( new SALOME_Parameter( this, theExpr ) );
}

void SALOME_Notebook::AddNamedExpression( const char* theName, const char* theExpr )
{
  AddParameter( new SALOME_Parameter( this, theName, theExpr, true ) );
}

void SALOME_Notebook::AddBoolean( const char* theName, CORBA::Boolean theValue )
{
  AddParameter( new SALOME_Parameter( this, theName, theValue ) );
}

void SALOME_Notebook::AddInteger( const char* theName, CORBA::Long theValue )
{
  AddParameter( new SALOME_Parameter( this, theName, (int)theValue ) );
}

void SALOME_Notebook::AddReal( const char* theName, CORBA::Double theValue )
{
  AddParameter( new SALOME_Parameter( this, theName, theValue ) );
}

void SALOME_Notebook::AddString( const char* theName, const char* theValue )
{
  AddParameter( new SALOME_Parameter( this, theName, theValue, false ) );
}

SALOME_Notebook::SubstitutionInfo SALOME_Notebook::CreateSubstitution( const std::string& theName, const std::string& theExpr, bool theIsRename )
{
  SubstitutionInfo anInfo;
  anInfo.myName = theName;
  anInfo.myExpr = theExpr;
  anInfo.myIsRename = theIsRename;
  std::list<std::string> aDeps = GetAllDependingOn( GetKey( theName ) );
  anInfo.myObjects.clear();
  std::list<std::string>::const_iterator dit = aDeps.begin(), dlast = aDeps.end();
  for( ; dit!=dlast; dit++ )
    anInfo.myObjects.push_back( KeyHelper( *dit, this ) );
  Sort( anInfo.myObjects );
  return anInfo;
}

void SALOME_Notebook::AddSubstitution( const std::string& theName, const std::string& theExpr, bool theIsRename )
{
  mySubstitutions.push_back( CreateSubstitution( theName, theExpr, theIsRename ) );
}

void SALOME_Notebook::Rename( const char* theOldName, const char* theNewName )
{
  SALOME_Parameter* aParam = GetParameterPtr( theOldName );
  std::string aMsg;
  if( !aParam )
    aMsg = arg( "Name '%1' does not exist", theOldName );
  else if( aParam->IsAnonymous() )
    aMsg = arg( "Parameter '%1' is anonymous", theOldName );
  else if( GetParameterPtr( theNewName ) )
    aMsg = arg( "New name '%1' is already used", theNewName );

  CheckParamName( theNewName, aMsg );

  if( aMsg.length() == 0 )
    AddSubstitution( theOldName, theNewName, true );
  else
    throwError( aMsg );
}

void SALOME_Notebook::Remove( const char* theParamName )
{
  SALOME_Parameter* aParam = GetParameterPtr( theParamName );
  if( aParam )
  {
    std::string anExpr;
    if( aParam->IsCalculable() )
      anExpr = aParam->Expression();
    else
      anExpr = aParam->AsString();
    AddSubstitution( theParamName, anExpr, false );
  }
  else
    throwError( arg( "The parameter '%1' is not found", theParamName ) );
}

void SALOME_Notebook::UpdateAnonymous( const std::string& theOldName, const std::string& theNewName )
{
  SubstitutionInfo anInfo = CreateSubstitution( theOldName, theNewName, true );
  Substitute( anInfo );
}

SALOME::StringArray* SALOME_Notebook::GenerateList( const std::list<std::string>& theList ) const
{
  SALOME::StringArray_var aRes = new SALOME::StringArray();
  aRes->length( theList.size() );
  std::list<std::string>::const_iterator nit = theList.begin(), nlast = theList.end();
  for( int i=0; nit!=nlast; nit++, i++ )
    aRes[i] = CORBA::string_dup( nit->c_str() );
    
  return aRes._retn();
}

SALOME::StringArray* SALOME_Notebook::Parameters()
{
  //Utils_Locker lock( &myMutex );

  std::list<std::string> aNames;
  std::map< std::string, SALOME_Parameter* >::const_iterator it = myParameters.begin(), last = myParameters.end();
  for( ; it!=last; it++ )
  {
    std::string anEntry = it->second->GetEntry();
    if( !it->second->IsAnonymous() && find( aNames.begin(), aNames.end(), anEntry ) == aNames.end() )
      aNames.push_back( anEntry );
  }

  aNames.sort();
  return GenerateList( aNames );
}

SALOME::StringArray* SALOME_Notebook::AbsentParameters()
{
  //Utils_Locker lock( &myMutex );

  std::list<std::string> anAbsents;
  std::map< std::string, std::list<std::string> >::const_iterator it = myDependencies.begin(), last = myDependencies.end();
  for( ; it!=last; it++ )
  {
    std::list<std::string>::const_iterator pit = it->second.begin(), plast = it->second.end();
    for( ; pit!=plast; pit++ )
    {
      std::string anEntry, aComponent = GetComponent( *pit, anEntry );
      if( aComponent == PARAM_COMPONENT && !GetParameterPtr( anEntry.c_str() ) && 
          find( anAbsents.begin(), anAbsents.end(), anEntry ) == anAbsents.end() )
        anAbsents.push_back( anEntry );
    }
  }

  anAbsents.sort();
  return GenerateList( anAbsents );
}

SALOME::Parameter_ptr SALOME_Notebook::GetParameter( const char* theParamName )
{
  //printf( "Param, name = %s\n", theParamName );
  SALOME_Parameter* aParam = GetParameterPtr( theParamName );
  //printf( "Result = %i\n", (int)aParam );
  SALOME::Parameter_var aRes;
  if( aParam )
    aRes = aParam->_this();
  return aRes._retn();
}

SALOME_Parameter* SALOME_Notebook::GetParameterPtr( const char* theParamName ) const
{
  //Utils_Locker lock( const_cast<Utils_Mutex*>( &myMutex ) );

  std::map< std::string, SALOME_Parameter* >::const_iterator it = myParameters.find( theParamName );
  return it==myParameters.end() ? 0 : it->second;
}

void SALOME_Notebook::AddParameter( SALOME_Parameter* theParam )
{
  //Utils_Locker lock( &myMutex );

  std::string anEntry = theParam->GetEntry(), aMsg;
  if( !theParam->IsAnonymous() && !CheckParamName( anEntry, aMsg ) )
    throwError( aMsg );

  //printf( "Add param: %s\n", anEntry.c_str() );

  std::map< std::string, SALOME_Parameter* >::const_iterator it = myParameters.find( anEntry );
  if( it!=myParameters.end() )
  {
    delete it->second;
    ClearDependencies( GetKey( anEntry ), SALOME::All );
  }

  try
  {
    AddDependencies( theParam );
  }
  catch( const SALOME::NotebookError& ex )
  {
    Remove( anEntry.c_str() );
    throw ex;
  }
}

void SALOME_Notebook::AddDependencies( SALOME_Parameter* theParam )
{
  //Utils_Locker lock( &myMutex );

  //printf( "Dependencies search\n" );
  std::string anEntry = theParam->GetEntry();
  std::string aParamKey = GetKey( anEntry );
  myParameters[anEntry] = theParam;
  SALOME_StringList aDeps = theParam->Dependencies();
  SALOME_StringList::const_iterator dit = aDeps.begin(), dlast = aDeps.end();
  for( ; dit!=dlast; dit++ )
  {
    std::string aKey = GetKey( *dit );
    AddDependency( aParamKey, aKey );
    //printf( "add dep to %s, res = %i\n", aKey.c_str(), ok );
  }
}

void SALOME_Notebook::AddDependency( const std::string& theObjKey, const std::string& theRefKey )
{
  //Utils_Locker lock( &myMutex );
  std::string anObjName, aRefName;
  GetComponent( theObjKey, anObjName );
  GetComponent( theRefKey, aRefName );

  std::list<std::string> aDeps = GetAllDependingOn( theObjKey );
  if( find( aDeps.begin(), aDeps.end(), theRefKey ) != aDeps.end () )
    throwError( arg( "Dependency %1 -> %2 creates a cyclic dependency", anObjName, aRefName ) );
    //after creation a cyclic dependency could appear

  std::list<std::string>& aList = myDependencies[theObjKey];
  bool ok = find( aList.begin(), aList.end(), theRefKey ) == aList.end();
  if( ok )
    aList.push_back( theRefKey );
  else
    throwError( arg( "Dependency %1 -> %2 is already created", anObjName, aRefName ) );
}

void SALOME_Notebook::ClearDependencies( const std::string& theObjKey, SALOME::DependenciesType theType )
{
  //Utils_Locker lock( &myMutex );

  //printf( "Clear dependencies: %s\n", theObjKey.c_str() );

  std::map< std::string, std::list<std::string> >::iterator it = myDependencies.find( theObjKey );
  if( it == myDependencies.end() )
    return;

  std::list<std::string> aNewDeps;
  if( theType != SALOME::All )
  {
    std::list<std::string>::const_iterator dit = it->second.begin(), dlast = it->second.end();
    for( ; dit!=dlast; dit++ )
    {
      SALOME::Parameter_var aParam = SALOME::Parameter::_narrow( FindObject( *dit ) );
      if( ( !CORBA::is_nil( aParam ) && theType == SALOME::Objects ) || theType == SALOME::Parameters )
        aNewDeps.push_back( *dit );
    }
  }

  if( aNewDeps.size() == 0 )
    myDependencies.erase( theObjKey );
  else
    it->second = aNewDeps;
}

std::string SALOME_Notebook::GetKey( SALOME::ParameterizedObject_ptr theObj )
{
  return GetKey( theObj->GetComponent(), theObj->GetEntry() );
}

std::string SALOME_Notebook::GetKey( const std::string& theComponent, const std::string& theEntry )
{
  return arg( "%1#%2", theComponent, theEntry );
}

std::string SALOME_Notebook::GetKey( const std::string& theParamName )
{
  return GetKey( PARAM_COMPONENT, theParamName );
}

std::list<std::string> SALOME_Notebook::GetAllDependingOn( const std::string& theKey )
{
  //Utils_Locker lock( &myMutex );

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
      std::map< std::string, std::list<std::string> >::const_iterator dit = myDependencies.begin(), dlast = myDependencies.end();
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

std::string SALOME_Notebook::GetComponent( const std::string& theKey, std::string& theEntry ) const
{
  int aPos = theKey.find( "#" );
  theEntry = theKey.substr( aPos+1, theKey.length()-aPos-1 );
  return theKey.substr( 0, aPos );
}

SALOME::ParameterizedObject_ptr SALOME_Notebook::FindObject( const std::string& theKey )
{
  std::string anEntry, aComponent = GetComponent( theKey, anEntry );
  if( aComponent == PARAM_COMPONENT )
    return GetParameter( anEntry.c_str() );
  else
    return SALOME::ParameterizedObject::_narrow( myStudy->FindObjectByInternalEntry( aComponent.c_str(), anEntry.c_str() ) );
}

CORBA::Boolean SALOME_Notebook::Save( const char* theFileName )
{
  //printf( "SALOME_Notebook::Save into %s\n", theFileName );

  FILE* aFile = fopen( theFileName, "w" );
  if( !aFile )
    return false;

  fprintf( aFile, "\n\nnotebook\n" );

  //1. Save dependencies
  fprintf( aFile, "Dependencies\n" );
  std::map< std::string, std::list<std::string> >::const_iterator dit = myDependencies.begin(), dlast = myDependencies.end();
  for( ; dit!=dlast; dit++ )
  {
    fprintf( aFile, "%s -> ", dit->first.c_str() );
    std::list<std::string>::const_iterator it = dit->second.begin(), last = dit->second.end();
    for( ; it!=last; it++ )
      fprintf( aFile, "%s ", it->c_str() );
    fprintf( aFile, "\n" );
  }

  //2. Save parameters
  fprintf( aFile, "Parameters\n" );
  std::map< std::string, SALOME_Parameter* >::const_iterator pit = myParameters.begin(), plast = myParameters.end();
  for( ; pit!=plast; pit++ )
  {
    fprintf( aFile, pit->second->Save().c_str() );
    fprintf( aFile, "\n" );
  }

  //3. Save update list
  fprintf( aFile, "Update\n" );
  std::list< KeyHelper >::const_iterator uit = myToUpdate.begin(), ulast = myToUpdate.end();
  for( ; uit!=ulast; uit++ )
  {
    fprintf( aFile, uit->key().c_str() );
    fprintf( aFile, "\n" );
  }

  fclose( aFile );
  return true;
}

namespace State
{
  typedef enum { Start, Title, Dependencies, Parameters, Update } LoadState;
}

CORBA::Boolean SALOME_Notebook::Load( const char* theFileName )
{
  printf( "SALOME_Notebook::Load\n" );

  State::LoadState aState = State::Start;
  FILE* aFile = fopen( theFileName, "r" );
  if( !aFile )
    return false;

  const int BUF_SIZE = 256;
  char aBuf[BUF_SIZE];
  bool ok = true;
  try
  {
    while( ok && fgets( aBuf, BUF_SIZE, aFile ) != NULL )
    {
      int aLen = strlen( aBuf );
      if( aLen > 0 )
        aBuf[aLen - 1] = 0;

      std::string aLine = aBuf;
      printf( "Line: '%s'\n", aBuf );
      if( aState == State::Start )
      {
        if( aLine == "notebook" )
          aState = State::Title;
      }
      else if( aLine == "Dependencies" )
        aState = State::Dependencies;
      else if( aLine == "Parameters" )
        aState = State::Parameters;     
      else if( aLine == "Update" )
        aState = State::Update;

      else switch( aState )
           {
           case State::Start:
           case State::Title:
             ok = false;
             break;

           case State::Dependencies:
             ParseDependencies( aLine );
             break;

           case State::Parameters:
             AddParameter( SALOME_Parameter::Load( aLine ) );
             break;

           case State::Update:
             myToUpdate.push_back( KeyHelper( aLine, this ) );
             break;
           }
    }
  }
  catch( const SALOME::NotebookError& ex )
  {
    printf( "Exception: %s\n", ex.Reason.in() );
    ok = false;
  }

  fclose( aFile );
  return ok;
}

std::vector<std::string> SALOME_Notebook::Split( const std::string& theData, const std::string& theSeparator, bool theIsKeepEmpty )
{
  std::vector<std::string> aParts;
  int aPrev = 0, anInd = 0;
  while( true )
  {
    anInd = theData.find( theSeparator, aPrev );
    if( anInd < 0 )
      break;

    std::string aPart = theData.substr( aPrev, anInd - aPrev );
    if( aPart.length() > 0 || theIsKeepEmpty )
      aParts.push_back( aPart );

    aPrev = anInd + 1;
  }
  return aParts;
}

void SALOME_Notebook::ParseDependencies( const std::string& theData )
{
  std::vector<std::string> aParts = Split( theData, " ", false );
  if( aParts.size() < 2 || aParts.at( 1 ) != "->" )
    throwError( arg( "Incorrect parts format: %1", theData ) );

  std::string aKey = aParts[0];
  for( int i = 2, n = aParts.size(); i < n; i++ )
    AddDependency( aKey, aParts[i] );
}

char* SALOME_Notebook::DumpPython()
{
  std::string aRes;
  
  aRes = "from salome_notebook import *\n\n";
  std::list< KeyHelper > aParams;
  //printf( "%i\n", myParameters.size() );
  std::map< std::string, SALOME_Parameter* >::const_iterator it = myParameters.begin(), last = myParameters.end();
  for( ; it!=last; it++ )
    aParams.push_back( KeyHelper( it->first, this ) );

  printf( "%i\n", aParams.size() );
  Sort( aParams );
  printf( "%i\n", aParams.size() );
  std::list< KeyHelper >::const_iterator pit = aParams.begin(), plast = aParams.end();
  std::string anEntry;
  for( ; pit!=plast; pit++ )
  {
    GetComponent( pit->key(), anEntry );

    SALOME_Parameter* aParam = GetParameterPtr( anEntry.c_str() );
    if( aParam->IsAnonymous() )
      continue;

    aRes += "notebook.set( ";
    aRes += "\"";
    aRes += anEntry;
    aRes += "\", ";
    if( aParam->IsCalculable() )
    {
      aRes += "\"";
      aRes += aParam->Expression();
      aRes += "\"";
    }
    else
      aRes += aParam->AsString();

    aRes += " )\n";
  }

  return CORBA::string_dup( aRes.c_str() );
}

bool SALOME_Notebook::CheckParamName( const std::string& theParamName, std::string& theMsg ) const
{
  int len = theParamName.length();
  if( len == 0 )
  {
    theMsg = "The name should not be empty";
    return false;
  }

  if( isdigit( theParamName[0] ) )
  {
    theMsg = "The name should not start by a digit";
    return false;
  }

  for( int i=0, n=theParamName.length(); i<n; i++ )
    if( isspace( theParamName[i] ) )
    {
      theMsg = "The name should not contain white symbols";
      return false;
    }

  SALOME_EvalExpr anExpr( theParamName );
  bool ok = anExpr.parser()->isMonoParam();
  if( !ok )
    theMsg = "The name should not be expression";
  return ok;
}

void SALOME_Notebook::Sort( std::list< KeyHelper >& theList ) const
{
  std::list<KeyHelper>::iterator uit = theList.begin(), uit1, ulast = theList.end();
  for( ; uit!=ulast; uit++ )
    for( uit1=uit, uit1++; uit1!=ulast; uit1++ )
      if( *uit1 < *uit )
      {
        KeyHelper tmp = *uit1;
        *uit1 = *uit;
        *uit = tmp;
      }
}

char* SALOME_Notebook::Dump()
{
  std::string aStr;

  //1. Dependencies
  aStr += "==========================\n";
  aStr += "===   NOTEBOOK Dump:   ===\n";
  aStr += "==========================\n\n";
  aStr += "Dependencies:\n";
  std::map< std::string, std::list<std::string> >::const_iterator dit = myDependencies.begin(), dlast = myDependencies.end();
  for( ; dit!=dlast; dit++ )
  {
    aStr += dit->first;
    aStr += " -> ";
    std::list<std::string>::const_iterator it = dit->second.begin(), last = dit->second.end();
    for( ; it!=last; it++ )
      aStr += *it + " ";
    aStr += "\n";
  }
  aStr += "\n";

  //2. Parameters
  aStr += "Parameters:\n";
  std::map< std::string, SALOME_Parameter* >::const_iterator pit = myParameters.begin(), plast = myParameters.end();
  for( ; pit!=plast; pit++ )
  {
    aStr += pit->first + " (" + pit->second->GetEntry() + "): ";
    if( pit->second->IsAnonymous() )
      aStr += "[A] ";
    if( pit->second->IsCalculable() )
      aStr += pit->second->Expression() + " (val=" + pit->second->AsString() + ")";
    else
      aStr += pit->second->AsString();
    aStr += "\n";
  }
  aStr += "\n";

  //3. Update lists
  aStr += "Update lists:\n";
  aStr += "\tSimple recompute:\n";
  std::list< KeyHelper >::const_iterator uit = myToUpdate.begin(), ulast = myToUpdate.end();
  for( ; uit!=ulast; uit++ )
    aStr += "\t" + uit->key() + "\n";

  aStr += "\tSubstitutions:\n";
  std::list<SubstitutionInfo>::const_iterator sit = mySubstitutions.begin(), slast = mySubstitutions.end();
  for( ; sit!=slast; sit++ )
  {
    aStr += "\t" + sit->myName;
    aStr += " -> " + sit->myExpr + ": ";
    std::list< KeyHelper >::const_iterator oit = sit->myObjects.begin(), olast = sit->myObjects.end();
    for( ; oit!=olast; oit++ )
      aStr += oit->key() + " ";
    aStr += "\n";
  }

  return CORBA::string_dup( aStr.c_str() );
}

void SALOME_Notebook::throwError( const std::string& theErrorMsg )
{
  SALOME::NotebookError anError;
  anError.Reason = CORBA::string_dup( theErrorMsg.c_str() );
  throw anError;
}

char* SALOME_Notebook::GetParameters( const char* theComponent, const char* theEntry )
{
  std::string aRes, aKey = GetKey( theComponent, theEntry ), aComponent, aName;
  std::map< std::string, std::list<std::string> >::const_iterator it = myDependencies.find( aKey );
  if( it!=myDependencies.end() )
  {
    std::list<std::string>::const_iterator dit = it->second.begin(), dlast = it->second.end();
    for( ; dit!=dlast; dit++ )
    {
      aComponent = GetComponent( *dit, aName );
      if( aComponent==PARAM_COMPONENT )
      {
        if( aRes.length() > 0 )
          aRes += ", ";
        aRes += aName;
      }
    }
  }

  return CORBA::string_dup( aRes.c_str() );
}
