//  SALOME ResourcesManager_Handler
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
//  File   : ResourcesManager_Handler.cxx
//  Author : Jean Rahuel
//  Module : SALOME
//$Header: 

#define TRACE 0

#include "ResourcesManager_Handler.hxx"

using namespace std;

ResourcesManager_Handler::ResourcesManager_Handler( CORBA::ORB_ptr orb ,
		                                    PortableServer::POA_ptr poa ) {
  MESSAGE("ResourcesManager_Handler creation") ;
  _Orb = CORBA::ORB::_duplicate( orb ) ;
  _Poa = PortableServer::POA::_duplicate( poa ) ;
  ResetParameters() ;
}

ResourcesManager_Handler::~ResourcesManager_Handler() {
  MESSAGE("~ResourcesManager_Handler");
}

//Resources::ListOfComputers * ResourcesManager_Handler::ListOfComputers() {
//  MESSAGE("ResourcesManager_Handler::ListOfComputers() " << _ComputersList->length() << " computers" ) ;
//  return _ComputersList._retn() ;
//}

vector< Computer_Impl * > ResourcesManager_Handler::ComputersVector() {
  MESSAGE("ResourcesManager_Handler::ComputersVector() " << _ComputersVector.size() << " computers" ) ;
  return _ComputersVector;
}

void ResourcesManager_Handler::ResetParameters() {
  _ComputerParameters.FullName = CORBA::string_dup( "" ) ;
  _ComputerParameters.Alias = CORBA::string_dup( "" ) ;
  _ComputerParameters.SshAccess = 0 ;
  _ComputerParameters.Interactive = 0 ;
  _ComputerParameters.Batch = 0 ;
  _ComputerParameters.UserName = CORBA::string_dup( "" ) ;
  _ComputerParameters.Os = Containers::Unknown ;
  _ComputerParameters.Memory = 0 ;
  _ComputerParameters.Swap = 0 ;
  _ComputerParameters.CpuClock = 0 ;
  _ComputerParameters.NbProc = 0 ;
  _ComputerParameters.NbNode = 0 ;
//  _ComputerEnvironment.Path = CORBA::string_dup( "" ) ;
//  _ComputerEnvironment.Ld_Library_Path = CORBA::string_dup( "" ) ;
//  _ComputerEnvironment.PythonPath = CORBA::string_dup( "" ) ;
//  _ComputerEnvironment.CasRoot = CORBA::string_dup( "" ) ;
  _ComputerEnvironment.Module_Root_Dir_Names.length( 0 ) ;
  _ComputerEnvironment.Module_Root_Dir_Values.length( 0 ) ;
}

Containers::System ResourcesManager_Handler::OsEnumValue( const string anOsName ) {
  if ( anOsName == "Linux" ) {
    return Containers::Linux ;
  }
  return Containers::Unknown ;
}

bool ResourcesManager_Handler::startDocument() {
  MESSAGE("Begin parse document");
  return true;
}

bool ResourcesManager_Handler::startElement( const QString&, 
					     const QString&,
					     const QString& qName, 
					     const QXmlAttributes& atts) {
  return true;
}

