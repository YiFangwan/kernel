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

SALOME_Notebook::SALOME_Notebook()
{
}

void SALOME_Notebook::AddDependency( SALOME::ParameterizedObject_ptr obj, SALOME::ParameterizedObject_ptr ref )
{
  std::string anObjEntry = obj->GetEntry(), aRefEntry = ref->GetEntry();
  std::list<std::string>& aList = myDependencies[anObjEntry];
  if( find( aList.begin(), aList.end(), aRefEntry ) == aList.end() )
    aList.push_back( aRefEntry );
}

void SALOME_Notebook::RemoveDependency( SALOME::ParameterizedObject_ptr obj, SALOME::ParameterizedObject_ptr ref )
{
  std::string anObjEntry = obj->GetEntry(), aRefEntry = ref->GetEntry();
  std::map< std::string, std::list<std::string> >::iterator it = myDependencies.find( anObjEntry );
  if( it!=myDependencies.end() )
    it->second.remove( aRefEntry );
}

void SALOME_Notebook::ClearDependencies( SALOME::ParameterizedObject_ptr obj )
{
  myDependencies.erase( obj->GetEntry() );
}

void SALOME_Notebook::Update( SALOME::ParameterizedObject_ptr obj )
{
}
