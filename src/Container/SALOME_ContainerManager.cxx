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
#include "SALOME_ContainerManager.hxx"
#include "SALOME_NamingService.hxx"
#include "OpUtil.hxx"
#include <sys/types.h>
#ifndef WNT
#include <unistd.h>
#endif
#include <vector>
#include "Utils_CorbaException.hxx"
#include "Batch_Date.hxx"

#define TIME_OUT_TO_LAUNCH_CONT 21

using namespace std;

vector<Engines::Container_ptr> SALOME_ContainerManager::_batchLaunchedContainers;

vector<Engines::Container_ptr>::iterator SALOME_ContainerManager::_batchLaunchedContainersIter;

const char *SALOME_ContainerManager::_ContainerManagerNameInNS = 
  "/ContainerManager";

//=============================================================================
/*! 
 *  Constructor
 *  \param orb
 *  Define a CORBA single thread policy for the server, which avoid to deal
 *  with non thread-safe usage like Change_Directory in SALOME naming service
 */
//=============================================================================

SALOME_ContainerManager::SALOME_ContainerManager(CORBA::ORB_ptr orb)
{
  MESSAGE("constructor");
  _NS = new SALOME_NamingService(orb);
  _ResManager = new SALOME_ResourcesManager(orb);
  _id=0;
  PortableServer::POA_var root_poa = PortableServer::POA::_the_root_poa();
  PortableServer::POAManager_var pman = root_poa->the_POAManager();
  PortableServer::POA_var my_poa;

  _orb = CORBA::ORB::_duplicate(orb) ;
  CORBA::PolicyList policies;
  policies.length(1);
  PortableServer::ThreadPolicy_var threadPol = 
    root_poa->create_thread_policy(PortableServer::SINGLE_THREAD_MODEL);
  policies[0] = PortableServer::ThreadPolicy::_duplicate(threadPol);

  my_poa = 
    root_poa->create_POA("SThreadPOA",pman,policies);
  threadPol->destroy();
  PortableServer::ObjectId_var id = my_poa->activate_object(this);
  CORBA::Object_var obj = my_poa->id_to_reference(id);
  Engines::ContainerManager_var refContMan =
    Engines::ContainerManager::_narrow(obj);

  _NS->Register(refContMan,_ContainerManagerNameInNS);
  MESSAGE("constructor end");
}

//=============================================================================
/*! 
 * destructor
 */
//=============================================================================

SALOME_ContainerManager::~SALOME_ContainerManager()
{
  MESSAGE("destructor");
  delete _NS;
  delete _ResManager;
}

//=============================================================================
/*! CORBA method:
 *  shutdown all the containers, then the ContainerManager servant
 */
//=============================================================================

void SALOME_ContainerManager::Shutdown()
{
  MESSAGE("Shutdown");
  ShutdownContainers();
  _NS->Destroy_Name(_ContainerManagerNameInNS);
  PortableServer::ObjectId_var oid = _default_POA()->servant_to_id(this);
  _default_POA()->deactivate_object(oid);
  _remove_ref();
  if(!CORBA::is_nil(_orb))
    _orb->shutdown(0);
}

//=============================================================================
/*! CORBA Method:
 *  Loop on all the containers listed in naming service, ask shutdown on each
 */
//=============================================================================

void SALOME_ContainerManager::ShutdownContainers()
{
  MESSAGE("ShutdownContainers");
  bool isOK;
  isOK = _NS->Change_Directory("/Containers");
  if( isOK ){
    vector<string> vec = _NS->list_directory_recurs();
    list<string> lstCont;
    for(vector<string>::iterator iter = vec.begin();iter!=vec.end();iter++){
      SCRUTE((*iter));
      CORBA::Object_var obj=_NS->Resolve((*iter).c_str());
      Engines::Container_var cont=Engines::Container::_narrow(obj);
      if(!CORBA::is_nil(cont)){
	lstCont.push_back((*iter));
      }
    }
    MESSAGE("Container list: ");
    for(list<string>::iterator iter=lstCont.begin();iter!=lstCont.end();iter++){
      SCRUTE((*iter));
    }
    for(list<string>::iterator iter=lstCont.begin();iter!=lstCont.end();iter++){
      SCRUTE((*iter));
      CORBA::Object_var obj=_NS->Resolve((*iter).c_str());
      Engines::Container_var cont=Engines::Container::_narrow(obj);
      if(!CORBA::is_nil(cont)){
	MESSAGE("ShutdownContainers: " << (*iter));
	cont->Shutdown();
      }
      else MESSAGE("ShutdownContainers: no container ref for " << (*iter));
    }
  }
}

