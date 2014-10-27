// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Author : Anthony GEAY (EDF R&D)

#include "SALOMESDS_AnyDataServer.hxx"

using namespace SALOMESDS;

AnyDataServer::AnyDataServer(const std::string& varName):BasicDataServer(varName),_data(new CORBA::Any)
{
}

/*!
 * Called remotely -> to protect against throw
 */
void AnyDataServer::setValueOf(const CORBA::Any& newValue)
{
  delete _data;
  _data=new CORBA::Any(newValue);
}

/*!
 * Called remotely -> to protect against throw
 */
CORBA::Any *AnyDataServer::getValueOf()
{
  return new CORBA::Any(*_data);
}
