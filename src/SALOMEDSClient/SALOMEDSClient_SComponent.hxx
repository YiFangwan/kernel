//  File   : SALOMEDSClient_SComponent.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDSCLIENT_SCOMPONENT_H__
#define __SALOMEDSCLIENT_SCOMPONENT_H__

//SALOMEDSClient headers
#include "SALOMEDSClient_SObject.hxx"

class SALOMEDSClient_SComponent: public virtual SALOMEDSClient_SObject
{
public:
  
  virtual char* ComponentDataType() = 0;
  virtual bool ComponentIOR(char* theID) = 0;    

};
#endif
