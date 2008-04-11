// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "SALOME_ResourcesManager.hxx" 
#include "Utils_ExceptHandlers.hxx"
#include "Utils_CorbaException.hxx"
#include "OpUtil.hxx"

#include <stdlib.h>
#ifndef WNT
#include <unistd.h>
#else
#include <io.h>
#include <process.h>
#endif
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <map>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include "utilities.h"

#define MAX_SIZE_FOR_HOSTNAME 256;

using namespace std;

const char *SALOME_ResourcesManager::_ResourcesManagerNameInNS = "/ResourcesManager";

//=============================================================================
/*!
 * just for test
 */ 
//=============================================================================

SALOME_ResourcesManager::
SALOME_ResourcesManager(CORBA::ORB_ptr orb, 
			PortableServer::POA_var poa, 
			SALOME_NamingService *ns,
                        const char *xmlFilePath) :
    _path_resources(xmlFilePath)
{
  MESSAGE("constructor");
  _NS = ns;
  _orb = CORBA::ORB::_duplicate(orb) ;
  _poa = PortableServer::POA::_duplicate(poa) ;
  PortableServer::ObjectId_var id = _poa->activate_object(this);
  CORBA::Object_var obj = _poa->id_to_reference(id);
  Engines::SalomeLauncher_var refContMan =
    Engines::SalomeLauncher::_narrow(obj);

  _NS->Register(refContMan,_ResourcesManagerNameInNS);
//   _MpiStarted = false;
  MESSAGE("constructor end");
}

//=============================================================================
/*!
 *  Standard constructor, parse resource file.
 *  - if ${APPLI} exists in environment,
 *    look for ${HOME}/${APPLI}/CatalogResources.xml
 *  - else look for default:
 *    ${KERNEL_ROOT_DIR}/share/salome/resources/kernel/CatalogResources.xml
 *  - parse XML resource file.
 */ 
//=============================================================================

SALOME_ResourcesManager::SALOME_ResourcesManager(CORBA::ORB_ptr orb, 
						 PortableServer::POA_var poa, 
						 SALOME_NamingService *ns)
{
  MESSAGE("constructor");
  _NS = ns;
  _orb = CORBA::ORB::_duplicate(orb) ;
  _poa = PortableServer::POA::_duplicate(poa) ;
  PortableServer::ObjectId_var id = _poa->activate_object(this);
  CORBA::Object_var obj = _poa->id_to_reference(id);
  Engines::ResourcesManager_var refContMan = Engines::ResourcesManager::_narrow(obj);
  _NS->Register(refContMan,_ResourcesManagerNameInNS);

  _isAppliSalomeDefined = (getenv("APPLI") != 0);
//   _MpiStarted = false;

  if (_isAppliSalomeDefined)
    {
      _path_resources = getenv("HOME");
      _path_resources += "/";
      _path_resources += getenv("APPLI");
      _path_resources += "/CatalogResources.xml";
    }

  else
    {
      _path_resources = getenv("KERNEL_ROOT_DIR");
      _path_resources += "/share/salome/resources/kernel/CatalogResources.xml";
    }

  ParseXmlFile();
  MESSAGE("constructor end");
}

//=============================================================================
/*!
 *  Standard Destructor
 */ 
//=============================================================================

SALOME_ResourcesManager::~SALOME_ResourcesManager()
{
  MESSAGE("destructor");
}


//=============================================================================
/*! CORBA method:
 *  shutdown all the containers, then the ContainerManager servant
 */
//=============================================================================

void SALOME_ResourcesManager::Shutdown()
{
  MESSAGE("Shutdown");
  _NS->Destroy_Name(_ResourcesManagerNameInNS);
  PortableServer::ObjectId_var oid = _poa->servant_to_id(this);
  _poa->deactivate_object(oid);
  //_remove_ref();
}

//=============================================================================
/*!
 *  get the list of name of ressources fitting for the specified module.
 *  If hostname specified, check it is local or known in resources catalog.
 *
 *  Else
 *  - select first machines with corresponding OS (all machines if
 *    parameter OS empty),
 *  - then select the sublist of machines on witch the module is known
 *    (if the result is empty, that probably means that the inventory of
 *    modules is probably not done, so give complete list from previous step)
 */ 
//=============================================================================

