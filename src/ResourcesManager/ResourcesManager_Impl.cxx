//  ResourcesManager_Impl
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : ResourcesManager_Impl.cxx
//  Author : Jean Rahuel
//  Module : SALOME

using namespace std;

#include <fstream>

#include "ResourcesManager_Impl.hxx"

Manager_Impl::Manager_Impl( CORBA::ORB_ptr orb ,
		            PortableServer::POA_ptr poa ,
                            SALOME_NamingService * NamingService ,
                            int argc ,
                            char ** argv ) {

  _Orb = CORBA::ORB::_duplicate( orb ) ;
  _Poa = PortableServer::POA::_duplicate( poa ) ;
  _Id = _Poa->activate_object( this ) ;
  _NamingService = NamingService ;

  _ComputersList = new Resources::ListOfComputers() ;

  if ( !ParseArguments( argc , argv , &_CatalogPath ) ) {
    MESSAGE( "Error while argument parsing" ) ;
  }
  else if ( _CatalogPath == NULL ) {
    MESSAGE( "Error the resources catalog should be indicated" ) ;
  }
  else {
    MESSAGE("Parse resources catalog");
    ParseXmlFile( _CatalogPath ) ;
  }
  int i ;
  MESSAGE(_ComputersVector.size() << " computers") ;
  int size = _ComputersVector.size() ;
  _ComputersList->length( size ) ;
  for ( i = 0 ; i < size ; i++ ) {
    Computer_Impl * aComputer = _ComputersVector[ i ] ;
    PortableServer::ObjectId * id = aComputer->getId() ;
    CORBA::Object_var obj = _Poa->id_to_reference( *id ) ;
    Resources::Computer_var iobject = Resources::Computer::_narrow( obj ) ;
    _ComputersList[ i ] = Resources::Computer::_duplicate( iobject ) ;
    Resources::ComputerParameters * aComputerParameters = aComputer->Parameters() ;
    const char * anAlias = aComputerParameters->Alias ;
    if ( _MapOfAliasComputers[ anAlias ] > 0 ) {
      MESSAGE("Duplicate computer " << aComputerParameters->FullName << " " << anAlias ) ;
    }
    else {
      MESSAGE("Computer " << aComputerParameters->FullName << " " << anAlias ) ;
      _MapOfAliasComputers[ anAlias ] = i + 1 ;
    }
    const char * aFullName = aComputerParameters->FullName ;
    if ( _MapOfFullNameComputers[ aFullName ] > 0 ) {
      MESSAGE("Duplicate computer " << aComputerParameters->FullName << " " << aFullName ) ;
    }
    else {
      MESSAGE("Computer " << aComputerParameters->FullName << " " << aFullName ) ;
      _MapOfFullNameComputers[ aFullName ] = i + 1 ;
    }
  }
  MESSAGE(_ComputersList->length() << " computers") ;
}

Manager_Impl::~Manager_Impl() {
  MESSAGE("~Manager_Impl()") ;
}

long Manager_Impl::ping() {
  MESSAGE("Manager_Impl::ping()") ;
  return 1 ;
}

long Manager_Impl::SshAccess( const char * aComputerName ) {
  if ( !CORBA::is_nil( SearchComputer( aComputerName ) ) ) {
    return SearchComputer( aComputerName )->Parameters()->SshAccess ;
  }
  return 0 ;
}

char * Manager_Impl::UserName( const char * aComputerName ) {
  if ( !CORBA::is_nil( SearchComputer( aComputerName ) ) ) {
    return CORBA::string_dup( SearchComputer( aComputerName )->Parameters()->UserName ) ;
  }
  return CORBA::string_dup( "" ) ;
}

Resources::ListOfComputers * Manager_Impl::AllComputers() {
  MESSAGE("Manager_Impl::AllComputers()") ;
  Resources::ListOfComputers_var aListOfComputers = new Resources::ListOfComputers() ;
  aListOfComputers->length( _ComputersList->length() ) ;
  int i ;
  for ( i = 0 ; i < _ComputersList->length() ; i++ ) {
    Resources::Computer_var aComputer = _ComputersList[ i ] ;
    aListOfComputers[ i ] = Resources::Computer::_duplicate( aComputer ) ;
  }
  return aListOfComputers._retn() ;
}

