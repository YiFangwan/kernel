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
//  File   : SALOME_ModuleCatalog_impl.cxx
//  Author : Estelle Deville
//  Module : SALOME
//  $Header$

#include "SALOME_ModuleCatalog_impl.hxx"
#include "SALOME_ModuleCatalog_Acomponent_impl.hxx"
#include <fstream>
#include <map>

#include <qstringlist.h>
#include <qfileinfo.h>
using namespace std;

static const char* SEPARATOR    = ":";

//----------------------------------------------------------------------
// Function : SALOME_ModuleCatalogImpl
// Purpose  : Constructor 
//----------------------------------------------------------------------
SALOME_ModuleCatalogImpl::SALOME_ModuleCatalogImpl(int argc, char** argv)
{
  MESSAGE("Catalog creation");

  // Empty used variables
  _general_module_list.resize(0);
  _general_path_list.resize(0);

  _personal_module_list.resize(0);
  _personal_path_list.resize(0);

  // Parse the arguments given at server run
  if (!_parseArguments(argc, argv,&_general_path,&_personal_path))
    MESSAGE( "Error while argument parsing" )

  // Test existency of files
  if (_general_path == NULL)
    MESSAGE( "Error the general catalog should be indicated" )
  else
    {
      //MESSAGE("Parse general catalog");
      // Affect the variables _general_module_list and _general_path_list 
      // with the common catalog
      
      QStringList dirList = QStringList::split( SEPARATOR, _general_path, false ); // skip empty entries
      for ( int i = 0; i < dirList.count(); i++ ) {
	QFileInfo fileInfo( dirList[ i ] );
	if ( fileInfo.isFile() && fileInfo.exists() ) {
	  _parse_xml_file(fileInfo.filePath(),_general_module_list, _general_path_list);
	}
      }

      // Verification of _general_path_list content
      if(!_verify_path_prefix(_general_path_list))
	MESSAGE( "Error while parsing the general path list, differents pathes are associated to one computer, the first will be choosen" )
      else MESSAGE("General path list OK");
    
      if(_personal_path != NULL)
	{
	  //MESSAGE("Parse personal catalog");
	  // Affect the variables _personal_module_list and _personal_path_list 
	  // with the personal catalog
	  _parse_xml_file(_personal_path,_personal_module_list, _personal_path_list);
	  
	  // Verification of _general_path_list content
	  if(!_verify_path_prefix(_personal_path_list))
	    MESSAGE("Error while parsing the personal path list, differents pathes are associated to one computer, the first will be choosen" )
	  else MESSAGE("Personal path list OK");
	}
      else MESSAGE("No personal catalog indicated or error while opening the personal catalog");
    }
}

//----------------------------------------------------------------------
// Function : ~SALOME_ModuleCatalogImpl
// Purpose  : Destructor 
//----------------------------------------------------------------------
SALOME_ModuleCatalogImpl::~SALOME_ModuleCatalogImpl()
{
  MESSAGE("Catalog Destruction");
}


//----------------------------------------------------------------------
// Function : GetComputerList
// Purpose  : get a computer list
//----------------------------------------------------------------------
SALOME_ModuleCatalog::ListOfComputers* 
SALOME_ModuleCatalogImpl::GetComputerList()
{
  SALOME_ModuleCatalog::ListOfComputers_var _list_computers = 
    new SALOME_ModuleCatalog::ListOfComputers;
  return _list_computers._retn();
}

//----------------------------------------------------------------------
// Function : GetPathPrefix
// Purpose  : get the PathPrefix of a computer
//----------------------------------------------------------------------
char* 
SALOME_ModuleCatalogImpl::GetPathPrefix(const char* machinename) {
  MESSAGE("Begin of GetPathPrefix")
  // Variables initialisation
  char* _path = NULL;
  bool _find = false ;

  // Parse all the path prefixes
  // looking for the wanted computer
  for (unsigned int ind = 0 ; ind < _personal_path_list.size() ; ind++)
    {
      for (unsigned int ind1 = 0 ; ind1 < _personal_path_list[ind].listOfComputer.size() ; ind1++)    
	{
	  if (strcmp(machinename, _personal_path_list[ind].listOfComputer[ind1].c_str()) == 0)
	    {
	      _find = true ;
	      // Wanted computer
	      // affect the path to be returned
	        const char* _temp = _personal_path_list[ind].path.c_str() ;
		  _path = new char[strlen(_temp)+1];
	      strcpy(_path,_temp);
	    }
	}
    }

  if (!_find)
    {
    for (unsigned int ind = 0 ; ind < _general_path_list.size() ; ind++)
      {
        for (unsigned int ind1 = 0 ; ind1 < _general_path_list[ind].listOfComputer.size() ; ind1++)    
	  {
	    if (strcmp(machinename, _general_path_list[ind].listOfComputer[ind1].c_str()) == 0)
	      {
	        _find = true ;
	        // Wanted computer
	        // affect the path to be returned
	          const char* _temp = _general_path_list[ind].path.c_str() ;
		    _path = new char[strlen(_temp)+1];
	        strcpy(_path,_temp);
	      }
	  }
      }
    }

  return _path;
}