Engines::MachineList *
SALOME_ResourcesManager::GetFittingResources(const Engines::MachineParameters& params,
					     const Engines::CompoList& componentList)
//throw(SALOME_Exception)
{
//   MESSAGE("ResourcesManager::GetFittingResources");
  vector <std::string> vec;
  Engines::MachineList *ret=new Engines::MachineList;

  try{
    // --- To be sure that we search in a correct list.
    ParseXmlFile();

    const char *hostname = (const char *)params.hostname;
    MESSAGE("GetFittingResources " << hostname << " " << GetHostname().c_str());

    if (hostname[0] != '\0')
      {
//       MESSAGE("ResourcesManager::GetFittingResources : hostname specified" );

	if ( strcmp(hostname, "localhost") == 0 ||
	     strcmp(hostname, GetHostname().c_str()) == 0 )
	  {
	    //           MESSAGE("ResourcesManager::GetFittingResources : localhost" );
	    vec.push_back(GetHostname().c_str());
	    // 	  MESSAGE("ResourcesManager::GetFittingResources : " << vec.size());
	  }
	
	else if (_resourcesList.find(hostname) != _resourcesList.end())
	  {
	    // --- params.hostname is in the list of resources so return it.
	    vec.push_back(hostname);
	  }
	
	else
	  {
	    // Cas d'un cluster: nombre de noeuds > 1
	    int cpt=0;
	    for (map<string, ParserResourcesType>::const_iterator iter = _resourcesList.begin(); iter != _resourcesList.end(); iter++){
	      if( (*iter).second.DataForSort._nbOfNodes > 1 ){
		if( strncmp(hostname,(*iter).first.c_str(),strlen(hostname)) == 0 ){
		  vec.push_back((*iter).first.c_str());
		  //cout << "SALOME_ResourcesManager::GetFittingResources vector["
		  //     << cpt << "] = " << (*iter).first.c_str() << endl ;
		  cpt++;
		}
	      }
	    }
	    if(cpt==0){
	      // --- user specified an unknown hostame so notify him.
	      MESSAGE("ResourcesManager::GetFittingResources : SALOME_Exception");
	      throw SALOME_Exception("unknown host");
	    }
	  }
      }
    
    else
      // --- Search for available resources sorted by priority
      {
	SelectOnlyResourcesWithOS(vec, params.OS);
	
	KeepOnlyResourcesWithModule(vec, componentList);
	
	if (vec.size() == 0)
	  SelectOnlyResourcesWithOS(vec, params.OS);
	
	// --- set wanted parameters
	ResourceDataToSort::_nbOfNodesWanted = params.nb_node;
	
	ResourceDataToSort::_nbOfProcPerNodeWanted = params.nb_proc_per_node;
	
	ResourceDataToSort::_CPUFreqMHzWanted = params.cpu_clock;
	
	ResourceDataToSort::_memInMBWanted = params.mem_mb;
	
	// --- end of set
	
	list<ResourceDataToSort> li;
	
	for (vector<string>::iterator iter = vec.begin();
           iter != vec.end();
	     iter++)
	  li.push_back(_resourcesList[(*iter)].DataForSort);
	
	li.sort();
	
	unsigned int i = 0;
	
	for (list<ResourceDataToSort>::iterator iter2 = li.begin();
	     iter2 != li.end();
	     iter2++)
	  vec[i++] = (*iter2)._hostName;
      }
    
    //  MESSAGE("ResourcesManager::GetFittingResources : return" << ret.size());
    ret->length(vec.size());
    for(unsigned int i=0;i<vec.size();i++)
      (*ret)[i]=(vec[i]).c_str();

  }
  catch(const SALOME_Exception &ex)
    {
      INFOS("Caught exception.");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::BAD_PARAM);
      //return ret;
    }  

  return ret;
}

//=============================================================================
/*!
 *  add an entry in the ressources catalog  xml file.
 *  Return 0 if OK (KERNEL found in new resources modules) else throw exception
 */ 
//=============================================================================

int
SALOME_ResourcesManager::
AddResourceInCatalog(const Engines::MachineParameters& paramsOfNewResources,
                     const vector<string>& modulesOnNewResources,
                     const char *alias,
                     const char *userName,
                     AccessModeType mode,
                     AccessProtocolType prot)
