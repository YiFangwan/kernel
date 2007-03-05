#ifndef _CALCIUM_TYPES_MANIPULATOR_HXX_
#define _CALCIUM_TYPES_MANIPULATOR_HXX_

//Cette classe n'est pas necessaire pour l'instant.
#include "Calcium_Ports.hh"

template <typename PortType> struct CalciumTypesManipulator : 
  public seq_u_manipulation<Ports::Calcium_Ports::seq_complex,
			    Ports::Calcium_Ports::Complex> {
  static bool needInterpolation=true;
}

template <POA_Ports::Calcium_Ports::Calcium_Complex_Port> struct CalciumTypesManipulator : 
  public seq_u_manipulation<Ports::Calcium_Ports::seq_complex,
			    Ports::Calcium_Ports::Complex> {
  static bool needInterpolation=true;
}

#endif