//----------------------------------------------------------------------
// Function : GetComponentList
// Purpose  : get a component list
//            If a component is defined in the personal catalog and 
//            in the general catalog (same name), the component defined
//            in the personal catalog is used
//----------------------------------------------------------------------
SALOME_ModuleCatalog::ListOfComponents* 
SALOME_ModuleCatalogImpl::GetComponentList()
{
  MESSAGE("Begin of GetComponentList");
  SALOME_ModuleCatalog::ListOfComponents_var _list_components = 
    new SALOME_ModuleCatalog::ListOfComponents;

  _list_components->length(_personal_module_list.size());

  // All the components defined in the personal catalog are taken
  for (unsigned int ind=0; ind < _personal_module_list.size();ind++)
    {
       _list_components[ind]=(_personal_module_list[ind].parserComponentName).c_str();
       SCRUTE(_list_components[ind]) ;
    }

  int indice = _personal_module_list.size() ;
  bool _find = false;
  
  // The components in the general catalog are taken only if they're
  // not defined in the personal catalog
  for (unsigned int ind=0; ind < _general_module_list.size();ind++)
    {
      _find = false;
      for (unsigned int ind1=0; ind1 < _personal_module_list.size();ind1++)
	{
	  // searching if the component is already defined in 
	  // the personal catalog
	  if ((_general_module_list[ind].parserComponentName.compare(_personal_module_list[ind1].parserComponentName)) == 0)
	    _find = true;
	}
      if (!_find)
	{
	  MESSAGE("A new component " << _general_module_list[ind].parserComponentName << " has to be to added in the list");
          _list_components->length(indice+1);
	  // The component is not already defined => has to be taken
	  _list_components[indice]=(_general_module_list[ind].parserComponentName).c_str();   
	  SCRUTE(_list_components[indice]) ;

	  indice++;
	}
      else 
	MESSAGE("The component " <<_general_module_list[ind].parserComponentName << " was already defined in the personal catalog") ;
     }

  MESSAGE ( "End of GetComponentList" )
  return _list_components._retn();
}


//----------------------------------------------------------------------
// Function : GetComponentIconeList
// Purpose  : get a component list of component name and component icone
//            If a component is defined in the personal catalog and 
//            in the general catalog (same name), the component defined
//            in the personal catalog is used
//----------------------------------------------------------------------
SALOME_ModuleCatalog::ListOfIAPP_Affich* 
SALOME_ModuleCatalogImpl::GetComponentIconeList()
{
  MESSAGE("Begin of GetComponentIconeList");

  SALOME_ModuleCatalog::ListOfIAPP_Affich_var _list_components_icone = 
    new SALOME_ModuleCatalog::ListOfIAPP_Affich;

  _list_components_icone->length(_personal_module_list.size());

  // All the components defined in the personal catalog are taken
  for (unsigned int ind=0; ind < _personal_module_list.size();ind++)
    {
       _list_components_icone[ind].modulename=(_personal_module_list[ind].parserComponentName).c_str();
       _list_components_icone[ind].moduleusername=(_personal_module_list[ind].parserComponentUsername).c_str();
       _list_components_icone[ind].moduleicone=(_personal_module_list[ind].parserComponentIcon).c_str();
       //SCRUTE(_list_components_icone[ind].modulename); 
       //SCRUTE(_list_components_icone[ind].moduleicone);
    }

  int indice = _personal_module_list.size() ;
  bool _find = false;
  
  // The components in the general catalog are taken only if they're
  // not defined in the personal catalog
  for (unsigned int ind=0; ind < _general_module_list.size();ind++)
    {
      _find = false;
      for (unsigned int ind1=0; ind1 < _personal_module_list.size();ind1++)
	{
	  // searching if the component is aleready defined in 
	  // the personal catalog
	  if ((_general_module_list[ind].parserComponentName.compare(_personal_module_list[ind1].parserComponentName)) == 0)
	    _find = true;
	}
      if (!_find)
	{
	  //	  MESSAGE("A new component " << _general_module_list[ind].parserComponentName << " has to be to added in the list");
          _list_components_icone->length(indice+1);
	  // The component is not already defined => has to be taken
	  _list_components_icone[indice].modulename=(_general_module_list[ind].parserComponentName).c_str();  
	  _list_components_icone[indice].moduleusername=(_general_module_list[ind].parserComponentUsername).c_str();  
	  _list_components_icone[indice].moduleicone=(_general_module_list[ind].parserComponentIcon).c_str(); 
	  //SCRUTE(_list_components_icone[indice].modulename) ;
	  //SCRUTE(_list_components_icone[indice].moduleicone);

	  indice++;
	}
      // else 
	//MESSAGE("The component " <<_general_module_list[ind].parserComponentName << " was already defined in the personal catalog"); 
     }

  return _list_components_icone._retn() ;
}

