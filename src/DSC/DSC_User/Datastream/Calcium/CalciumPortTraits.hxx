// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-02-28 15:26:32 +0100 (mer, 28 f√©v 2007) $
// Id          : $Id$

#ifndef _PORT_TRAITS_HXX_
#define _PORT_TRAITS_HXX_

#include "Superv_Component_i.hxx"


struct UnknownPortType {};
template <class T> struct ProvidesPortTraits {
  typedef  UnknownPortType PortType;
};
template <> struct ProvidesPortTraits<int> {
  typedef  calcium_integer_port_provides PortType;
};
template <> struct ProvidesPortTraits<long> {
  typedef  calcium_integer_port_provides PortType;
};
template <> struct ProvidesPortTraits<float> {
  typedef  calcium_real_port_provides PortType;
};
template <> struct ProvidesPortTraits<double> {
  typedef  calcium_double_port_provides PortType;
};
template <> struct ProvidesPortTraits<char *> {
  typedef  calcium_string_port_provides PortType;
};
template <> struct ProvidesPortTraits<bool> {
  typedef  calcium_logical_port_provides PortType;
};

// DÈfinition du type cplx pour disciminer ce type de port
// de celui du float 
struct cplx {};
template <> struct ProvidesPortTraits<cplx> {
   typedef calcium_complex_port_provides PortType;
};

template < typename T > struct StarTrait        { typedef  T NonStarType; };
template < typename T > struct StarTrait< T * > { typedef  T NonStarType; };

template <class T> struct UsesPortTraits {
  typedef  UnknownPortType PortType;
};
template <> struct UsesPortTraits<int> {
  typedef  calcium_integer_port_uses PortType;
};
template <> struct UsesPortTraits<long> {
  typedef  calcium_integer_port_uses PortType;
};
template <> struct UsesPortTraits<float> {
  typedef  calcium_real_port_uses PortType;
};
template <> struct UsesPortTraits<double> {
  typedef  calcium_double_port_uses PortType;
};
template <> struct UsesPortTraits<char *> {
  typedef  calcium_string_port_uses PortType;
};
template <> struct UsesPortTraits<bool> {
  typedef  calcium_logical_port_uses PortType;
};
template <> struct UsesPortTraits<cplx> {
   typedef  calcium_complex_port_uses PortType;
};

#endif
