#ifndef __SALOME_CONTAINERMANAGER_HXX__
#define __SALOME_CONTAINERMANAGER_HXX__

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Component)
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)
#include "SALOME_ResourcesManager.hxx"
#include "SALOME_LoadRateManager.hxx"

#include <string>
#include <vector>

class SALOME_NamingService;

class SALOME_ContainerManager: public POA_Engines::ContainerManager,
			       public PortableServer::RefCountServantBase {
private:
  SALOME_ResourcesManager _ResManager;
  SALOME_LoadRateManager _LoadManager;
  SALOME_NamingService *_NS;
public:
  SALOME_ContainerManager(SALOME_NamingService *ns);
  Engines::Container_ptr FindOrStartContainer(const char *containerName, const MachineList& possibleComputer);
  MachineList *GetResourcesFitting(const MachineParameters& params, const char *componentName);
  char* FindBest(const MachineList& possibleComputers);
  void ShutdownContainers();

  static const char *_ContainerManagerNameInNS;
private:
  Engines::Container_ptr DoesExistContainer(const char *containerName,const MachineList& possibleComputers);
  Engines::Container_ptr FindContainer(const char *containerName,const char *theMachine);
  std::string BuildContainerNameInNS(const char *containerName,const char *machineName);
};

#endif