//----------------------------------------------------------------------
// Function : GetTypedComponentList
// Purpose  : get a component list of a wanted type
//            If a component is defined in the personal catalog and 
//            in the general catalog (same name), the component defined
//            in the personal catalog is used
//----------------------------------------------------------------------
SALOME_ModuleCatalog::ListOfComponents* 
SALOME_ModuleCatalogImpl::GetTypedComponentList(SALOME_ModuleCatalog::ComponentType component_type)
{
  MESSAGE("Begin of GetTypedComponentList");
  SALOME_ModuleCatalog::ListOfComponents_var _list_typed_component = 
    new SALOME_ModuleCatalog::ListOfComponents;
  int _j = 0;

  _list_typed_component->length(0);
  // Transform SALOME_ModuleCatalog::ComponentType in ParserComponentType
  ParserComponentType _temp_component_type;
  switch(component_type){
  case SALOME_ModuleCatalog::GEOM:
    _temp_component_type = GEOM ;
    break;
  case SALOME_ModuleCatalog::MESH:
    _temp_component_type = MESH;
    break;   
  case SALOME_ModuleCatalog::Med:
    _temp_component_type = Med;
    break;    
  case SALOME_ModuleCatalog::SOLVER:   
    _temp_component_type = SOLVER;
    break;
  case SALOME_ModuleCatalog::DATA:
    _temp_component_type = DATA;
    break;
  case SALOME_ModuleCatalog::VISU:
    _temp_component_type = VISU;
    break;  
  case SALOME_ModuleCatalog::SUPERV:
    _temp_component_type = SUPERV;
    break;
  case SALOME_ModuleCatalog::OTHER:
    _temp_component_type = OTHER;
    break;
  }

  // All the components in the personal catalog are taken
  for (unsigned int ind=0; ind < _personal_module_list.size();ind++)
    {
      if  (_personal_module_list[ind].parserComponentType == _temp_component_type)
	{
	  _list_typed_component->length(_j + 1); 
	  _list_typed_component[_j] = (_moduleList[ind].parserComponentName).c_str();
	  //SCRUTE(_list_typed_component[_j])
	  _j++;
	}
    }

  int indice = _list_typed_component->length() ;
  bool _find = false;
  
  // The components in the general catalog are taken only if they're
  // not defined in the personal catalog
  for (unsigned int ind=0; ind < _general_module_list.size();ind++)
    {
      _find = false;

      if(_general_module_list[ind].parserComponentType == _temp_component_type)
	{
	  for (unsigned int ind1=0; ind1 < _personal_module_list.size();ind1++)
	    {
	      // searching if the component is aleready defined in 
	      // the personal catalog
	      if ((_general_module_list[ind].parserComponentName.compare(_personal_module_list[ind1].parserComponentName)) == 0)
		_find = true;
	    }
	  if (!_find)
	    {
	      //MESSAGE("A new component " << _general_module_list[ind].parserComponentName << " has to be to added in the list");
              _list_typed_component->length(indice+1);
	      // The component is not already defined => has to be taken
	      _list_typed_component[indice]=(_general_module_list[ind].parserComponentName).c_str();   
	      //SCRUTE(_list_typed_component[indice]) ;

	      indice++;
	    }
	  //else 
	    //MESSAGE("The component " <<_general_module_list[ind].parserComponentName << " was already defined in the personal catalog") ;
        }
    }


  return _list_typed_component._retn();
}

