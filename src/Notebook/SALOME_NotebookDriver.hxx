
#ifndef SALOME_NOTEBOOK_DRIVER_HEADER
#define SALOME_NOTEBOOK_DRIVER_HEADER

#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOME_Component)
#include <string>

class SALOME_NotebookDriver : public POA_SALOMEDS::Driver
{
public:
  SALOME_NotebookDriver();
  virtual ~SALOME_NotebookDriver();

  virtual SALOMEDS::TMPFile* Save      ( SALOMEDS::SComponent_ptr theComponent, const char* theURL, bool isMultiFile );
  virtual SALOMEDS::TMPFile* SaveASCII ( SALOMEDS::SComponent_ptr theComponent, const char* theURL, bool isMultiFile );
  virtual CORBA::Boolean     Load      ( SALOMEDS::SComponent_ptr theComponent, const SALOMEDS::TMPFile& theStream,
                                         const char* theURL, bool isMultiFile );
  virtual CORBA::Boolean     LoadASCII ( SALOMEDS::SComponent_ptr theComponent, const SALOMEDS::TMPFile& theStream,
                                         const char* theURL, bool isMultiFile );
  Engines::TMPFile*          DumpPython( SALOMEDS::Study_ptr theStudy, bool& isValid ) const;
  virtual void               Close     ( SALOMEDS::SComponent_ptr theComponent );

  virtual char* ComponentDataType();
  virtual char* IORToLocalPersistentID( SALOMEDS::SObject_ptr theSObject, const char* theIOR,
                                        CORBA::Boolean isMultiFile, CORBA::Boolean isASCII );
  virtual char* LocalPersistentIDToIOR( SALOMEDS::SObject_ptr theSObject, const char* theLocalPersistentID,
                                        CORBA::Boolean isMultiFile, CORBA::Boolean isASCII );

  virtual bool                  CanPublishInStudy        ( CORBA::Object_ptr theIOR );
  virtual SALOMEDS::SObject_ptr PublishInStudy           ( SALOMEDS::Study_ptr theStudy, SALOMEDS::SObject_ptr theSObject,
                                                           CORBA::Object_ptr theObject, const char* theName );

  virtual CORBA::Boolean        CanCopy  ( SALOMEDS::SObject_ptr theObject );
  virtual SALOMEDS::TMPFile*    CopyFrom ( SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID );
  virtual CORBA::Boolean        CanPaste ( const char* theComponentName, CORBA::Long theObjectID );
  virtual SALOMEDS::SObject_ptr PasteInto( const SALOMEDS::TMPFile& theStream, CORBA::Long theObjectID,
                                           SALOMEDS::SObject_ptr theObject );

  static SALOMEDS::Driver_ptr getInstance();

protected:
  std::string GetFileName( SALOMEDS::SComponent_ptr theComponent, bool isMultiFile ) const;
};

#endif
