//  ResourcesComputer_Impl
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
//  File   : ResourcesComputer_Impl.cxx
//  Author : Jean Rahuel
//  Module : SALOME

using namespace std;
#include "ResourcesComputer_Impl.hxx"
#include <fstream>

Computer_Impl::Computer_Impl() {
}

Computer_Impl::Computer_Impl( CORBA::ORB_ptr orb ,
		              PortableServer::POA_ptr poa ,
                              Resources::ComputerParameters  * aComputerParameters ,
                              Resources::ComputerEnvironment * aComputerEnvironment ) {
  _Orb = CORBA::ORB::_duplicate( orb ) ;
  _Poa = PortableServer::POA::_duplicate( poa ) ;
  _Id = _Poa->activate_object( this ) ;
  _ComputerParameters = new Resources::ComputerParameters() ;
  _ComputerParameters->FullName = CORBA::string_dup( aComputerParameters->FullName ) ;
  _ComputerParameters->Alias = CORBA::string_dup( aComputerParameters->Alias ) ;
  _ComputerParameters->SshAccess = aComputerParameters->SshAccess ;
  _ComputerParameters->Interactive = aComputerParameters->Interactive ;
  _ComputerParameters->Batch = aComputerParameters->Batch ;
  _ComputerParameters->UserName = CORBA::string_dup( aComputerParameters->UserName ) ;
  _ComputerParameters->Os = aComputerParameters->Os ;
  _ComputerParameters->Memory = aComputerParameters->Memory ;
  _ComputerParameters->Swap = aComputerParameters->Swap ;
  _ComputerParameters->CpuClock = aComputerParameters->CpuClock ;
  _ComputerParameters->NbProc = aComputerParameters->NbProc ;
  _ComputerParameters->NbNode = aComputerParameters->NbNode ;
  _ComputerEnvironment = new Resources::ComputerEnvironment() ;
  _ComputerEnvironment->Path = CORBA::string_dup( aComputerEnvironment->Path ) ;
  _ComputerEnvironment->Ld_Library_Path = CORBA::string_dup( aComputerEnvironment->Ld_Library_Path ) ;
  _ComputerEnvironment->PythonPath = CORBA::string_dup( aComputerEnvironment->PythonPath ) ;
  _ComputerEnvironment->CasRoot = CORBA::string_dup( aComputerEnvironment->CasRoot ) ;
  int size = aComputerEnvironment->Module_Root_Dir_Names.length() ;
  _ComputerEnvironment->Module_Root_Dir_Names.length( size ) ;
  _ComputerEnvironment->Module_Root_Dir_Values.length( size ) ;
  int i ;
  for ( i = 0 ; i < size ; i++ ) {
    _ComputerEnvironment->Module_Root_Dir_Names[ i ] = CORBA::string_dup( aComputerEnvironment->Module_Root_Dir_Names[ i ] ) ;
    _ComputerEnvironment->Module_Root_Dir_Values[ i ] = CORBA::string_dup( aComputerEnvironment->Module_Root_Dir_Values[ i ] ) ;
  }
  cout << "Computer_Impl::Computer_Impl FullName " << _ComputerParameters->FullName << endl ;
  cout << "Computer_Impl::Computer_Impl Alias " << _ComputerParameters->Alias << endl ;
  cout << "Computer_Impl::Computer_Impl SshAccess " << _ComputerParameters->SshAccess << endl ;
  cout << "Computer_Impl::Computer_Impl Interactive " << _ComputerParameters->Interactive << endl ;
  cout << "Computer_Impl::Computer_Impl Batch " << _ComputerParameters->Batch << endl ;
  cout << "Computer_Impl::Computer_Impl UserName " << _ComputerParameters->UserName << endl ;
  cout << "Computer_Impl::Computer_Impl Os " << _ComputerParameters->Os << endl ;
  cout << "Computer_Impl::Computer_Impl Memory " << _ComputerParameters->Memory << endl ;
  cout << "Computer_Impl::Computer_Impl Swap " << _ComputerParameters->Swap << endl ;
  cout << "Computer_Impl::Computer_Impl CpuClock " << _ComputerParameters->CpuClock << endl ;
  cout << "Computer_Impl::Computer_Impl NbProc " << _ComputerParameters->NbProc << endl ;
  cout << "Computer_Impl::Computer_Impl NbNode " << _ComputerParameters->NbNode << endl ;
  for ( i = 0 ; i < size ; i++ ) {
    cout << "Computer_Impl::Computer_Impl Module_Root_Dir_Names[" << i << " ] " << _ComputerEnvironment->Module_Root_Dir_Names[ i ] << endl ;
    cout << "Computer_Impl::Computer_Impl Module_Root_Dir_Values[" << i << " ] " << _ComputerEnvironment->Module_Root_Dir_Values[ i ] << endl ;
  }
  cout << "Computer_Impl::Computer_Impl Path " << _ComputerEnvironment->Path << endl ;
  cout << "Computer_Impl::Computer_Impl Ld_Library_Path " << _ComputerEnvironment->Ld_Library_Path << endl ;
  cout << "Computer_Impl::Computer_Impl PythonPath " << _ComputerEnvironment->PythonPath << endl ;
  cout << "Computer_Impl::Computer_Impl CasRoot " << _ComputerEnvironment->CasRoot << endl ;
}

