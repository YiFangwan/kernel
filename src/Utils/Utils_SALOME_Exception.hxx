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
//  File   : Utils_SALOME_Exception.hxx
//  Author : Antoine YESSAYAN, EDF
//  Module : SALOME
//  $Header$

#if !defined( __Utils_SALOME_Exception_hxx__ )
#define __Utils_SALOME_Exception_hxx__

# include <exception>
# include <iostream>

using namespace std;

# define LOCALIZED(message) #message , __FILE__ , __LINE__

class SALOME_Exception : public exception
{

private :
	SALOME_Exception( void );

protected :
	const char* _text ;	// pointeur constant et zone pointee constante !!!!

public :
	SALOME_Exception( const char *text, const char *fileName=0, const unsigned int lineNumber=0 );
	SALOME_Exception( const SALOME_Exception &ex );
	~SALOME_Exception() throw ();
	friend ostream & operator<<( ostream &os , const SALOME_Exception &ex );
	virtual const char *what( void ) const throw () ;
} ;


#endif		/* #if !defined( __Utils_SALOME_Exception_hxx__ ) */