//----------------------------------------------------------------------
// Function : GetComponent
// Purpose  : get a component 
//            If a component is defined in the personal catalog and 
//            in the general catalog (same name), the component defined
//            in the personal catalog is used
//----------------------------------------------------------------------
SALOME_ModuleCatalog::Acomponent_ptr 
SALOME_ModuleCatalogImpl::GetComponent(const char* componentname)
{
  SALOME_ModuleCatalog::Acomponent_ptr compo;
  SALOME_ModuleCatalog::ListOfDefInterface _list_interfaces;
  _list_interfaces.length(0);
  char* _constraint = NULL;
  char* _icone = NULL;
  char* _componentusername = NULL;
  SALOME_ModuleCatalog::ComponentType _componentType = SALOME_ModuleCatalog::OTHER; // default initialisation
  CORBA::Boolean _componentmultistudy = false ; // default initialisation
  ListOfPathPrefix _pathes ;
  _pathes.resize(0);
  
  // Looking for component named "componentname" in the personal catalog
  // If found, get name, interfaces and constraint
  // If not found, looking for component named "componentname" in
  // the general catalog
  // If found, get name, interfaces and constraint
  // If not found, NULL pointer is returned

  ParserComponent *p = NULL;
  ListOfParserPathPrefix *pp = NULL;

  for (unsigned int ind=0; ind < _personal_module_list.size();ind++)
    {
     if (strcmp((_personal_module_list[ind].parserComponentName).c_str(),componentname) == 0)
        {
          MESSAGE("Component named " << componentname << " found in the personal catalog");
	  p = &(_personal_module_list[ind]);
	  pp = &_personal_path_list;
	}
    }

  if (!p)
    for (unsigned int ind=0; ind < _general_module_list.size();ind++)
      {
	if (strcmp((_general_module_list[ind].parserComponentName).c_str(),componentname) == 0)
	  {
	    MESSAGE("Component named " << componentname << " found in the general catalog");
	    p = &(_general_module_list[ind]);
	    pp = &_general_path_list;
	  }
      }

  if (p) {
    
//    DebugParserComponent(*p);

    // get constraint
    _constraint = CORBA::string_dup(p->parserConstraint.c_str());
    
    // get component type
    switch(p->parserComponentType) {
    case GEOM:
      _componentType = SALOME_ModuleCatalog::GEOM;
      break;
    case MESH:
      _componentType = SALOME_ModuleCatalog::MESH;
      break;
    case Med:
      _componentType = SALOME_ModuleCatalog::Med;
      break;
    case SOLVER:
      _componentType = SALOME_ModuleCatalog::SOLVER;
      break;
    case DATA:
      _componentType = SALOME_ModuleCatalog::DATA;
      break;
    case VISU:
      _componentType = SALOME_ModuleCatalog::VISU;
      break;
    case SUPERV:
      _componentType = SALOME_ModuleCatalog::SUPERV;
      break;
    case OTHER:
      _componentType = SALOME_ModuleCatalog::OTHER;
      break;
    }
    
    // get component multistudy
    _componentmultistudy = p->parserComponentMultistudy ;
    
    // get component icone
    _icone = CORBA::string_dup(p->parserComponentIcon.c_str());
    
    // get component user name
    _componentusername = CORBA::string_dup(p->parserComponentUsername.c_str());
    
    // get component interfaces
    _list_interfaces = duplicate_interfaces(p->parserListInterface);
    
    // get pathes prefix
    _pathes = duplicate_pathes(*pp);
    
    SALOME_ModuleCatalog_AcomponentImpl* aComponentImpl = 
      new SALOME_ModuleCatalog_AcomponentImpl(componentname,
					      _componentusername,
					      _constraint,
					      _componentType,
					      _componentmultistudy,
					      _icone,
					      _list_interfaces,
					      _pathes);
    
    compo = aComponentImpl->_this();
  }
  else
    // Not found in the personal catalog and in the general catalog
    // return NULL object
    {
      MESSAGE("Component with name  " << componentname << " not found in catalog");
      compo = NULL;
    }

  return compo;
}

 
//----------------------------------------------------------------------
// Function : _parse_xml_file
// Purpose  : parse one module catalog 
//----------------------------------------------------------------------
void 
SALOME_ModuleCatalogImpl::_parse_xml_file(const char* file, 
					  ListOfParserComponent& modulelist, 
					  ListOfParserPathPrefix& pathList)
{
  BEGIN_OF("_parse_xml_file");
  SCRUTE(file);

  SALOME_ModuleCatalog_Handler* handler = new SALOME_ModuleCatalog_Handler();
  QFile xmlFile(file);

  QXmlInputSource source(xmlFile);

  QXmlSimpleReader reader;
  reader.setContentHandler( handler );
  reader.setErrorHandler( handler );
  reader.parse( source );
  xmlFile.close();

  unsigned int ind, j;
  for ( ind = 0; ind < _moduleList.size(); ind++) {
    for (j=0; j<modulelist.size(); j++)
      if (modulelist[j].parserComponentName ==
	  _moduleList[ind].parserComponentName) {
	modulelist[ind] = _moduleList[ind];
      }
    if (j == modulelist.size())
      modulelist.push_back(_moduleList[ind]) ;
  }

  for ( ind = 0; ind < _pathList.size(); ind++)
    pathList.push_back(_pathList[ind]) ;
}

