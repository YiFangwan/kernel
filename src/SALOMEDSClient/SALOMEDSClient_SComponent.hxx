//  File   : SALOMEDSClient_SComponent.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDSCLIENT_SCOMPONENT_H__
#define __SALOMEDSCLIENT_SCOMPONENT_H__

#include "SALOMEDSClient_SObject.hxx"
#include <string> 

class SALOMEDSClient_SComponent: public virtual SALOMEDSClient_SObject
{
public:
  
  virtual std::string ComponentDataType() = 0;
  virtual bool ComponentIOR(std::string& theID) = 0;    

};
#endif
