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
//  File   : SALOME_ModuleCatalog_Acomponent_impl.cxx
//  Author : Estelle Deville
//  Module : SALOME
//  $Header$

#include "SALOME_ModuleCatalog_Acomponent_impl.hxx"
using namespace std;

//----------------------------------------------------------------------
// Function : SALOME_ModuleCatalog_AcomponentImpl
// Purpose  : Constructor
//            Affect the component name, type,icone,  a bool to define 
//            if it's multistudy or not.
//            Affect too the constraint and the interfaces of the component
//            and the pathes prefixes for all computers
//----------------------------------------------------------------------  
SALOME_ModuleCatalog_AcomponentImpl::SALOME_ModuleCatalog_AcomponentImpl(
                   const char* name,
                   const char* username,
		   const char* constraint,  
		   SALOME_ModuleCatalog::ComponentType componenttype,
		   CORBA::Boolean componentmultistudy,
		   const char* icone,
		   SALOME_ModuleCatalog::ListOfDefInterface list_interfaces,
		   ListOfPathPrefix pathes)
{
  BEGIN_OF("SALOME_ModuleCatalog_AcomponentImpl");
  SCRUTE(name);
  SCRUTE(username);
  SCRUTE(constraint);
  SCRUTE(componenttype);
  SCRUTE(componentmultistudy);
  SCRUTE(icone);

  // Affect component name
  _component_name = new char[strlen(name)+1];
 strcpy(_component_name, name);

  // Affect component user name
  _component_user_name = new char[strlen(username)+1];
 strcpy(_component_user_name, username);

 // Affect constraint
 _constraint =new char[strlen(constraint)+1];
 strcpy(_constraint, constraint);

 // Affect component type
 _componenttype = componenttype;
 
 // Affect component multistudy
 _componentmultistudy = componentmultistudy;

 // Affect icone
 _icone =new char[strlen(icone)+1];
 strcpy(_icone, icone);

 //Affect interfaces
 _list_interfaces = list_interfaces;

 // affect path prefixes
 // _pathes.resize(pathes.size());
 _pathes = pathes ;

  END_OF("SALOME_ModuleCatalog_AcomponentImpl");
}

//----------------------------------------------------------------------
// Function : ~SALOME_ModuleCatalog_AcomponentImpl
// Purpose  : Destructor 
//----------------------------------------------------------------------
SALOME_ModuleCatalog_AcomponentImpl::~SALOME_ModuleCatalog_AcomponentImpl()
{
  BEGIN_OF("~SALOME_ModuleCatalog_AcomponentImpl");

  // empty memory
  delete [] _component_name;
  delete [] _component_user_name;
  delete [] _constraint;

  END_OF("~SALOME_ModuleCatalog_AcomponentImpl");
}

//----------------------------------------------------------------------
// Function : GetInterfaceList
// Purpose  : get a list of the interfaces name of a component
//----------------------------------------------------------------------
SALOME_ModuleCatalog::ListOfInterfaces* 
SALOME_ModuleCatalog_AcomponentImpl::GetInterfaceList() 
{
  BEGIN_OF("GetInterfaceList");

  SALOME_ModuleCatalog::ListOfInterfaces_var _list = new SALOME_ModuleCatalog::ListOfInterfaces;

  // All the interfaces are defined in _list_interfaces affected at the
  // component creation
  unsigned int _length_interfaces =_list_interfaces.length();

  _list->length(_length_interfaces);

  // Parse all the interfaces to get their name
  for (unsigned int ind = 0; ind < _length_interfaces; ind++)
    {
      _list[ind] = CORBA::string_dup(_list_interfaces[ind].interfacename);
      MESSAGE("The component " << _component_name << " contains " << _list[ind] << " as interface") 
    }
  
  END_OF("GetInterfaceList");
  return _list._retn();
}

