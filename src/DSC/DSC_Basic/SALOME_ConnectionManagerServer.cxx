#include "ConnectionManager_i.hxx"
#include "utilities.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
  PortableServer::POA_var root_poa;
  PortableServer::POAManager_var pman;
  CORBA::Object_var obj;
  CORBA::ORB_var orb = CORBA::ORB_init( argc , argv ) ;
  try{
    obj = orb->resolve_initial_references("RootPOA");
    if(!CORBA::is_nil(obj))
      root_poa = PortableServer::POA::_narrow(obj);
    if(!CORBA::is_nil(root_poa))
      pman = root_poa->the_POAManager();
    ConnectionManager_i * serv = new ConnectionManager_i(orb);
    pman->activate();
    orb->run();
  }catch(CORBA::SystemException&){
    MESSAGE("Caught CORBA::SystemException.");
  }catch(PortableServer::POA::WrongPolicy&){
    MESSAGE("Caught CORBA::WrongPolicyException.");
  }catch(PortableServer::POA::ServantAlreadyActive&){
    MESSAGE("Caught CORBA::ServantAlreadyActiveException");
  }catch(CORBA::Exception&){
    MESSAGE("Caught CORBA::Exception.");
  }catch(std::exception& exc){
    MESSAGE("Caught std::exception - "<<exc.what()); 
  }catch(...){
    MESSAGE("Caught unknown exception.");
  }
  END_OF(argv[0]);
}