//=============================================================================
/*! CORBA Method:
 *  Find a suitable Container in a list of machines, or start one
 *  \param params            Machine Parameters required for the container
 *  \param possibleComputers list of machines usable for find or start
 */
//=============================================================================

Engines::Container_ptr
SALOME_ContainerManager::
FindOrStartContainer(const Engines::MachineParameters& params,
		     const Engines::MachineList& possibleComputers)
{
  Engines::Container_ptr ret = FindContainer(params,possibleComputers);
  if(!CORBA::is_nil(ret))
    return ret;
  MESSAGE("Container doesn't exist try to launch it ...");

  return StartContainer(params,possibleComputers,Engines::P_FIRST);

}

//=============================================================================
/*! CORBA Method:
 *  Start a suitable Container in a list of machines
 *  \param params            Machine Parameters required for the container
 *  \param possibleComputers list of machines usable for start
 */
//=============================================================================

Engines::Container_ptr
SALOME_ContainerManager::
StartContainer(const Engines::MachineParameters& params,
	       const Engines::MachineList& possibleComputers,
	       Engines::ResPolicy policy)
{
  long id;
  string containerNameInNS;
  char idc[3*sizeof(long)];
  Engines::Container_ptr ret = Engines::Container::_nil();

  MESSAGE("SALOME_ContainerManager::StartContainer " <<
	  possibleComputers.length());

  string theMachine;
  try{
    switch(policy){
    case Engines::P_FIRST:
      theMachine=_ResManager->FindFirst(possibleComputers);
      break;
    case Engines::P_CYCL:
      theMachine=_ResManager->FindNext(possibleComputers);
      break;
    case Engines::P_BEST:
      theMachine=_ResManager->FindBest(possibleComputers);
      break;
    }
  }
  catch( const SALOME_Exception &ex ){
    MESSAGE(ex.what());
    return Engines::Container::_nil();
  }

  MESSAGE("try to launch it on " << theMachine);

  // Get Id for container: a parallel container registers in Naming Service
  // on the machine where is process 0. ContainerManager does'nt know the name
  // of this machine before the launch of the parallel container. So to get
  // the IOR of the parallel container in Naming Service, ContainerManager
  // gives a unique Id. The parallel container registers his name under
  // /ContainerManager/Id directory in NamingService

  id = GetIdForContainer();

  string command;
  if(theMachine==""){
    MESSAGE("SALOME_ContainerManager::StartContainer : " <<
	    "no possible computer");
    return Engines::Container::_nil();
  }
  else if(theMachine==GetHostname())
    command=_ResManager->BuildCommandToLaunchLocalContainer(params,id);
  else
    command = _ResManager->BuildCommandToLaunchRemoteContainer(theMachine,params,id);

  _ResManager->RmTmpFile();
  int status=system(command.c_str());
  if (status == -1){
    MESSAGE("SALOME_LifeCycleCORBA::StartOrFindContainer rsh failed " <<
	    "(system command status -1)");
    return Engines::Container::_nil();
  }
  else if (status == 217){
    MESSAGE("SALOME_LifeCycleCORBA::StartOrFindContainer rsh failed " <<
	    "(system command status 217)");
    return Engines::Container::_nil();
  }
  else{
    int count=TIME_OUT_TO_LAUNCH_CONT;
    MESSAGE("count = "<<count);
    while ( CORBA::is_nil(ret) && count ){
#ifndef WNT
      sleep( 1 ) ;
#else
      Sleep(1000);
#endif
      count-- ;
      if ( count != 10 )
	MESSAGE( count << ". Waiting for FactoryServer on " << theMachine);

      if(params.isMPI){
	containerNameInNS = "/ContainerManager/id";
	sprintf(idc,"%ld",id);
	containerNameInNS += idc;
      }
      else
	containerNameInNS = _NS->BuildContainerNameForNS(params,theMachine.c_str());

      SCRUTE(containerNameInNS);
      CORBA::Object_var obj = _NS->Resolve(containerNameInNS.c_str());
      ret=Engines::Container::_narrow(obj);
    }
    
    if ( CORBA::is_nil(ret) )
      MESSAGE("SALOME_LifeCycleCORBA::StartOrFindContainer rsh failed");

    return ret;
  }
}