Resources::ListOfComputers * Manager_Impl::GetComputers( const Containers::MachineParameters & aMachineParameters ) {
  MESSAGE("Manager_Impl::GetComputers() " << aMachineParameters.HostName << " in list of "
          << _ComputersList->length() << " computers for '" << aMachineParameters.HostName << "'" ) ;
  Resources::ListOfComputers_var aListOfComputers = new Resources::ListOfComputers() ;
  int i ;
  if ( strlen( (char * ) ((CORBA::String_member ) aMachineParameters.HostName) ) ) {
    for ( i = 0 ; i < _ComputersList->length() ; i++ ) {
      Resources::Computer_var aComputer = _ComputersList[ i ] ;
      if ( strcmp( aComputer->Parameters()->FullName , aMachineParameters.HostName ) == 0 ||
           strcmp( aComputer->Parameters()->Alias , aMachineParameters.HostName ) == 0 ) {
        aListOfComputers->length( 1 ) ;
        aListOfComputers[ 0 ] = Resources::Computer::_duplicate( aComputer ) ;
        MESSAGE("Manager_Impl::GetComputers() " << aComputer->Parameters()->FullName << " "
                << aComputer->Parameters()->Alias << " selected" ) ;
        break ;
      }
      else {
        MESSAGE("Manager_Impl::GetComputers() " << aComputer->Parameters()->FullName << " "
                << strcmp( aComputer->Parameters()->FullName , aMachineParameters.HostName ) << " "
                << aComputer->Parameters()->Alias << " "
                << strcmp( aComputer->Parameters()->Alias , aMachineParameters.HostName ) << " skipped" ) ;
      }
    }
  }
  else {
    for ( i = 0 ; i < _ComputersList->length() ; i++ ) {
      Resources::Computer_var aComputer = _ComputersList[ i ] ;
      if ( ( aMachineParameters.Os == Containers::Unknown ||
             ( aComputer->Parameters()->Os == aMachineParameters.Os ) ) &&
           aComputer->Parameters()->Memory >= aMachineParameters.Memory &&
           aComputer->Parameters()->CpuClock >= aMachineParameters.CpuClock &&
           aComputer->Parameters()->NbProc >= aMachineParameters.NbProc &&
           aComputer->Parameters()->NbNode >= aMachineParameters.NbNode ) {
        int size = aListOfComputers->length() ;
        aListOfComputers->length( size + 1 ) ;
        aListOfComputers[ size ] = Resources::Computer::_duplicate( aComputer ) ;
        MESSAGE("Manager_Impl::GetComputers() " << aComputer->FullName() << " selected" ) ;
      }
    }
  }
  MESSAGE("Manager_Impl::GetComputers() " << aListOfComputers->length() << " found" ) ;
  return aListOfComputers._retn() ;
}

Resources::Computer_ptr Manager_Impl::SelectComputer( const Containers::MachineParameters & aMachineParameters ) {
  MESSAGE("Manager_Impl::SelectComputer()") ;
  Resources::ListOfComputers_var aListOfComputers = GetComputers( aMachineParameters ) ;
  MESSAGE("Manager_Impl::SelectComputer()") ;
  return GetComputer( aListOfComputers ) ;
}

Resources::Computer_ptr Manager_Impl::GetComputer( const Resources::ListOfComputers & aListOfComputers ) {
  MESSAGE("Manager_Impl::GetComputer()") ;
  Resources::Computer_var aComputer = Resources::Computer::_nil() ;
  if ( aListOfComputers.length() ) {
    aComputer = aListOfComputers[ 0 ] ;
  }
  MESSAGE("Manager_Impl::GetComputer()") ;
  return Resources::Computer::_duplicate( aComputer ) ;
}

Resources::Computer_ptr Manager_Impl::SearchComputer( const char * aComputerName ) {
  MESSAGE("Manager_Impl::SearchComputer()") ;
  Resources::Computer_var aComputer = Resources::Computer::_nil() ;
  int i = _MapOfAliasComputers[ aComputerName ] ;
  if ( i ) {
    aComputer = _ComputersList[ i - 1 ] ;
    MESSAGE("Manager_Impl::SearchComputer " << aComputerName << " found in MapOfAliasComputers") ;
  }
  else {
    MESSAGE("Manager_Impl::SearchComputer " << aComputerName << " NOT found in MapOfAliasComputers") ;
  }
  i = _MapOfFullNameComputers[ aComputerName ] ;
  if ( i ) {
    aComputer = _ComputersList[ i - 1 ] ;
    MESSAGE("Manager_Impl::SearchComputer " << aComputerName << " found in MapOfFullNameComputers") ;
  }
  else {
    MESSAGE("Manager_Impl::SearchComputer " << aComputerName << " NOT found in MapOfFullNameComputers") ;
  }
  return Resources::Computer::_duplicate( aComputer ) ;
}

//Resources::ListOfComputers_var Manager_Impl::ParseXmlFile( const char * xmlfile )  {
void Manager_Impl::ParseXmlFile( const char * xmlfile )  {
  ResourcesManager_Handler * MyHandler = new ResourcesManager_Handler( _Orb , _Poa );
  QFile xmlFile( xmlfile ) ;
  QXmlInputSource source( xmlFile ) ;
  QXmlSimpleReader reader ;
  reader.setContentHandler( MyHandler ) ;
  reader.setErrorHandler( MyHandler ) ;
  reader.parse( source ) ;
//  Resources::ListOfComputers_var aListOfComputers = MyHandler->ListOfComputers() ;
  _ComputersVector = MyHandler->ComputersVector() ;
  xmlFile.close() ;
  MESSAGE("Manager_Impl::ParseXmlFile()" << _ComputersVector.size() << " computers" ) ;
//  return aListOfComputers ;
}

bool Manager_Impl::ParseArguments( int argc , char ** argv , char ** path) {
  bool RetVal = true;
  *path = NULL;
  for ( int ind = 0 ; ind < argc ; ind++ ) {
    if ( strcmp( argv[ind] , "-help" ) == 0 ) {
      INFOS( "Usage: " << argv[0] << " -common 'path to ressources catalog' -ORBInitRef NameService=corbaname::localhost");
      RetVal = false ;
    }
    if ( strcmp( argv[ind] ,"-common" ) == 0 ) {
      if ( ind + 1 <= argc ) {
	*path = argv[ind + 1] ;
	ifstream path_file(*path);
	if ( !path_file ) {
	  MESSAGE( "Sorry the file " << *path << " can't be open" )
	  *path = NULL ;
	  RetVal = false;
	}
      }
    }
  }
  return RetVal ;
}