void 
SALOME_ModuleCatalogImpl::ImportXmlCatalogFile(const char* file)
{
  _parse_xml_file(file, _personal_module_list, _personal_path_list);
}

//----------------------------------------------------------------------
// Function : duplicate_interfaces
// Purpose  : create a list of interfaces from the parsing of the catalog
//----------------------------------------------------------------------
SALOME_ModuleCatalog::ListOfDefInterface
SALOME_ModuleCatalogImpl::duplicate_interfaces(ListOfDefinitionInterface list_interface)
{

  // Choices for DataStream port types (en dur ... pas terrible) (MT)
  std::map < string, SALOME_ModuleCatalog::DataStreamType > DataStreamTypeConvert;
  DataStreamTypeConvert["UNKNOWN"] = SALOME_ModuleCatalog::DATASTREAM_UNKNOWN;
  DataStreamTypeConvert["INTEGER"] = SALOME_ModuleCatalog::DATASTREAM_INTEGER;
  DataStreamTypeConvert["FLOAT"]   = SALOME_ModuleCatalog::DATASTREAM_FLOAT;
  DataStreamTypeConvert["DOUBLE"]  = SALOME_ModuleCatalog::DATASTREAM_DOUBLE;
  DataStreamTypeConvert["STRING"]  = SALOME_ModuleCatalog::DATASTREAM_STRING;
  DataStreamTypeConvert["BOOLEAN"] = SALOME_ModuleCatalog::DATASTREAM_BOOLEAN;
  std::map < string, SALOME_ModuleCatalog::DataStreamType >::iterator it_type;

  // Choices for DataStream ports dependencies (en dur ... pas terrible) (MT)
  std::map<string,SALOME_ModuleCatalog::DataStreamDependency> DataStreamDepConvert;
  DataStreamDepConvert["UNDEFINED"] = SALOME_ModuleCatalog::DATASTREAM_UNDEFINED;
  DataStreamDepConvert["T"]         = SALOME_ModuleCatalog::DATASTREAM_TEMPORAL;
  DataStreamDepConvert["I"]         = SALOME_ModuleCatalog::DATASTREAM_ITERATIVE;
  std::map < string, SALOME_ModuleCatalog::DataStreamDependency >::iterator it_dep;


  SALOME_ModuleCatalog::ListOfDefInterface _list_interfaces;
  unsigned int _length_interfaces = list_interface.size();
  _list_interfaces.length(_length_interfaces);
  
  for (unsigned int ind = 0; ind < _length_interfaces; ind++)
    {
      SALOME_ModuleCatalog::DefinitionInterface & I_corba =  _list_interfaces[ind];
      ParserDefInterface & I_parser = list_interface[ind];

      //duplicate interface name
      I_corba.interfacename = CORBA::string_dup(I_parser.parserInterfaceName.c_str());

      // duplicate service list
      unsigned int _length_services = I_parser.parserInterfaceServiceList.size();
      SCRUTE(_length_services);
      I_corba.interfaceservicelist.length(_length_services);

      for (unsigned int ind1 = 0; ind1 < _length_services ; ind1 ++)
	{
	  // input structure form XML parser
	  const ParserService & S_parser          = I_parser.parserInterfaceServiceList[ind1];
	  // output structure to CORBA
	  SALOME_ModuleCatalog::Service & S_corba = I_corba.interfaceservicelist[ind1];

	  // duplicate service name
	  S_corba.ServiceName 
	    = CORBA::string_dup(S_parser.parserServiceName.c_str());

	  // duplicate service by default
	  S_corba.Servicebydefault 
	    = S_parser.parserServiceByDefault;

	  // duplicate in Parameters
	  unsigned int _length_in_param = S_parser.parserServiceInParameter.size();
	  S_corba.ServiceinParameter.length(_length_in_param);
	  for (unsigned int ind2 = 0; ind2 < _length_in_param ; ind2 ++)
	    {
	     SALOME_ModuleCatalog::ServicesParameter & P_corba = S_corba.ServiceinParameter[ind2];
	     const ParserServParam & P_parser = S_parser.parserServiceInParameter[ind2];
	      // duplicate parameter type
	     P_corba.Parametertype = CORBA::string_dup(P_parser.parserParamType.c_str());
	      
	      // duplicate parameter name
	     P_corba.Parametername = CORBA::string_dup(P_parser.parserParamName.c_str());
	    }

	  // duplicate out Parameters
	  unsigned int _length_out_param = S_parser.parserServiceOutParameter.size();
	  S_corba.ServiceoutParameter.length(_length_out_param);
	  for (unsigned int ind2 = 0; ind2 < _length_out_param ; ind2 ++)
	    {
	     SALOME_ModuleCatalog::ServicesParameter & P_corba
	       = S_corba.ServiceoutParameter[ind2];
	     const ParserServParam & P_parser
	       = S_parser.parserServiceOutParameter[ind2];

	      // duplicate parameter type
	     P_corba.Parametertype = CORBA::string_dup(P_parser.parserParamType.c_str());
	      
	      // duplicate parameter name
	     P_corba.Parametername = CORBA::string_dup(P_parser.parserParamName.c_str());
	    }
      
	  // duplicate in DataStreamParameters
	  unsigned int _length_in_datastream_param 
	    = S_parser.parserServiceInDataStreamParameter.size();
	  S_corba.ServiceinDataStreamParameter.length(_length_in_datastream_param);
	  for (unsigned int ind2 = 0; ind2 < _length_in_datastream_param ; ind2 ++)
	    {
	     SALOME_ModuleCatalog::ServicesDataStreamParameter & P_corba 
	       = S_corba.ServiceinDataStreamParameter[ind2];
	     const ParserServDataStreamParam & P_parser
	       = S_parser.parserServiceInDataStreamParameter[ind2];

	      // duplicate parameter type
	     it_type = DataStreamTypeConvert.find(P_parser.parserParamType);
	     S_corba.ServiceinDataStreamParameter[ind2].Parametertype
	       = (it_type == DataStreamTypeConvert.end()) 
	       ? it_type->second : SALOME_ModuleCatalog::DATASTREAM_UNKNOWN;
	      
	      // duplicate parameter name
	     S_corba.ServiceinDataStreamParameter[ind2].Parametername 
	       = CORBA::string_dup(P_parser.parserParamName.c_str());
	    }

	  // duplicate out DataStreamParameters
	  unsigned int _length_out_datastream_param 
	    = S_parser.parserServiceOutDataStreamParameter.size();
	 S_corba.ServiceoutDataStreamParameter.length(_length_out_datastream_param);

	  for (unsigned int ind2 = 0; ind2 < _length_out_datastream_param ; ind2 ++)
	    {
	     SALOME_ModuleCatalog::ServicesDataStreamParameter & P_corba 
	       = S_corba.ServiceoutDataStreamParameter[ind2];
	     const ParserServDataStreamParam & P_parser
	       = S_parser.parserServiceOutDataStreamParameter[ind2];

	      // duplicate parameter type
	     it_type = DataStreamTypeConvert.find(P_parser.parserParamType);
	     S_corba.ServiceoutDataStreamParameter[ind2].Parametertype
	       = (it_type == DataStreamTypeConvert.end()) 
	       ? it_type->second : SALOME_ModuleCatalog::DATASTREAM_UNKNOWN;
	      
	      // duplicate parameter name
	     P_corba.Parametername = CORBA::string_dup(P_parser.parserParamName.c_str());
	    }
      
	}
    }
  return _list_interfaces;
}