//=============================================================================
/*! CORBA Method:
 *  Start a suitable Container in a list of machines
 *  \param params            Machine Parameters required for the container
 *  \param possibleComputers list of machines usable for start
 */
//=============================================================================

Engines::Container_ptr
SALOME_ContainerManager::
StartContainer(const Engines::MachineParameters& params,
	       Engines::ResPolicy policy,
	       const Engines::CompoList& componentList)
{
  Engines::MachineList_var possibleComputers = GetFittingResources(params,componentList);
  return StartContainer(params,possibleComputers,policy);
}

//=============================================================================
/*! CORBA Method:
 *  Give a suitable Container in a list of machines
 *  \param params            Machine Parameters required for the container
 *  \param possibleComputers list of machines usable for start
 */
//=============================================================================

Engines::Container_ptr
SALOME_ContainerManager::
GiveContainer(const Engines::MachineParameters& params,
	       Engines::ResPolicy policy,
	       const Engines::CompoList& componentList)
{
  if(strcmp(getenv("SALOME_BATCH"),"1")==0)
    {
      if(_batchLaunchedContainers.empty())
	fillBatchLaunchedContainers();
      return *(_batchLaunchedContainersIter++);
    }
  return StartContainer(params,policy,componentList);
}


//=============================================================================
/*! CORBA Method:
 *  Submit a batch job on a cluster and returns the JobId
 *  \param fileToExecute      : .py/.exe/.sh/... to execute on the batch cluster
 *  \param filesToExport      : to export on the batch cluster
 *  \param NumberOfProcessors : Number of processors needed on the batch cluster
 *  \param params             : Constraints for the choice of the batch cluster
 */
//=============================================================================
CORBA::Long SALOME_ContainerManager::submitSalomeJob( const char * fileToExecute ,
						      const Engines::FilesList& filesToExport ,
						      const Engines::FilesList& filesToImport ,
						      const CORBA::Long NumberOfProcessors ,
						      const Engines::MachineParameters& params)
{
  CORBA::Long jobId;
  try{
    jobId = _ResManager->submitSalomeJob(fileToExecute, filesToExport, filesToImport, NumberOfProcessors, params);
  }
  catch(const SALOME_Exception &ex){
    INFOS("Caught exception.");
    THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
  }
  return jobId;
}

//=============================================================================
/*! CORBA Method:
 *  Query a batch job on a cluster and returns the status of job
 *  \param jobId              : identification of Salome job
 *  \param params             : Constraints for the choice of the batch cluster
 */
//=============================================================================
char* SALOME_ContainerManager::querySalomeJob( const CORBA::Long jobId, 
					       const Engines::MachineParameters& params)
{
  string status;
  try{
    status = _ResManager->querySalomeJob( jobId, params);
  }
  catch(const SALOME_Exception &ex){
    INFOS("Caught exception.");
    THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::BAD_PARAM);
  }
  return CORBA::string_dup(status.c_str());
}

//=============================================================================
/*! CORBA Method:
 *  Delete a batch job on a cluster 
 *  \param jobId              : identification of Salome job
 *  \param params             : Constraints for the choice of the batch cluster
 */
//=============================================================================
void SALOME_ContainerManager::deleteSalomeJob( const CORBA::Long jobId, 
					       const Engines::MachineParameters& params)
{
  try{
    _ResManager->deleteSalomeJob( jobId, params);
  }
  catch(const SALOME_Exception &ex){
    INFOS("Caught exception.");
    THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::BAD_PARAM);
  }
}

//=============================================================================
/*! CORBA Method:
 *  Get result files of job on a cluster
 *  \param jobId              : identification of Salome job
 *  \param params             : Constraints for the choice of the batch cluster
 */
