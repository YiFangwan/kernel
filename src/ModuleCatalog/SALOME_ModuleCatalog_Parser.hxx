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
  std::string         path;
  std::vector<std::string> listOfComputer ;
};

typedef std::vector<ParserPathPrefix> ListOfParserPathPrefix ;

enum ParserComponentType {GEOM, MESH, Med, SOLVER, DATA, VISU, SUPERV, OTHER} ;

struct ParserServParam
{
  std::string parserParamType;
  std::string parserParamName;
} ;


struct ParserServDataStreamParam 
{
  std::string parserParamType;
  std::string parserParamName;
  std::string parserParamDependency;
} ;

typedef std::vector<ParserServParam> ListOfParserServicesParameter;
typedef std::vector<ParserServDataStreamParam> ListOfParserServicesDataStreamParameter;

struct ParserService
{
  std::string                        parserServiceName;
  ListOfParserServicesParameter parserServiceInParameter;
  ListOfParserServicesParameter parserServiceOutParameter;
  ListOfParserServicesDataStreamParameter parserServiceInDataStreamParameter;
  ListOfParserServicesDataStreamParameter parserServiceOutDataStreamParameter;
  bool                          parserServiceByDefault;
} ;

typedef std::vector<ParserService> ListOfParserServices ;

struct ParserDefInterface
{
  std::string               parserInterfaceName ;
  ListOfParserServices parserInterfaceServiceList ;
} ;

typedef std::vector<ParserDefInterface> ListOfDefinitionInterface;

struct ParserComponent
{
  std::string parserComponentName;
  std::string parserComponentUsername;
  ParserComponentType parserComponentType;
  bool parserComponentMultistudy ;
  std::string parserComponentIcon;
  ListOfDefinitionInterface parserListInterface;
  std::string parserConstraint ;
};

typedef std::vector<ParserComponent> ListOfParserComponent ;

#ifdef WRITE_CATA_COMPONENT
// contains all the paths and the computers defined in the catalog
ListOfParserPathPrefix _pathList;
  
// contains all the modules defined in the catalog
ListOfParserComponent _moduleList; 
#else
extern ListOfParserPathPrefix _pathList;
extern ListOfParserComponent _moduleList; 
#endif


#endif // SALOME_CATALOG_PARSER_H
