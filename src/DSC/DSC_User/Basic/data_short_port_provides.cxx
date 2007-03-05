// André Ribes EDF R&D - 2006
// 
#include "data_short_port_provides.hxx"

data_short_port_provides::data_short_port_provides() {
  _val = 0;
}

data_short_port_provides::~data_short_port_provides() {}

void
data_short_port_provides::put(CORBA::Short data) {
  _val = data;
}

CORBA::Short
data_short_port_provides::get() {
  return _val;
}

CORBA::Short
data_short_port_provides::get_local() {
  return _val;
}

Ports::Port_ptr
data_short_port_provides::get_port_ref() {
  return this->_this();
}
