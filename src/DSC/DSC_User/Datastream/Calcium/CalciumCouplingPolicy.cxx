// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-02-28 15:26:32 +0100 (mer, 28 f√©v 2007) $
// Id          : $Id$

#include "CalciumCouplingPolicy.hxx"

CalciumCouplingPolicy::CalciumCouplingPolicy():_dependencyType(CalciumTypes::UNDEFINED_DEPENDENCY),
					       _storageLevel(CalciumTypes::UNLIMITED_STORAGE_LEVEL),
					       _dateCalSchem(CalciumTypes::TI_SCHEM),
					       _interpolationSchem(CalciumTypes::L1_SCHEM),
					       _extrapolationSchem(CalciumTypes::UNDEFINED_EXTRA_SCHEM),
					       _alpha(0.0),_deltaT(CalciumTypes::EPSILON),
					       _disconnectDirective(CalciumTypes::UNDEFINED_DIRECTIVE){};

void CalciumCouplingPolicy::setDependencyType (CalciumTypes::DependencyType dependencyType) {_dependencyType=dependencyType;}
CalciumTypes::DependencyType CalciumCouplingPolicy::getDependencyType () const                        { return _dependencyType;}
 
void   CalciumCouplingPolicy::setStorageLevel   (size_t         storageLevel)   {
  if ( storageLevel < 1  )
    throw DATASTREAM_EXCEPTION(LOC("Un niveau < 1 n'est pas autorisÈ"));
  _storageLevel = storageLevel;
}
size_t CalciumCouplingPolicy::getStorageLevel   () const                        {return _storageLevel;}
void   CalciumCouplingPolicy::setDateCalSchem   (CalciumTypes::DateCalSchem   dateCalSchem)   {
  if ( _dependencyType != CalciumTypes::TIME_DEPENDENCY )
    throw DATASTREAM_EXCEPTION(LOC("Il est impossible de positionner un schÈma temporel sur un port en dÈpendance itÈrative"));
  _dateCalSchem = dateCalSchem;
}

CalciumTypes::DateCalSchem CalciumCouplingPolicy::getDateCalSchem () const   {
  if ( _dependencyType != CalciumTypes::TIME_DEPENDENCY )
    throw DATASTREAM_EXCEPTION(LOC("Un schÈma temporel sur un port en dÈpendance itÈrative n'a pas de sens"));
  return _dateCalSchem;
}

void CalciumCouplingPolicy::setAlpha(double alpha) {
  if ( _dependencyType != CalciumTypes::TIME_DEPENDENCY )
    throw DATASTREAM_EXCEPTION(LOC("Il est impossible de positionner alpha sur un port en dÈpendance itÈrative"));
  
  if ( 0 <= alpha <= 1 ) _alpha = alpha; 
  else 
    throw DATASTREAM_EXCEPTION(LOC("Le paramËtre alpha doit Ítre compris entre [0,1]"));
}

double CalciumCouplingPolicy::getAlpha() const  {
  if ( _dependencyType != CalciumTypes::TIME_DEPENDENCY )
    throw DATASTREAM_EXCEPTION(LOC("Le paramËtre alpha sur un port en dÈpendance itÈrative n'a pas de sens"));
  return _alpha;
}

void CalciumCouplingPolicy::setDeltaT(double deltaT ) {
  if ( 0 <= deltaT <= 1 ) _deltaT = deltaT; 
  else 
    throw(DATASTREAM_EXCEPTION(LOC("Le paramËtre deltaT doit Ítre compris entre [0,1]")));
}
double CalciumCouplingPolicy::getdeltaT() const  {return _deltaT;}

void CalciumCouplingPolicy::setInterpolationSchem (CalciumTypes::InterpolationSchem interpolationSchem) {_interpolationSchem=interpolationSchem;}
void CalciumCouplingPolicy::setExtrapolationSchem (CalciumTypes::ExtrapolationSchem extrapolationSchem) {_extrapolationSchem=extrapolationSchem;}


CalciumCouplingPolicy::TimeType 
CalciumCouplingPolicy::getEffectiveTime(CalciumCouplingPolicy::TimeType ti, 
					CalciumCouplingPolicy::TimeType tf) {
  if ( _dateCalSchem == CalciumTypes::TI_SCHEM )  return ti;
  if ( _dateCalSchem == CalciumTypes::TF_SCHEM )  return tf;
  
  //CalciumTypes::ALPHA_SCHEM
  return tf*_alpha + ti*(1-_alpha);
}

void CalciumCouplingPolicy::disconnect(bool provideLastGivenValue) {
  if (provideLastGivenValue) {
    std::cout << "-------- CalciumCouplingPolicy::disconnect CP_CONT  ------------------" << std::endl;
    _disconnectDirective = CalciumTypes::CONTINUE;
  } else {
    std::cout << "-------- CalciumCouplingPolicy::disconnect CP_ARRET  ------------------" << std::endl;
    _disconnectDirective = CalciumTypes::STOP;
  }
}