//----------------------------------------------------------------------
// Function : duplicate_pathes
// Purpose  : create the path prefix structures from the catalog parsing
//----------------------------------------------------------------------
ListOfPathPrefix 
SALOME_ModuleCatalogImpl::duplicate_pathes(ListOfParserPathPrefix pathes)
{
  ListOfPathPrefix _pathes ;

  unsigned int _length = pathes.size() ;
  _pathes.resize(_length);
  unsigned int _length_comput = 0;
  
  for (unsigned int ind = 0; ind < _length ; ind++)
    {
      // duplicate path
      _pathes[ind].path = CORBA::string_dup(pathes[ind].path.c_str()) ;
      //MESSAGE("Prefix : " << _pathes[ind].path);

      _length_comput = pathes[ind].listOfComputer.size() ;
      _pathes[ind].listOfComputer.resize(_length_comput);
      for (unsigned int ind1 = 0; ind1 <_length_comput  ; ind1++)
	{
	  // duplicate computer name
	  _pathes[ind].listOfComputer[ind1] = CORBA::string_dup(pathes[ind].listOfComputer[ind1].c_str());
	  //MESSAGE("Computer associated to the prefix : " <<_pathes[ind].listOfComputer[ind1]);  
	}
    } 
  return _pathes ;
}


//----------------------------------------------------------------------
// Function : _verify_path_prefix
// Purpose  : verify the path prefix structures from the catalog parsing
//            Verify that there only one path prefix associated to a 
//            particular computer
//----------------------------------------------------------------------
bool
SALOME_ModuleCatalogImpl::_verify_path_prefix(ListOfParserPathPrefix pathList)
{
  bool _return_value = true;
  vector<string> _machine_list;
  _machine_list.resize(0);

  // Fill a list of all computers indicated in the path list
  for (unsigned int ind = 0; ind < pathList.size(); ind++)
    { 
      for (unsigned int ind1 = 0 ; ind1 < pathList[ind].listOfComputer.size(); ind1++)
	{
	  _machine_list.push_back(pathList[ind].listOfComputer[ind1]);
	}
    }

  // Parse if a computer name is twice in the list of computers
  for (unsigned int ind = 0; ind < _machine_list.size(); ind++)
    {
     for (unsigned int ind1 = ind+1 ; ind1 < _machine_list.size(); ind1++)
       {
	 if(_machine_list[ind].compare(_machine_list[ind1]) == 0)
	   {
	     MESSAGE( "The computer " << _machine_list[ind] << " is indicated more than once in the path list")
	     _return_value = false; 
	   }
       }
    }
  return _return_value;
}