Computer_Impl::~Computer_Impl() {
}

PortableServer::ObjectId * Computer_Impl::getId() {
  return _Id ;
}

long Computer_Impl::ping() {
  MESSAGE("Computer_Impl::ping()") ;
  return 1 ;
}

char * Computer_Impl::FullName() {
  MESSAGE("Computer_Impl::FullName()") ;
  return CORBA::string_dup( _ComputerParameters->FullName ) ;
}

char * Computer_Impl::Alias() {
  MESSAGE("Computer_Impl::Alias()") ;
  return CORBA::string_dup( _ComputerParameters->Alias ) ;
}

Resources::ComputerParameters * Computer_Impl::Parameters() {
  Resources::ComputerParameters_var aComputerParameters = new Resources::ComputerParameters() ;
  aComputerParameters->FullName = CORBA::string_dup( _ComputerParameters->FullName ) ;
  aComputerParameters->Alias = CORBA::string_dup( _ComputerParameters->Alias ) ;
  aComputerParameters->SshAccess = _ComputerParameters->SshAccess ;
  aComputerParameters->Interactive = _ComputerParameters->Interactive ;
  aComputerParameters->Batch = _ComputerParameters->Batch ;
  aComputerParameters->UserName = CORBA::string_dup( _ComputerParameters->UserName ) ;
  aComputerParameters->Os = _ComputerParameters->Os ;
  aComputerParameters->Memory = _ComputerParameters->Memory ;
  aComputerParameters->Swap = _ComputerParameters->Swap ;
  aComputerParameters->CpuClock = _ComputerParameters->CpuClock ;
  aComputerParameters->NbProc = _ComputerParameters->NbProc ;
  aComputerParameters->NbNode = _ComputerParameters->NbNode ;
  return aComputerParameters._retn() ;
}

Resources::ComputerEnvironment * Computer_Impl::Environment() {
  Resources::ComputerEnvironment_var aComputerEnvironment = new Resources::ComputerEnvironment() ;
  aComputerEnvironment->Path = CORBA::string_dup( _ComputerEnvironment->Path ) ;
  aComputerEnvironment->Ld_Library_Path = CORBA::string_dup( _ComputerEnvironment->Ld_Library_Path ) ;
  aComputerEnvironment->PythonPath = CORBA::string_dup( _ComputerEnvironment->PythonPath ) ;
  aComputerEnvironment->CasRoot = CORBA::string_dup( _ComputerEnvironment->CasRoot ) ;
  int size = _ComputerEnvironment->Module_Root_Dir_Names.length() ;
  aComputerEnvironment->Module_Root_Dir_Names.length( size ) ;
  aComputerEnvironment->Module_Root_Dir_Values.length( size ) ;
  int i ;
  for ( i = 0 ; i < size ; i++ ) {
    aComputerEnvironment->Module_Root_Dir_Names[ i ] = CORBA::string_dup( _ComputerEnvironment->Module_Root_Dir_Names[ i ] ) ;
    aComputerEnvironment->Module_Root_Dir_Values[ i ] = CORBA::string_dup( _ComputerEnvironment->Module_Root_Dir_Values[ i ] ) ;
  }
  return aComputerEnvironment._retn() ;
}