throw(SALOME_Exception)
{
  vector<string>::const_iterator iter = find(modulesOnNewResources.begin(),
					     modulesOnNewResources.end(),
					     "KERNEL");

  if (iter != modulesOnNewResources.end())
    {
      ParserResourcesType newElt;
      newElt.DataForSort._hostName = paramsOfNewResources.hostname;
      newElt.Alias = alias;
      newElt.Protocol = prot;
      newElt.Mode = mode;
      newElt.UserName = userName;
      newElt.ModulesList = modulesOnNewResources;
      newElt.OS = paramsOfNewResources.OS;
      newElt.DataForSort._memInMB = paramsOfNewResources.mem_mb;
      newElt.DataForSort._CPUFreqMHz = paramsOfNewResources.cpu_clock;
      newElt.DataForSort._nbOfNodes = paramsOfNewResources.nb_node;
      newElt.DataForSort._nbOfProcPerNode =
        paramsOfNewResources.nb_proc_per_node;
      _resourcesList[newElt.DataForSort._hostName] = newElt;
      return 0;
    }

  else
    throw SALOME_Exception("KERNEL is not present in this resource");
}

//=============================================================================
/*!
 *  Deletes a resource from the catalog
 */ 
//=============================================================================

void SALOME_ResourcesManager::DeleteResourceInCatalog(const char *hostname)
{
  _resourcesList.erase(hostname);
}

//=============================================================================
/*!
 *  write the current data in memory in file.
 */ 
//=============================================================================

void SALOME_ResourcesManager::WriteInXmlFile()
{
  const char* aFilePath = _path_resources.c_str();
  
  FILE* aFile = fopen(aFilePath, "w");

  if (aFile == NULL)
    {
      INFOS("Error opening file !");
      return;
    }
  
  xmlDocPtr aDoc = xmlNewDoc(BAD_CAST "1.0");
  xmlNewDocComment(aDoc, BAD_CAST "ResourcesCatalog");

  SALOME_ResourcesCatalog_Handler* handler =
    new SALOME_ResourcesCatalog_Handler(_resourcesList);
  handler->PrepareDocToXmlFile(aDoc);
  delete handler;

  int isOk = xmlSaveFile(aFilePath, aDoc);
  
  if (!isOk)
    INFOS("Error while XML file saving.");
  
  // Free the document
  xmlFreeDoc(aDoc);

  fclose(aFile);
  
  MESSAGE("WRITING DONE!");
}

//=============================================================================
/*!
 *  parse the data type catalog
 */ 
//=============================================================================

const MapOfParserResourcesType& SALOME_ResourcesManager::ParseXmlFile()
{
  SALOME_ResourcesCatalog_Handler* handler =
    new SALOME_ResourcesCatalog_Handler(_resourcesList);

  const char* aFilePath = _path_resources.c_str();
  FILE* aFile = fopen(aFilePath, "r");
  
  if (aFile != NULL)
    {
      xmlDocPtr aDoc = xmlReadFile(aFilePath, NULL, 0);
      
      if (aDoc != NULL)
	handler->ProcessXmlDocument(aDoc);
      else
	INFOS("ResourcesManager: could not parse file "<<aFilePath);
      
      // Free the document
      xmlFreeDoc(aDoc);

      fclose(aFile);
    }
  else
    INFOS("ResourcesManager: file "<<aFilePath<<" is not readable.");
  
  delete handler;

  return _resourcesList;
}

//=============================================================================
/*!
 *   consult the content of the list
 */ 
//=============================================================================

const MapOfParserResourcesType& SALOME_ResourcesManager::GetList() const
  {
    return _resourcesList;
  }


//=============================================================================
/*!
 *  dynamically obtains the first machines
 */ 
//=============================================================================

char *
SALOME_ResourcesManager::FindFirst(const Engines::MachineList& listOfMachines)
{
  return CORBA::string_dup(_dynamicResourcesSelecter.FindFirst(listOfMachines).c_str());
}

//=============================================================================
/*!
 *  dynamically obtains the best machines
 */ 
//=============================================================================

string
SALOME_ResourcesManager::FindNext(const Engines::MachineList& listOfMachines)
{
  return _dynamicResourcesSelecter.FindNext(listOfMachines,_resourcesList,_NS);
}
//=============================================================================
/*!
 *  dynamically obtains the best machines
 */ 
//=============================================================================

