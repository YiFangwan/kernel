// André Ribes - EDF R&D 2006
//

#include "basic_port_factory.hxx"

basic_port_factory::basic_port_factory() {}

basic_port_factory::~basic_port_factory() {}

provides_port *
basic_port_factory::create_data_servant(string type) {
  provides_port * rtn_port = NULL;
  if (type == "short") {
    rtn_port = new data_short_port_provides();  
  }
  return rtn_port;
}

uses_port * 
basic_port_factory::create_data_proxy(string type) {
  uses_port * rtn_port = NULL;
  if (type == "short")
    rtn_port = new data_short_port_uses();
  return rtn_port;
}
