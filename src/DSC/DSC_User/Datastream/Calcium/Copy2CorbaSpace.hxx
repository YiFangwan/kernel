// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-02-07 18:26:44 +0100 (mer, 07 f√©v 2007) $
// Id          : $Id$

#ifndef _COPY_TO_CORBA_SPACE_HXX_
#define _COPY_TO_CORBA_SPACE_HXX_

#include <string>
#include <iostream>
#include "CalciumPortTraits.hxx"

template <bool zerocopy> 
struct Copy2CorbaSpace  {

  template <class T1, class T2>
  static void apply( T1 * & corbaData, T2 & data, size_t nRead){

    typedef typename ProvidesPortTraits<T2>::PortType  PortType;
    //typedef typename UsesPortTraits<T2>::PortType      PortType;
    typedef typename PortType::DataManipulator         DataManipulator;
    typedef typename DataManipulator::InnerType        InnerType;

    std::cerr << "-------- Copy2CorbaSpace<true> MARK 1 ------------------" << std::endl;
    // CrÈe le type corba ‡ partir du data sans lui en donner la propriÈtÈ
    corbaData = DataManipulator::create(nRead,&data,false);
    std::cerr << "-------- Copy2CorbaSpace<true> MARK 2 --(dataPtr : " 
	      << DataManipulator::getPointer(corbaData,false)<<")----------------" << std::endl;

  }
};

// Cas ou il faut effectuer une recopie
template <> struct
Copy2CorbaSpace<false>  {
  
  template <class T1, class T2>
  static void apply( T1 * & corbaData,  T2 & data, size_t nRead){

    typedef typename ProvidesPortTraits<T2>::PortType  PortType;
    // typedef typename UsesPortTraits<T2>::PortType     PortType;
    typedef typename PortType::DataManipulator        DataManipulator;
    typedef typename DataManipulator::InnerType       InnerType;

    corbaData = DataManipulator::create(nRead);
    InnerType * dataPtr  = DataManipulator::getPointer(corbaData,false);

    std::cerr << "-------- Copy2CorbaSpace<false> MARK 1 --(dataPtr : " <<
      dataPtr<<")----------------" << std::endl;
    
    std::copy(&data,&data+nRead,dataPtr);
 
    std::cerr << "-------- Copy2CorbaSpace<false> MARK 2 --(nRead: "<<nRead<<")-------------" << std::endl;
 
    std::cerr << "-------- Copy2CorbaSpace<false> MARK 3 : " ;
    std::copy(dataPtr,dataPtr+nRead,std::ostream_iterator<InnerType>(std::cout," "));
    std::cout << std::endl;
    std::cerr << "-------- Copy2CorbaSpace<false> MARK 4 --(data : " <<data<<") :" ;
    for (int i=0; i<nRead; ++i)
      std::cerr << (*corbaData)[i] << " ";
    std::cout << std::endl;
    
  }
};

#endif