//----------------------------------------------------------------------
// Function : GetInterface
// Purpose  : get one interface of a component
//----------------------------------------------------------------------
SALOME_ModuleCatalog::DefinitionInterface*
SALOME_ModuleCatalog_AcomponentImpl::GetInterface(const char* interfacename)
                                     throw(SALOME_ModuleCatalog::NotFound)
{
  BEGIN_OF("GetInterface");
  SCRUTE(interfacename);

  SALOME_ModuleCatalog::DefinitionInterface_var _interface = new SALOME_ModuleCatalog::DefinitionInterface;
  SALOME_ModuleCatalog::Service_var _service = new SALOME_ModuleCatalog::Service;
  // Variables initialisation
  _interface->interfaceservicelist.length(0);
  int _length = 0;
  bool _find = false ;
  
  // looking for the specified interface
  for (unsigned int ind = 0; ind < _list_interfaces.length(); ind++)
    {
      if (strcmp(interfacename, _list_interfaces[ind].interfacename) == 0)
	{
	  // wanted interface
	  _find = true ;

	  // Affect the name of the interface
	  _interface->interfacename = interfacename;

	  // Affect each service of the wanted interface
	  for (unsigned int ind1 = 0; ind1 <  _list_interfaces[ind].interfaceservicelist.length() ; ind1++)
	    {
	      _interface->interfaceservicelist.length(_length+1);
	      _service = _duplicate_service(_list_interfaces[ind].interfaceservicelist[ind1]);
	      _interface->interfaceservicelist[_length] = _service;	
	     
	      _length++;
	    }

	}
    }
  
  if (!_find)
    {
      // The interface was not found, the exception should be thrown
      string message = "The interface";
      message += interfacename;
      message += " of the component ";
      message += _component_name;
      message += " was not found"; 
      MESSAGE(message)
	throw SALOME_ModuleCatalog::NotFound(message.c_str());
    }

  END_OF("GetInterfaceList");
  return _interface._retn();
}



//----------------------------------------------------------------------
// Function : GetServiceList
// Purpose  : get a list of the services name of an interface 
//            of a component
//----------------------------------------------------------------------
SALOME_ModuleCatalog::ListOfServices* 
SALOME_ModuleCatalog_AcomponentImpl::GetServiceList(const char* interfacename)
                                     throw(SALOME_ModuleCatalog::NotFound)
{
  BEGIN_OF("GetServiceList");
  SCRUTE(interfacename);

  SALOME_ModuleCatalog::ListOfServices_var _list = new SALOME_ModuleCatalog::ListOfServices;

  // Variables initialisation
  _list->length(0); 
  bool _find = false ;

  // looking for the specified interface
  for (unsigned int ind = 0; ind < _list_interfaces.length(); ind++)
    {
      if (strcmp(interfacename, _list_interfaces[ind].interfacename) == 0)
	{
	  _find = true ;
	  // wanted interface
	  // Get the list of services name for this interface
	  unsigned int _length_services = _list_interfaces[ind].interfaceservicelist.length();
	  _list->length(_length_services);
	  for (unsigned int ind1 = 0; ind1 < _length_services ; ind1++)
	    {
	      _list[ind1] = CORBA::string_dup(_list_interfaces[ind].interfaceservicelist[ind1].ServiceName);
	      MESSAGE("The interface " << interfacename << " of the component " << _component_name << " contains " << _list[ind1] << " as a service") 
	    }
	}
    }

 if (!_find)
    {
      // The interface was not found, the exception should be thrown
      string message = "The interface";
      message += interfacename;
      message += " of the component ";
      message += _component_name;
      message += " was not found"; 
      MESSAGE(message)
	throw SALOME_ModuleCatalog::NotFound(message.c_str());
    }

  END_OF("GetServiceList");
  return _list._retn();
}

    
//----------------------------------------------------------------------
// Function : GetService
// Purpose  : get one service of an interface of a component
//----------------------------------------------------------------------
SALOME_ModuleCatalog::Service* 
SALOME_ModuleCatalog_AcomponentImpl::GetService(const char* interfacename, 
						const char* servicename) 
                                     throw(SALOME_ModuleCatalog::NotFound)
{
  BEGIN_OF("GetService");
  SCRUTE(interfacename);
  SCRUTE(servicename);

  SALOME_ModuleCatalog::Service_var _service = new SALOME_ModuleCatalog::Service;
  // Varaibles initialisation
  bool _find = false ;
  
  MESSAGE("");
  // looking for the specified interface
  for (unsigned int ind = 0; ind < _list_interfaces.length(); ind++)
    {
      SCRUTE(ind);
      SCRUTE(_list_interfaces[ind].interfacename);

      if (strcmp(interfacename, _list_interfaces[ind].interfacename) == 0)
	{
	  // wanted interface
	  // looking for the specified service
	  for (unsigned int ind1 = 0; ind1 <  _list_interfaces[ind].interfaceservicelist.length() ; ind1++)
	    {
	      SCRUTE(ind1);
	      SCRUTE(_list_interfaces[ind].interfaceservicelist[ind1].ServiceName);

	      if (strcmp(servicename, _list_interfaces[ind].interfaceservicelist[ind1].ServiceName) == 0)
	      {
		// Wanted Service
		// Affect the service to be returned
		_find = true ;
		_service = _duplicate_service(_list_interfaces[ind].interfaceservicelist[ind1]);
	      }
	    }

	}
    }
  
  SCRUTE(_find);
  if (!_find)
    {
      // The interface was not found, the exception should be thrown
      string message = "The service";
      message += servicename;
      message += " of the interface ";
      message += interfacename;
      message += " of the component ";
      message += _component_name;
      message += " was not found"; 
      MESSAGE(message)
	throw SALOME_ModuleCatalog::NotFound(message.c_str());
    }

  END_OF("GetService");
  return _service._retn();
}

