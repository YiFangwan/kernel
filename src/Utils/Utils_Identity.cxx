//  SALOME Utils : general SALOME's definitions and tools
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
//  File   : Utils_Identity.cxx
//  Author : Pascale NOYRET, EDF
//  Module : SALOME
//  $Header$

# include <iostream>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/types.h>
# include <netdb.h>
# include "utilities.h"
# include "Utils_Identity.hxx"
using namespace std;
extern "C"
{
# include <string.h>
# include <pwd.h>
}

const char* duplicate( const char *const str ) ;

const struct utsname get_uname( void )
{
	struct utsname		hostid;
	const int retour=uname(&hostid);
	ASSERT(retour>=0);
	return hostid ;
}

const char* get_adip( void )
{
	struct utsname	hostid;
	const int retour=uname(&hostid);
	ASSERT(retour>=0);

	const hostent* pour_adip=gethostbyname(hostid.nodename);
	ASSERT(pour_adip!=NULL);
	const in_addr ip_addr=*(struct in_addr*)(pour_adip->h_addr) ;
	return duplicate(inet_ntoa(ip_addr));
}
const char* const get_pwname( void )
{
	struct passwd *papa = getpwuid( getuid() ) ;
	return papa->pw_name ;
}

Identity::Identity( const char *name ):	_name(duplicate(name)),\
			 				_hostid(get_uname()),\
							_adip(get_adip()),\
			 				_uid(getuid()) ,\
			 				_pwname(get_pwname()) ,\
							_dir(getcwd(NULL,256)),\
			 				_pid(getpid()) ,\
							_start(time(NULL)),\
							_cstart(ctime(&_start))
//CCRT							_dir(getcwd(NULL,0)),\

{
	ASSERT(strlen(_dir)<256);
}


Identity::~Identity(void)
{
	delete [] (char*)_name ;
	(char*&)_name = NULL ;

	delete [] (char*)_dir ;
	(char*&)_dir = NULL ;

	delete [] (char*)_adip ;
	(char*&)_adip = NULL ;
}

/*------------*/
/* Accessors  */
/*------------*/

const char* const Identity::name (void) const
{
	return  _name ;
}
const pid_t &Identity::pid(void) const
{
	return _pid ;
}
const struct utsname &Identity::hostid(void) const
{
	return _hostid ;
}
const uid_t &Identity::uid(void) const
{
	return _uid ;
}
const time_t &Identity::start(void) const
{
	return _start ;
}
const char* const Identity::rep (void) const
{
	return  _dir ;
}
const char* const Identity::pwname (void) const
{
	return  _pwname ;
}
const char* const Identity::adip (void) const
{
	return _adip ;
}

/*------------------*/
/* Other methods    */
/*------------------*/

const char* Identity::host_char( void ) const
{
	return _hostid.nodename;
}

const char* Identity::start_char(void) const
{
	return ctime(&_start) ;
}

ostream & operator<< ( ostream& os , const Identity& monid )
{
	ASSERT(monid._name!=NULL) ;
	os << "Identity :" << endl ;
	os << '\t' << "Component name : " << monid._name << endl ;
	os << '\t' << "Numero de PID :  " << monid._pid << endl;
	os << '\t' << "Uid utilisateur  : "   << monid._uid << endl;
	os << '\t' << "nom utilisateur  : "   << monid._pwname << endl;
	os << '\t' << "Nom de machine : " << (monid._hostid).nodename << endl;
	os << '\t' << "Adresse IP : " << monid._adip << endl;
	os << '\t' << "Heure de lancement : " << monid._cstart ; //ctime(&monid._start) ;
	os << '\t' << "Dans le repertoire : " << monid._dir << endl;

	return os ;
}
