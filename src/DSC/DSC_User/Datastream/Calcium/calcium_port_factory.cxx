// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-01-08 19:01:14 +0100 (lun, 08 jan 2007) $
// Id          : $Id$

#include "calcium_port_factory.hxx"

calcium_port_factory::calcium_port_factory() {}

calcium_port_factory::~calcium_port_factory() {}

provides_port *
calcium_port_factory::create_data_servant(string type) {
  provides_port * rtn_port = NULL;

  if ( type == "integer")
    rtn_port = new calcium_integer_port_provides();
  if ( type == "real")
    rtn_port = new calcium_real_port_provides();
  if ( type == "double")
    rtn_port = new calcium_double_port_provides();
  if ( type == "string")
    rtn_port = new calcium_string_port_provides();
  if ( type == "logical")
    rtn_port = new calcium_logical_port_provides();
  if ( type == "complex")
    rtn_port = new calcium_complex_port_provides();

  return rtn_port;
}

uses_port * 
calcium_port_factory::create_data_proxy(string type) {
  uses_port * rtn_port = NULL;

  if ( type == "integer")
    rtn_port = new calcium_integer_port_uses();
  if ( type == "real")
    rtn_port = new calcium_real_port_uses();
  if ( type == "double")
    rtn_port = new calcium_double_port_uses();
  if ( type == "string")
    rtn_port = new calcium_string_port_uses();
  if ( type == "logical")
    rtn_port = new calcium_logical_port_uses();
  if ( type == "complex")
    rtn_port = new calcium_complex_port_uses();

  return rtn_port;
}
