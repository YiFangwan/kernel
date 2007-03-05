// André Ribes EDF R&D - 2006
//

#include "data_short_port_uses.hxx"
#include <iostream>
using namespace std;

data_short_port_uses::data_short_port_uses() {
  _my_port = Ports::Data_Short_Port::_nil();
  _my_ports = NULL;
}

data_short_port_uses::~data_short_port_uses() {}

const char *
data_short_port_uses::get_repository_id() {
  return "IDL:Ports/Data_Short_Port:1.0";
}

bool
data_short_port_uses::set_port(Ports::Port_ptr port) {
  _my_port = Ports::Data_Short_Port::_narrow(port);
}

void
data_short_port_uses::put(CORBA::Short data) {
//  if (!CORBA::is_nil(_my_port))
//    _my_port->put(data);
  if (!_my_ports)
    cerr << "data_short_port_uses::put is NULL" << endl;
  else
  {
    for(int i = 0; i < _my_ports->length(); i++)
    {
      Ports::Data_Short_Port_ptr port = Ports::Data_Short_Port::_narrow((*_my_ports)[i]);
      port->put(data);
    }
  }
}

void 
data_short_port_uses::uses_port_changed(Engines::DSC::uses_port * new_uses_port,
		       const Engines::DSC::Message message)
{
  if (_my_ports)
    delete _my_ports;

  cerr << "data_short_port_uses::uses_port_changed" << endl;
  _my_ports = new Engines::DSC::uses_port(*new_uses_port);
}