bool ResourcesManager_Handler::endElement( const QString& , const QString& ,
					   const QString& qName ) {
  if ( qName.compare( QString( "fullname" ) ) == 0 ) {
#if TRACE
    cout << "fullname " << content.c_str() << endl ;
#endif
    _ComputerParameters.FullName = CORBA::string_dup( content.c_str() ) ;
  }
  else if ( qName.compare(QString( "alias" ) ) == 0 ) {
#if TRACE
    cout << "alias " << content.c_str() << endl ;
#endif
    _ComputerParameters.Alias = CORBA::string_dup( content.c_str() ) ;
  }
  else if ( qName.compare(QString( "sshaccess" ) ) == 0 ) {
#if TRACE
    cout << "sshaccess " << content.c_str() << endl ;
#endif
    _ComputerParameters.SshAccess = atol( content.c_str() ) ;
  }
  else if ( qName.compare(QString( "interactive" ) ) == 0 ) {
#if TRACE
    cout << "interactive " << content.c_str() << endl ;
#endif
    _ComputerParameters.Interactive = atol( content.c_str() ) ;
  }
  else if ( qName.compare(QString( "batch" ) ) == 0 ) {
#if TRACE
    cout << "batch " << content.c_str() << endl ;
#endif
    _ComputerParameters.Batch = atol( content.c_str() ) ;
  }
  else if ( qName.compare(QString( "username" ) ) == 0 ) {
#if TRACE
    cout << "username " << content.c_str() << " length " << content.length() << endl ;
#endif
    if ( content[0] != '\n' ) {
      _ComputerParameters.UserName = CORBA::string_dup( content.c_str() ) ;
    }
    else {
      _ComputerParameters.UserName = CORBA::string_dup( "" ) ;
    }
  }
  else if ( qName.compare(QString( "os" ) ) == 0 ) {
#if TRACE
    cout << "os " << content.c_str() << endl ;
#endif
    _ComputerParameters.Os = OsEnumValue( content ) ;
  }
  else if ( qName.compare(QString( "memory" ) ) == 0 ) {
#if TRACE
    cout << "memory " << content.c_str() << endl ;
#endif
    _ComputerParameters.Memory = atol( content.c_str() ) ;
  }
  else if ( qName.compare(QString( "swap" ) ) == 0 ) {
#if TRACE
    cout << "swap " << content.c_str() << endl ;
#endif
    _ComputerParameters.Swap = atol( content.c_str() ) ;
  }
  else if ( qName.compare(QString( "cpuclock" ) ) == 0 ) {
#if TRACE
    cout << "cpuclock " << content.c_str() << endl ;
#endif
    _ComputerParameters.CpuClock = atol( content.c_str() ) ;
  }
  else if ( qName.compare(QString( "nbproc" ) ) == 0 ) {
#if TRACE
    cout << "nbproc " << content.c_str() << endl ;
#endif
    _ComputerParameters.NbProc = atol( content.c_str() ) ;
  }
  else if ( qName.compare(QString( "nbnode" ) ) == 0 ) {
#if TRACE
    cout << "nbnode " << content.c_str() << endl ;
#endif
    _ComputerParameters.NbNode = atol( content.c_str() ) ;
  }

//  else if ( qName.compare(QString("path") ) == 0 ) {
//#if TRACE
//    cout << "path " << content.c_str() << endl ;
//#endif
//    _ComputerEnvironment.Path = CORBA::string_dup( content.c_str() ) ;
//  }
//  else if ( qName.compare(QString("ld_library_path") ) == 0 ) {
//#if TRACE
//    cout << "ld_library_path " << content.c_str() << endl ;
//#endif
//    _ComputerEnvironment.Ld_Library_Path = CORBA::string_dup( content.c_str() ) ;
//  }
//  else if ( qName.compare(QString("pythonpath") ) == 0 ) {
//#if TRACE
//    cout << "pythonpath " << content.c_str() << endl ;
//#endif
//    _ComputerEnvironment.PythonPath = CORBA::string_dup( content.c_str() ) ;
//  }
//  else if ( qName.compare(QString("casroot") ) == 0 ) {
//#if TRACE
//    cout << "casroot " << content.c_str() << endl ;
//#endif
//    _ComputerEnvironment.CasRoot = CORBA::string_dup( content.c_str() ) ;
//  }
  else if ( qName.compare(QString("modules_root_dir") ) == 0 ) {
#if TRACE
    cout << "modules_root_dir " << content.c_str() << endl ;
#endif
    int size = _ComputerEnvironment.Module_Root_Dir_Names.length() ;
    _ComputerEnvironment.Module_Root_Dir_Names.length( size + 1 ) ;
    _ComputerEnvironment.Module_Root_Dir_Names[ size ] = CORBA::string_dup( content.c_str() ) ;
  }
  else if ( qName.compare(QString("modules_root_path") ) == 0 ) {
#if TRACE
    cout << "modules_root_path " << content.c_str() << endl ;
#endif
    int size = _ComputerEnvironment.Module_Root_Dir_Values.length() ;
    _ComputerEnvironment.Module_Root_Dir_Values.length( size + 1 ) ;
    _ComputerEnvironment.Module_Root_Dir_Values[ size ] = CORBA::string_dup( content.c_str() ) ;
  }

  else if ( qName.compare(QString("computer") ) == 0 ) {
    Computer_Impl * aComputer = new Computer_Impl( _Orb , _Poa ,
                                                   &_ComputerParameters , &_ComputerEnvironment ) ;
    int size = _ComputersVector.size() ;
    _ComputersVector.resize( size + 1 ) ;
    _ComputersVector[ size ] = aComputer ;
    ResetParameters() ;
#if TRACE
    cout << size+1 << " computers" << endl ;
#endif
  }

  return true;
}

bool ResourcesManager_Handler::characters(const QString& chars) {
  content = (const char *)chars ;
  return true;
}

bool ResourcesManager_Handler::endDocument() {
  MESSAGE("End parse document");
 return true;
}

QString ResourcesManager_Handler::errorProtocol() {
  return errorProt;
}

bool ResourcesManager_Handler::fatalError(const QXmlParseException& exception) {
    errorProt += QString( "fatal parsing error: %1 in line %2, column %3\n" )
    .arg( exception.message() )
    .arg( exception.lineNumber() )
    .arg( exception.columnNumber() );

  return QXmlDefaultHandler::fatalError( exception );
}

