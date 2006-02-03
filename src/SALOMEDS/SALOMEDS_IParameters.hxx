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
#ifndef SALOMEDS_IParameters_H
#define SALOMEDS_IParameters_H

#include <string>
#include <vector>

#include "SALOMEDSClient_AttributeParameter.hxx"

/*! 
  Class which an interface to store the parameters of the objects
*/
class SALOMEDS_IParameters
{
public:
  SALOMEDS_IParameters(const _PTR(AttributeParameter)& ap); 

  /*!
    Appends a string value to a named list. 
    Returns a number of the added value.
    Note: the name of the list MUST be unique
   */
  int append(const std::string& listName, const std::string& value); 

  /*!
    Returns a number elements in the list
   */
  int nbValues(const std::string& listName);

  /*!
    Returns a list of values in the list
   */
  std::vector<std::string> getValues(const std::string& listName);

  /*!
    Returns a value with given index
   */
  std::string getValue(const std::string& listName, int index);

  /*!
    Sets a new named parameter value for the given entry
   */
  void setParameter(const std::string& entry, const std::string& parameterName, const std::string& value);

  /*!
    Gets a named parameter value for the given entry
   */
  std::string getParameter(const std::string& entry, const std::string& parameterName);

  /*!
    Returns all parameter names and their values of the given entry
   */
  void getAllParameters(const std::string& entry,  std::vector<std::string>& names, std::vector<std::string>& values);

  /*!
    Returns a number of parameters of the given entry
   */
  int getNbParameters(const std::string& entry);

private:
  _PTR(AttributeParameter) _ap;

};


#endif
