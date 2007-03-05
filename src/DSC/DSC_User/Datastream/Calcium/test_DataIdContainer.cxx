// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-03-01 15:07:46 +0100 (jeu, 01 mar 2007) $
// Id          : $Id$

#include "CalciumCouplingPolicy.hxx"

#include "calcium_port_provides.hxx"
#include "DatastreamException.hxx"

using namespace CalciumTypes;

class TEST1 : public CalciumCouplingPolicy {
public:
  int ret;
  TEST1() {

    ret=0;
  
    CORBA::Long time=1,tag=1;
    typedef CalciumCouplingPolicy::DataIdContainer DataIdContainer;  
    typedef CalciumCouplingPolicy::DataId          DataId;
    
    DataId          dataId(time,tag);   //potentiellement avec un troisième paramètre
    try {
      DataIdContainer dataIds(dataId,*this);   

      DataIdContainer::iterator dataIdIt = dataIds.begin();
      
      if (!dataIds.empty())
	for (;dataIdIt != dataIds.end();++dataIdIt) {
	  std::cout << "(*dataIdIt) must be equal to given dataId parameter : " << *dataIdIt;
	  std::cout << " == " << dataId << " : " << (ret = (*dataIdIt == dataId)) << std::endl;
	}
    } catch(const DATASTREAM_EXCEPTION & ex) {
      ret=1;
      std::cout << ex.what() << std::endl;
    }

  }
};

class TEST2 : public CalciumCouplingPolicy {
    
public:
  int ret;
  TEST2() {

    //	Doit filtrer le mode de dépendance temporel car le mode est 
    //  défini itératif
    ret=0;

    CORBA::Long time=1,tag=1;
    typedef CalciumCouplingPolicy::DataIdContainer DataIdContainer;  
    typedef CalciumCouplingPolicy::DataId          DataId;
    
    DataId          dataId(time,tag);   //potentiellement avec un troisième paramètre
    setDependencyType(ITERATION_DEPENDENCY);
    DataIdContainer dataIds(dataId,*this);   


    DataIdContainer::iterator dataIdIt = dataIds.begin();

    if (!dataIds.empty())
      for (;dataIdIt != dataIds.end();++dataIdIt) {
	std::cout << "(*dataIdIt) must be equal to given dataId parameter : " << *dataIdIt ;
	std::cout << " == " << DataId(0,tag) << " : " << (ret = (*dataIdIt == DataId(0,tag))) << std::endl;

      }
  }
};
 

class TEST3 : public CalciumCouplingPolicy {
public:
  int ret;

  TEST3() {

    //	Doit filtrer le mode de dépendance temporel car le mode est 
    //  défini itératif
    ret=0;

    CORBA::Long time=1,tag=1;
    typedef CalciumCouplingPolicy::DataIdContainer DataIdContainer;  
    typedef CalciumCouplingPolicy::DataId          DataId;
    
    DataId          dataId(time,tag);   //potentiellement avec un troisième paramètre
    setDependencyType(TIME_DEPENDENCY);
    DataIdContainer dataIds(dataId,*this);   


    DataIdContainer::iterator dataIdIt = dataIds.begin();

    if (!dataIds.empty())
      for (;dataIdIt != dataIds.end();++dataIdIt) {
	std::cout << "(*dataIdIt) must be equal to given dataId parameter : " << *dataIdIt ;
	std::cout << " == " << DataId(time,0) << " : " << (ret = (*dataIdIt == DataId(time,0))) << std::endl;

      }
  }
};
 
int main() {
  TEST1 test1;
  TEST2 test2;
  TEST3 test3;
  return !test1.ret+!test2.ret+!test3.ret;
};

