#include "SALOME_ContainerManager.hxx"
#include "SALOME_NamingService.hxx"
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"
#include "LocalTraceCollector.hxx"

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
  ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
  CORBA::ORB_var &orb = init( argc , argv ) ;
  LocalTraceCollector *myThreadTrace = LocalTraceCollector::instance(orb);
  try { 
    SALOME_NamingService *ns=new SALOME_NamingService(orb);
    SALOME_ContainerManager *contManager=new SALOME_ContainerManager(ns);
    Engines::ContainerManager_ptr contManagerServant=contManager->_this();
    ns->Register(contManagerServant,SALOME_ContainerManager::_ContainerManagerNameInNS);
    CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var root_poa = PortableServer::POA::_narrow(obj);
    PortableServer::POAManager_var pman = root_poa->the_POAManager();
    pman->activate();
    orb->run();
    delete myThreadTrace;
  }catch(CORBA::SystemException&){
    INFOS("Caught CORBA::SystemException.");
  }catch(PortableServer::POA::WrongPolicy&){
    INFOS("Caught CORBA::WrongPolicyException.");
  }catch(PortableServer::POA::ServantAlreadyActive&){
    INFOS("Caught CORBA::ServantAlreadyActiveException");
  }catch(CORBA::Exception&){
    INFOS("Caught CORBA::Exception.");
  }catch(std::exception& exc){
    INFOS("Caught std::exception - "<<exc.what()); 
  }catch(...){
    INFOS("Caught unknown exception.");
  }
}