//----------------------------------------------------------------------
// Function : GetDefaultService
// Purpose  : get the default service of the interface
//----------------------------------------------------------------------
SALOME_ModuleCatalog::Service* 
SALOME_ModuleCatalog_AcomponentImpl::GetDefaultService(const char* interfacename) 
                                     throw(SALOME_ModuleCatalog::NotFound)
{
  BEGIN_OF("GetDefaultService");
  SCRUTE(interfacename);

  SALOME_ModuleCatalog::Service_var _service = new SALOME_ModuleCatalog::Service;

  // Variables initialisation
  bool _find = false ;

  // looking for the specified interface
  for (unsigned int ind = 0; ind < _list_interfaces.length(); ind++)
    {
      if (strcmp(interfacename, _list_interfaces[ind].interfacename) == 0)
	{
	  // wanted interface
	  // looking for the defautl service of the wanted interface
	  for (unsigned int ind1 = 0; ind1 <  _list_interfaces[ind].interfaceservicelist.length() ; ind1++)
	    {
	      if (_list_interfaces[ind].interfaceservicelist[ind1].Servicebydefault)
	      {
		// Default Service
		// affect the service to be returned
		_find = true ;
		_service = _duplicate_service(_list_interfaces[ind].interfaceservicelist[ind1]);
	      }
	    }

	}
    }

  if (!_find)
    {
      // The service was not found, the exception should be thrown
      string message = "The default service of the interface ";
      message += interfacename;
      message += " of the component ";
      message += _component_name;
      message += " was not found";
      MESSAGE(message)
	throw SALOME_ModuleCatalog::NotFound(message.c_str());
    }

  END_OF("GetDefaultService");
  return _service._retn();
}

//----------------------------------------------------------------------
// Function : GetPathPrefix
// Purpose  : get the PathPrefix of a computer
//----------------------------------------------------------------------
char* 
SALOME_ModuleCatalog_AcomponentImpl::GetPathPrefix(const char* machinename) 
                                     throw(SALOME_ModuleCatalog::NotFound)
{
  BEGIN_OF("GetPathPrefix");
  SCRUTE(machinename);

  // Variables initialisation
  char* _path = NULL;
  bool _find = false ;

  // Parse all the path prefixes
  // looking for the wanted computer
  for (unsigned int ind = 0 ; ind < _pathes.size() ; ind++)
    {
      for (unsigned int ind1 = 0 ; ind1 < _pathes[ind].listOfComputer.size() ; ind1++)    
	{
	  if (strcmp(machinename, _pathes[ind].listOfComputer[ind1].c_str()) == 0)
	    {
	      _find = true ;
	      // Wanted computer
	      // affect the path to be returned
	        const char* _temp = _pathes[ind].path.c_str() ;
		  _path = new char[strlen(_temp)+1];
	      strcpy(_path,_temp);
	    }
	}
    }

  if (!_find)
    {
      // The computer was not found, the exception should be thrown
      string message = "The computer ";
      message += machinename;
      message += " was not found in the catalog associated to the component ";
      message += _component_name;
      MESSAGE(message)
	throw SALOME_ModuleCatalog::NotFound(message.c_str());
    }

  END_OF("GetPathPrefix");
  return _path;
}

//----------------------------------------------------------------------
// Function : constraint
// Purpose  : obtain the constraint affected to a component
//----------------------------------------------------------------------
char* SALOME_ModuleCatalog_AcomponentImpl::constraint() 
{
  return CORBA::string_dup(_constraint);
}

//----------------------------------------------------------------------
// Function : componentname
// Purpose  : obtain the componentname
//----------------------------------------------------------------------
char* SALOME_ModuleCatalog_AcomponentImpl::componentname()
{
  return CORBA::string_dup(_component_name);
}

//----------------------------------------------------------------------
// Function : componentusername
// Purpose  : obtain the componentusername
//----------------------------------------------------------------------
char* SALOME_ModuleCatalog_AcomponentImpl::componentusername()
{
  return CORBA::string_dup(_component_user_name);
}

