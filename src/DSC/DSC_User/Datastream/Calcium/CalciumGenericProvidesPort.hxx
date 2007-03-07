//  Copyright (C) 2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : CalciumGenericProvidesPort.hxx
//  Author : Eric Fayolle (EDF)
//  Module : KERNEL
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
