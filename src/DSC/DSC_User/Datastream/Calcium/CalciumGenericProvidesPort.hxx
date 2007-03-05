// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-03-01 13:40:26 +0100 (jeu, 01 mar 2007) $
// Id          : $Id$

#ifndef _CALCIUM_GENERIC_PROVIDES_PORT_HXX_
#define _CALCIUM_GENERIC_PROVIDES_PORT_HXX_

//include utile ? :
#include "Calcium_Ports.hh"

#include "provides_port.hxx"

#include "GenericPort.hxx"
#include "CalciumCouplingPolicy.hxx"

// TODO: Creer un trait qui à partir de CorbaInterface déduit CorbaDataManipulator
// et simplifier la classe
 
//
//  Avant d'utiliser la macro : template <typename CorbaInterface, typename CorbaDataManipulator > 
// 
// utilisation de __VA_ARGS__ au lieu de CorbaDataManipulator car à l'appel de la
// macro le token du troisième argument contient une virgule qui est considéré comme
// séparateur d'argument par le PP 
#define CALCIUM_GENERIC_PROVIDES_PORT(specificPortName,CorbaInterface,...) \
  class specificPortName :   public virtual CorbaInterface ,		\
			     public virtual provides_port,		\
			     public GenericPort< __VA_ARGS__ , CalciumCouplingPolicy > { \
  public :								\
    typedef  __VA_ARGS__               DataManipulator;			\
    typedef  DataManipulator::Type     CorbaDataType;			\
    typedef GenericPort< DataManipulator ,				\
      CalciumCouplingPolicy >                  Port;			\
  									\
    virtual ~ specificPortName ();					\
									\
    inline void disconnect(bool provideLastGivenValue) {		\
      Port::disconnect(provideLastGivenValue);				\
    }									\
									\
    inline void put( DataManipulator::CorbaInType data,			\
		     CORBA::Double time, CORBA::Long tag) {		\
      Port::put(data, time, tag);					\
    }									\
									\
    inline Ports::Port_ptr get_port_ref() {				\
      return _this();							\
    } 									\
  };									\
  

#endif