//----------------------------------------------------------------------
// Function : multistudy
// Purpose  : define if a component can be multistudy or not
//----------------------------------------------------------------------
CORBA::Boolean SALOME_ModuleCatalog_AcomponentImpl::multistudy()
{
  return _componentmultistudy ;
}

//----------------------------------------------------------------------
// Function : component_type
// Purpose  : define the type of the component
//----------------------------------------------------------------------
SALOME_ModuleCatalog::ComponentType 
SALOME_ModuleCatalog_AcomponentImpl::component_type() 
{
  return _componenttype;
}

//----------------------------------------------------------------------
// Function : icone
// Purpose  : obtain the icone affected to a component (for IAPP)
//----------------------------------------------------------------------
char* SALOME_ModuleCatalog_AcomponentImpl::component_icone() 
{
  return CORBA::string_dup(_icone);
}

//----------------------------------------------------------------------
// Function :  _duplicate_service
// Purpose  : duplicate a service
//----------------------------------------------------------------------
SALOME_ModuleCatalog::Service_var 
SALOME_ModuleCatalog_AcomponentImpl::_duplicate_service(SALOME_ModuleCatalog::Service & service)
{
  BEGIN_OF("_duplicate_service");

  unsigned int _length;
  SALOME_ModuleCatalog::Service_var _service = new SALOME_ModuleCatalog::Service;

  // service name
  _service->ServiceName = CORBA::string_dup(service.ServiceName);
  // service by default
  _service->Servicebydefault = service.Servicebydefault;

  MESSAGE("");
  // in Parameters service
  _length = service.ServiceinParameter.length();
  SCRUTE(_length);
  for(unsigned int ind = 0; ind < _length; ind++)
    {
      SCRUTE(service.ServiceinParameter[ind].Parametername);
      _service->ServiceinParameter.length(_length);
      // in Parameter type
      _service->ServiceinParameter[ind].Parametertype 
	= CORBA::string_dup(service.ServiceinParameter[ind].Parametertype);
      // in Parameter name
       _service->ServiceinParameter[ind].Parametername 
	 = CORBA::string_dup(service.ServiceinParameter[ind].Parametername);
    }

   // out Parameters service
  _length = service.ServiceoutParameter.length();
  SCRUTE(_length);
  for(unsigned int ind = 0; ind < _length; ind++)
    {
      SCRUTE(service.ServiceoutParameter[ind].Parametername);
      _service->ServiceoutParameter.length(_length);
      // out Parameter type
      _service->ServiceoutParameter[ind].Parametertype 
	= CORBA::string_dup(service.ServiceoutParameter[ind].Parametertype);
      // out Parameter name
       _service->ServiceoutParameter[ind].Parametername 
	 = CORBA::string_dup(service.ServiceoutParameter[ind].Parametername);
    }    

  // in DataStreamParameters service
  _length = service.ServiceinDataStreamParameter.length();
  SCRUTE(_length);
  for(unsigned int ind = 0; ind < _length; ind++)
    {
      SCRUTE(service.ServiceinDataStreamParameter[ind].Parametername);
      _service->ServiceinDataStreamParameter.length(_length);
      // in DataStreamParameter type
      _service->ServiceinDataStreamParameter[ind].Parametertype 
	= service.ServiceinDataStreamParameter[ind].Parametertype;
      // in DataStreamParameter dependency
      _service->ServiceinDataStreamParameter[ind].Parameterdependency
	= service.ServiceinDataStreamParameter[ind].Parameterdependency;
      // in DataStreamParameter name
      _service->ServiceinDataStreamParameter[ind].Parametername 
	= CORBA::string_dup(service.ServiceinDataStreamParameter[ind].Parametername);
    }

   // out DataStreamParameters service
  _length = service.ServiceoutDataStreamParameter.length();
  SCRUTE(_length);
  for(unsigned int ind = 0; ind < _length; ind++)
    {
     SCRUTE(service.ServiceoutDataStreamParameter[ind].Parametername);
      _service->ServiceoutDataStreamParameter.length(_length);
      // out DataStreamParameter type
      _service->ServiceoutDataStreamParameter[ind].Parametertype 
	= service.ServiceoutDataStreamParameter[ind].Parametertype;
      // in DataStreamParameter dependency
      _service->ServiceoutDataStreamParameter[ind].Parameterdependency
	= service.ServiceoutDataStreamParameter[ind].Parameterdependency;
      // out DataStreamParameter name
      _service->ServiceoutDataStreamParameter[ind].Parametername 
	= CORBA::string_dup(service.ServiceoutDataStreamParameter[ind].Parametername);
    }    

  END_OF("_duplicate_service");
  return _service;
}
