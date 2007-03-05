// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-01-08 19:01:14 +0100 (lun, 08 jan 2007) $
// Id          : $Id$


#include "palm_port_factory.hxx"

palm_port_factory::palm_port_factory() {
}

palm_port_factory::~palm_port_factory() {}

provides_port *
palm_port_factory::create_data_servant(string type) {
  provides_port * rtn_port = NULL;
  if (type == "short") {
    rtn_port = new palm_data_short_port_provides();
  }
  if (type == "seq_short") {
    rtn_port = new palm_data_seq_short_port_provides();
  }
  return rtn_port;
}

uses_port * 
palm_port_factory::create_data_proxy(string type) {
  uses_port * rtn_port = NULL;
  return rtn_port;
}