string
SALOME_ResourcesManager::FindBest(const Engines::MachineList& listOfMachines)
{
  return _dynamicResourcesSelecter.FindBest(listOfMachines);
}

//=============================================================================
/*!
 *  Gives a sublist of machines with matching OS.
 *  If parameter OS is empty, gives the complete list of machines
 */ 
//=============================================================================

// Warning need an updated parsed list : _resourcesList
void
SALOME_ResourcesManager::SelectOnlyResourcesWithOS
( vector<string>& hosts,
  const char *OS) const
throw(SALOME_Exception)
{
  string base(OS);

  for (map<string, ParserResourcesType>::const_iterator iter =
         _resourcesList.begin();
       iter != _resourcesList.end();
       iter++)
    {
      if ( (*iter).second.OS == base || base.size() == 0)
        hosts.push_back((*iter).first);
    }
}


//=============================================================================
/*!
 *  Gives a sublist of machines on which the module is known.
 */ 
//=============================================================================

//Warning need an updated parsed list : _resourcesList
void
SALOME_ResourcesManager::KeepOnlyResourcesWithModule
( vector<string>& hosts,
  const Engines::CompoList& componentList) const
throw(SALOME_Exception)
{
  for (vector<string>::iterator iter = hosts.begin(); iter != hosts.end();)
    {
      MapOfParserResourcesType::const_iterator it = _resourcesList.find(*iter);
      const vector<string>& mapOfModulesOfCurrentHost = (((*it).second).ModulesList);

      bool erasedHost = false;
      if( mapOfModulesOfCurrentHost.size() > 0 ){
	for(int i=0;i<componentList.length();i++){
          const char* compoi = componentList[i];
	  vector<string>::const_iterator itt = find(mapOfModulesOfCurrentHost.begin(),
					      mapOfModulesOfCurrentHost.end(),
					      compoi);
// 					      componentList[i]);
	  if (itt == mapOfModulesOfCurrentHost.end()){
	    erasedHost = true;
	    break;
	  }
	}
      }
      if(erasedHost)
        hosts.erase(iter);
      else
        iter++;
    }
}


Engines::MachineParameters* SALOME_ResourcesManager::GetMachineParameters(const char *hostname)
{
  ParserResourcesType resource = _resourcesList[string(hostname)];
  Engines::MachineParameters *p_ptr = new Engines::MachineParameters;
  p_ptr->container_name = CORBA::string_dup("");
  p_ptr->hostname = CORBA::string_dup("hostname");
  p_ptr->alias = CORBA::string_dup(resource.Alias.c_str());
  if( resource.Protocol == rsh )
    p_ptr->protocol = "rsh";
  else if( resource.Protocol == ssh )
    p_ptr->protocol = "ssh";
  p_ptr->username = CORBA::string_dup(resource.UserName.c_str());
  p_ptr->applipath = CORBA::string_dup(resource.AppliPath.c_str());
  p_ptr->modList.length(resource.ModulesList.size());
  for(int i=0;i<resource.ModulesList.size();i++)
    p_ptr->modList[i] = CORBA::string_dup(resource.ModulesList[i].c_str());
  p_ptr->OS = CORBA::string_dup(resource.OS.c_str());
  p_ptr->mem_mb = resource.DataForSort._memInMB;
  p_ptr->cpu_clock = resource.DataForSort._CPUFreqMHz;
  p_ptr->nb_proc_per_node = resource.DataForSort._nbOfProcPerNode;
  p_ptr->nb_node = resource.DataForSort._nbOfNodes;
  if( resource.mpi == indif )
    p_ptr->mpiImpl = "indif";
  else if( resource.mpi == lam )
    p_ptr->mpiImpl = "lam";
  else if( resource.mpi == mpich1 )
    p_ptr->mpiImpl = "mpich1";
  else if( resource.mpi == mpich2 )
    p_ptr->mpiImpl = "mpich2";
  else if( resource.mpi == openmpi )
    p_ptr->mpiImpl = "openmpi";
  if( resource.Batch == pbs )
    p_ptr->batch = "pbs";
  else if( resource.Batch == lsf )
    p_ptr->batch = "lsf";
  else if( resource.Batch == slurm )
    p_ptr->batch = "slurm";
  return p_ptr;
}

ParserResourcesType SALOME_ResourcesManager::GetResourcesList(const std::string& machine)
{
  return _resourcesList[machine];
}
