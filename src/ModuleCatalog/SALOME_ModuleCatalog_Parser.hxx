//  SALOME ModuleCatalog : implementation of ModuleCatalog server which parsers xml description of modules
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
//  File   : SALOME_ModuleCatalog_Parser.hxx
//  Author : Estelle Deville
//  Module : SALOME
//  $Header$

#ifndef SALOME_CATALOG_PARSER_H
#define SALOME_CATALOG_PARSER_H

#include <string>
#include <vector>

// Type Definitions
struct ParserPathPrefix
{
  string         path;
  vector<string> listOfComputer ;
};

typedef vector<ParserPathPrefix> ListOfParserPathPrefix ;

enum ParserComponentType {GEOM, MESH, Med, SOLVER, DATA, VISU, SUPERV, OTHER} ;

struct ParserServParam
{
  string parserParamType;
  string parserParamName;
} ;


struct ParserServDataStreamParam 
{
  string parserParamType;
  string parserParamName;
  string parserParamDependency;
} ;

typedef vector<ParserServParam> ListOfParserServicesParameter;
typedef vector<ParserServDataStreamParam> ListOfParserServicesDataStreamParameter;

struct ParserService
{
  string                        parserServiceName;
  ListOfParserServicesParameter parserServiceInParameter;
  ListOfParserServicesParameter parserServiceOutParameter;
  ListOfParserServicesDataStreamParameter parserServiceInDataStreamParameter;
  ListOfParserServicesDataStreamParameter parserServiceOutDataStreamParameter;
  bool                          parserServiceByDefault;
} ;

typedef vector<ParserService> ListOfParserServices ;

struct ParserDefInterface
{
  string               parserInterfaceName ;
  ListOfParserServices parserInterfaceServiceList ;
} ;

typedef vector<ParserDefInterface> ListOfDefinitionInterface;

struct ParserComponent
{
  string parserComponentName;
  string parserComponentUsername;
  ParserComponentType parserComponentType;
  bool parserComponentMultistudy ;
  string parserComponentIcon;
  ListOfDefinitionInterface parserListInterface;
  string parserConstraint ;
};

typedef vector<ParserComponent> ListOfParserComponent ;

#ifdef WRITE_CATA_COMPONENT
// contains all the paths and the computers defined in the catalog
ListOfParserPathPrefix _pathList;
  
// contains all the modules defined in the catalog
ListOfParserComponent _moduleList; 
#else
extern ListOfParserPathPrefix _pathList;
extern ListOfParserComponent _moduleList; 
#endif


inline void DebugParserService(const ParserService &S)
{
  MESSAGE("Service name : " << S.parserServiceName);
  MESSAGE("Service default : " << S.parserServiceByDefault);
  
  for (unsigned int k=0; k<S.parserServiceInParameter.size() ; k++)
    {
      const ParserServParam & P = S.parserServiceInParameter[k];
      MESSAGE("Service Parameter in name : " << P.parserParamName);
      MESSAGE("Service Parameter in type : " << P.parserParamType);
    }
  for (unsigned int k=0; k<S.parserServiceOutParameter.size() ; k++)
    {
      const ParserServParam & P = S.parserServiceOutParameter[k];
      MESSAGE("Service Parameter out name : " << P.parserParamName);
      MESSAGE("Service Parameter out type : " << P.parserParamType);
    }
  
  for (unsigned int k=0; k<S.parserServiceInDataStreamParameter.size() ; k++)
    {
      const ParserServDataStreamParam & P = S.parserServiceInDataStreamParameter[k];
      MESSAGE("Service DataStreamParameter in name : " << P.parserParamName);
      MESSAGE("Service DataStreamParameter in type : " << P.parserParamType);
    }
  for (unsigned int k=0; k<S.parserServiceOutDataStreamParameter.size() ; k++)
    {
      const ParserServDataStreamParam & P = S.parserServiceOutDataStreamParameter[k];
      MESSAGE("Service DataStreamParameter out name : " << P.parserParamName);
      MESSAGE("Service DataStreamParameter out type : " << P.parserParamType);
    }
}

inline void DebugParserComponent(const ParserComponent &P)
{
  MESSAGE("Component name : " << P.parserComponentName);
  MESSAGE("Component type : " << P.parserComponentType);
  MESSAGE("Component constraint : " << P.parserConstraint);
  MESSAGE("Component icone : " << P.parserComponentIcon);
  for (unsigned int i = 0; i < P.parserListInterface.size(); i++)
    {
      const ParserDefInterface &L = P.parserListInterface[i];
      
      MESSAGE("Component interface name : " << L.parserInterfaceName);
      for (unsigned int j=0; j<L.parserInterfaceServiceList.size(); j++)
	DebugParserService(L.parserInterfaceServiceList[j]);
    }
};

#endif // SALOME_CATALOG_PARSER_H
