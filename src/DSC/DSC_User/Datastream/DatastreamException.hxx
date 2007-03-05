// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-02-06 17:20:28 +0100 (mar, 06 fév 2007) $
// Id          : $Id$

#ifndef DATASTREAM_EXCEPTION_HXX
#define DATASTREAM_EXCEPTION_HXX

#include "DSC_Exception.hxx"
#include "CalciumTypes.hxx"

struct DatastreamException  : public DSC_Exception {

  DatastreamException( const CalciumTypes::InfoType info,
		       const string &     text, 
		       const char        *fileName, 
		       const unsigned int lineNumber,
		       const char        *funcName):
  DSC_Exception(text,fileName,lineNumber,funcName),_info(info)
  {};


  DatastreamException( const CalciumTypes::InfoType info,
		       const string & text ):
    DSC_Exception(text),_info(info)
  {};

  DatastreamException(CalciumTypes::InfoType info, const DSC_Exception & ex ):
    DSC_Exception(ex),_info(info) {};
 
  virtual ~DatastreamException() throw() {};
  CalciumTypes::InfoType getInfo() const { return _info;}
  void setInfo(CalciumTypes::InfoType info) {_info=info;}

private:
  CalciumTypes::InfoType _info;
};


#endif /* DATASTREAM_EXCEPTION_HXX */
