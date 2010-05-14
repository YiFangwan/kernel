//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

// --- include all Unit Test from basics until the present directory
//
#include "SALOMELocalTraceTest.hxx"
#include "SALOMETraceCollectorTest.hxx"
#include "UtilsTest.hxx"
#include "NamingServiceTest.hxx"

// --- Registers the fixture into the 'registry'

CPPUNIT_TEST_SUITE_REGISTRATION( SALOMELocalTraceTest );
CPPUNIT_TEST_SUITE_REGISTRATION( SALOMETraceCollectorTest );
CPPUNIT_TEST_SUITE_REGISTRATION( UtilsTest );
CPPUNIT_TEST_SUITE_REGISTRATION( NamingServiceTest );

// --- generic Main program from Basic/Test

#include "BasicMainTest.hxx"
