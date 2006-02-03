// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
#include "SALOMEDS_IParameters.hxx"
#include <utilities.h>

using namespace std;

#define PT_INTEGER   0
#define PT_REAL      1
#define PT_BOOLEAN   2
#define PT_STRING    3
#define PT_REALARRAY 4
#define PT_INTARRAY  5
#define PT_STRARRAY  6


/*!
  Constructor
*/
SALOMEDS_IParameters::SALOMEDS_IParameters(const _PTR(AttributeParameter)& ap)
{
  _ap = ap;
}


int SALOMEDS_IParameters::append(const string& listName, const string& value)
{
  return -1;
}

int SALOMEDS_IParameters::nbValues(const string& listName)
{
  return -1;
}

vector<string> SALOMEDS_IParameters::getValues(const string& listName)
{
  vector<string> v;
  return v;
}


string SALOMEDS_IParameters::getValue(const string& listName, int index)
{
  return "";
}


void SALOMEDS_IParameters::setParameter(const string& entry, const string& parameterName, const string& value)
{
  
}


string SALOMEDS_IParameters::getParameter(const string& entry, const string& parameterName)
{
  return "";
}


void SALOMEDS_IParameters::getAllParameters(const string& entry,  vector<string>& names, vector<string>& values)
{
}


int SALOMEDS_IParameters::getNbParameters(const string& entry)
{
  return  -1;
}

