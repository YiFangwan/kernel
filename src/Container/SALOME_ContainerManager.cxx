#include "SALOME_ContainerManager.hxx"
#include "SALOME_NamingService.hxx"
#include "OpUtil.hxx"
#include <sys/types.h>
#include <unistd.h>
#define TIME_OUT_TO_LAUNCH_CONT 21

using namespace std;

const char *SALOME_ContainerManager::_ContainerManagerNameInNS="ContainerManager";

SALOME_ContainerManager::SALOME_ContainerManager(SALOME_NamingService *ns):_NS(ns)
{
}

Engines::Container_ptr SALOME_ContainerManager::FindOrStartContainer(const char *containerName, const MachineList& possibleComputers)
{
  Engines::Container_ptr ret=DoesExistContainer(containerName,possibleComputers);
  if(!CORBA::is_nil(ret))
    return ret;
  // Container doesn't exist try to launch it ...
  vector<string> vector;
  string theMachine=_LoadManager.FindBest(possibleComputers);
  string command;
  if(theMachine==GetHostname())
    command=_ResManager.BuildCommandToLaunchLocalContainer(containerName);
  else
    command=_ResManager.BuildTempFileToLaunchRemoteContainer(theMachine,containerName);
  _ResManager.RmTmpFile();
  int status=system(command.c_str());
  if (status == -1) {
    MESSAGE("SALOME_LifeCycleCORBA::StartOrFindContainer rsh failed (system command status -1)");
    return Engines::Container::_nil();
  }
  else if (status == 217) {
    MESSAGE("SALOME_LifeCycleCORBA::StartOrFindContainer rsh failed (system command status 217)");
    return Engines::Container::_nil();
  }
  else {
    int count=TIME_OUT_TO_LAUNCH_CONT;
    while ( CORBA::is_nil(ret) && count ) {
      sleep( 1 ) ;
      count-- ;
      if ( count != 10 )
	MESSAGE( count << ". Waiting for FactoryServer on " << theMachine);
      string containerNameInNS=BuildContainerNameInNS(containerName,theMachine.c_str());
      CORBA::Object_var obj = _NS->Resolve(containerNameInNS.c_str());
      ret=Engines::Container::_narrow(obj);
    }
    if ( CORBA::is_nil(ret) ) {
      MESSAGE("SALOME_LifeCycleCORBA::StartOrFindContainer rsh failed");
    }
    return ret;
  }
}

MachineList *SALOME_ContainerManager::GetResourcesFitting(const MachineParameters& params, const char *componentName)
{
  vector<string> vec=_ResManager.GetResourcesFitting(params,componentName);
  MachineList *ret=new MachineList;
  ret->length(vec.size());
  for(unsigned int i=0;i<vec.size();i++)
    {
      (*ret)[i]=(vec[i]).c_str();
    }
  return ret;
}

char* SALOME_ContainerManager::FindBest(const MachineList& possibleComputers)
  {
    string theMachine=_LoadManager.FindBest(possibleComputers);
    return CORBA::string_dup(theMachine.c_str());
  }

void SALOME_ContainerManager::ShutdownContainers()
{
}

Engines::Container_ptr SALOME_ContainerManager::FindContainer(const char *containerName,const char *theMachine)
{
  string containerNameInNS(BuildContainerNameInNS(containerName,theMachine));
  CORBA::Object_var obj = _NS->Resolve(containerNameInNS.c_str());
  if( !CORBA::is_nil(obj) )
    return Engines::Container::_narrow(obj);
  else
    return Engines::Container::_nil();
}

Engines::Container_ptr SALOME_ContainerManager::DoesExistContainer(const char *containerName,const MachineList& possibleComputers)
{
  for(unsigned int i=0;i<possibleComputers.length();i++)
    {
      Engines::Container_ptr cont=FindContainer(containerName,possibleComputers[i]);
      if( !CORBA::is_nil(cont) )
	return cont;
    }
  return Engines::Container::_nil();
}

string SALOME_ContainerManager::BuildContainerNameInNS(const char *containerName,const char *machineName)
{
  string containerNameInNS("/Containers/");
  containerNameInNS+=machineName;
  containerNameInNS+="/";
  containerNameInNS+=containerName;
  return containerNameInNS;
}