//----------------------------------------------------------------------
// Function : _parseArguments
// Purpose  : parse arguments to get general and personal catalog files
//----------------------------------------------------------------------
bool
SALOME_ModuleCatalogImpl::_parseArguments(int argc, char **argv, 
					  char **_general, 
					  char** _personal)
{
  bool _return_value = true;
  *_general = NULL;
  *_personal = NULL ;
  for (int ind = 0; ind < argc ; ind++)
    {

      if (strcmp(argv[ind],"-help") == 0)
	{
	  INFOS( "Usage: " << argv[0] << " -common 'path to general catalog' -personal 'path to personal catalog' -ORBInitRef NameService=corbaname::localhost");
	    _return_value = false ;
	}
      if (strcmp(argv[ind],"-common") == 0)
	{
	  if (ind + 1 < argc)
	    {
	      // General catalog file
	      *_general = argv[ind + 1] ;
/*	      ifstream _general_file(*_general);
	      if (!_general_file)
		{
		  MESSAGE( "Sorry the file " << *_general << " can't be open" )
		  *_general = NULL;
		  _return_value = false;
		}
*/
	    }
	}
      else if (strcmp(argv[ind],"-personal") == 0)
	{
	  if (ind + 1 < argc)
	    {
	      // Personal catalog file
	      *_personal = argv[ind + 1] ;
/*	      ifstream _personal_file(*_personal);
	      if (!_personal_file)
		{
		  MESSAGE("Sorry the file " << *_personal << " can't be open" )
		  *_personal = NULL;
		  _return_value = false;
		}
*/
	    }
	}
    }
  return _return_value;
}