//=============================================================================
void SALOME_ContainerManager::getResultSalomeJob( const char *directory,
						  const CORBA::Long jobId, 
						  const Engines::MachineParameters& params)
{
  try{
    _ResManager->getResultSalomeJob( directory, jobId, params);
  }
  catch(const SALOME_Exception &ex){
    INFOS("Caught exception.");
    THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::BAD_PARAM);
  }
}

//=============================================================================
/*! 
 * 
 */
//=============================================================================

Engines::MachineList *
SALOME_ContainerManager::
GetFittingResources(const Engines::MachineParameters& params,
		    const Engines::CompoList& componentList)
{
  MESSAGE("SALOME_ContainerManager::GetFittingResources");
  Engines::MachineList *ret=new Engines::MachineList;
  vector<string> vec;
  try
    {
      vec = _ResManager->GetFittingResources(params,componentList);
    }
  catch(const SALOME_Exception &ex)
    {
      INFOS("Caught exception.");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::BAD_PARAM);
      //return ret;
    }

  //  MESSAGE("Machine list length "<<vec.size());
  ret->length(vec.size());
  for(unsigned int i=0;i<vec.size();i++)
    {
      (*ret)[i]=(vec[i]).c_str();
    }
  return ret;
}

//=============================================================================
/*! 
 * 
 */
//=============================================================================

char*
SALOME_ContainerManager::
FindFirst(const Engines::MachineList& possibleComputers)
{
  string theMachine=_ResManager->FindFirst(possibleComputers);
  return CORBA::string_dup(theMachine.c_str());
}

//=============================================================================
/*! 
 * 
 */
//=============================================================================

Engines::Container_ptr
SALOME_ContainerManager::
FindContainer(const Engines::MachineParameters& params,
	      const char *theMachine)
{
  string containerNameInNS(_NS->BuildContainerNameForNS(params,theMachine));
  CORBA::Object_var obj = _NS->Resolve(containerNameInNS.c_str());
  if( !CORBA::is_nil(obj) )
    return Engines::Container::_narrow(obj);
  else
    return Engines::Container::_nil();
}

//=============================================================================
/*! 
 * 
 */
//=============================================================================

Engines::Container_ptr
SALOME_ContainerManager::
FindContainer(const Engines::MachineParameters& params,
	      const Engines::MachineList& possibleComputers)
{
  MESSAGE("FindContainer "<<possibleComputers.length());
  for(unsigned int i=0;i<possibleComputers.length();i++)
    {
      MESSAGE("FindContainer possible " << possibleComputers[i]);
      Engines::Container_ptr cont = FindContainer(params,possibleComputers[i]);
      if( !CORBA::is_nil(cont) )
	return cont;
    }
  MESSAGE("FindContainer: not found");
  return Engines::Container::_nil();
}

//=============================================================================
/*! 
 * Get Id for container: a parallel container registers in Naming Service
 * on the machine where is process 0. ContainerManager does'nt know the name
 * of this machine before the launch of the parallel container. So to get
 * the IOR of the parallel container in Naming Service, ContainerManager
 * gives a unique Id. The parallel container registers his name under
 * /ContainerManager/Id directory in NamingService
 */
//=============================================================================


long SALOME_ContainerManager::GetIdForContainer(void)
{
  _id++;
  return _id;
}

Engines::MachineParameters* SALOME_ContainerManager::GetMachineParameters(const char *hostname)
{
  return _ResManager->GetMachineParameters(hostname);
}

void SALOME_ContainerManager::fillBatchLaunchedContainers()
{
  _batchLaunchedContainers.clear();
  _NS->Change_Directory("/Containers");
  vector<string> vec = _NS->list_directory_recurs();
  for(vector<string>::iterator iter = vec.begin();iter!=vec.end();iter++){
    CORBA::Object_var obj=_NS->Resolve((*iter).c_str());
    Engines::Container_ptr cont=Engines::Container::_narrow(obj);
    if(!CORBA::is_nil(cont)){
      _batchLaunchedContainers.push_back(cont);
    }
  }
  _batchLaunchedContainersIter=_batchLaunchedContainers.begin();
}
