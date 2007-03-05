// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-01-24 16:30:34 +0100 (mer, 24 jan 2007) $
// Id          : $Id$


#ifndef _ADJACENT_FUNCTOR_HXX_
#define _ADJACENT_FUNCTOR_HXX_

#include "ConstTraits.hxx"
// Pour affichage
#include "DisplayPair.hxx"
//

// Suppose que le container est trié
template < typename T > struct AdjacentFunctor {

  typedef typename ConstTrait<T>::NonConstType TNoConst;
  const T & _minValue;
  T         _maxValue;
  TNoConst  _max;
  TNoConst  _min;
  bool      _minFound,_maxFound,_equal;
  
  AdjacentFunctor(const T& value):_minValue(value),_maxValue(value),
				  _minFound(false),_maxFound(false),
				  _equal(false) {}

  // Suppose que les valeurs passées en paramètres sont triées par ordre croissant
  bool operator()(const T &v1) {
    if ( v1 < _minValue)    {
      std::cout << "EE1: _min : " << _min << std::endl;
      _min=v1;_minFound=true;
      std::cout << "AdjacentFunctor: _minFound : " <<_min << ", _minValue " << _minValue << std::endl;
    } else if ( v1 > _maxValue ) {
      _max=v1;_maxFound=true;
      std::cout << "AdjacentFunctor: _maxFound : " <<_max << ", _maxValue " << _maxValue << std::endl;
    } else {
      _equal= true;
      std::cout << "AdjacentFunctor: _equal : " << v1<< ", _minValue " << _minValue << ", _maxValue " << _maxValue << std::endl;   
      return true; 
    } // end if
    
    std::cout << "AdjacentFunctor: _minFound : " <<_min << ", _maxFound " << _max << std::endl;
    return  ( _minFound && _maxFound );
  }

  void setMaxValue(const T & value) {_maxValue = value;}
  bool isEqual()   const { return _equal;}
  bool isBounded() const { return _minFound && _maxFound;}
  bool getBounds(TNoConst & min, TNoConst & max) const {
    std::cout << "_minFound : " <<_minFound << ", _maxFound " << _maxFound << std::endl;
    if (_minFound && _maxFound ) { min=_min; max=_max; return true; }
    return false;
  }
  void reset() { _minFound = false; _maxFound = false; _equal = false; };
};

#endif
