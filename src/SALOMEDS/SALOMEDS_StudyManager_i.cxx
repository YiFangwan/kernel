//  SALOME SALOMEDS : data structure of SALOME and sources of Salome data server 
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : SALOMEDS_StudyManager_i.cxx
//  Author : Yves FRICAUD 
//  Module : SALOME
//  $Header$

#include "utilities.h"
#include "SALOME_LifeCycleCORBA.hxx"
#include "SALOMEDS_StudyManager_i.hxx"
#include "SALOMEDS_Study_i.hxx"
#include "SALOMEDS_SComponent_i.hxx"

#include "SALOMEDS_IORAttribute.hxx"
#include "SALOMEDS_PersRefAttribute.hxx"
#include "SALOMEDS_Tool.hxx"

#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx> 
#include <TDF_Tool.hxx>
#include <TDF_Reference.hxx>
#include <TDF_Data.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_AttributeIterator.hxx>
//  #include <TDocStd_Owner.hxx>
#include <TColStd_HArray1OfCharacter.hxx>
#include <TCollection_ExtendedString.hxx>
#include "HDFexplorer.hxx"
#include "SALOMEDS_SequenceOfRealAttribute.hxx"
#include "SALOMEDS_SequenceOfIntegerAttribute.hxx"
#include <TColStd_HSequenceOfReal.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include "SALOMEDS_PixMapAttribute.hxx"
#include "SALOMEDS_DrawableAttribute.hxx"
#include "SALOMEDS_SelectableAttribute.hxx"
#include "SALOMEDS_ExpandableAttribute.hxx"
#include "SALOMEDS_OpenedAttribute.hxx"
#include "SALOMEDS_TextColorAttribute.hxx"
#include "SALOMEDS_TextHighlightColorAttribute.hxx"
#include "SALOMEDS_LocalIDAttribute.hxx"
#include "SALOMEDS_TargetAttribute.hxx"
#include "SALOMEDS_TableOfIntegerAttribute.hxx"
#include "SALOMEDS_TableOfRealAttribute.hxx"
#include "SALOMEDS_TableOfStringAttribute.hxx"
#include "SALOMEDS_StudyPropertiesAttribute.hxx"
#include "SALOMEDS_PythonObjectAttribute.hxx"
#include <OSD_Process.hxx>
#include <Quantity_Date.hxx>

#include "Utils_CorbaException.hxx"

#include <strstream>

#include "SALOME_GenericObj_i.hh"

#include "Utils_ExceptHandlers.hxx"
using namespace std;

UNEXPECT_CATCH(SalomeException,SALOME::SALOME_Exception);
UNEXPECT_CATCH(LockProtection, SALOMEDS::StudyBuilder::LockProtection);

#define USE_CASE_LABEL_ID             "0:2"
#define AUTO_SAVE_GUID                "128268A3-71C9-4036-89B1-F81BD6A4FCF2"
#define AUTO_SAVE_TAG                 "0:8"
#define AUTO_SAVE_TIME_OUT_IN_SECONDS 1200
//===========================================================================
//Function : LoadAttributes
//===========================================================================
static void ReadAttributes(SALOMEDS::Study_ptr theStudy,
			   SALOMEDS::SObject_ptr aSO,
			   HDFdataset* hdf_dataset)
{
  hdf_dataset->OpenOnDisk();

  SALOMEDS::GenericAttribute_var anAttr;

  char* current_string = new char[hdf_dataset->GetSize()];
  hdf_dataset->ReadFromDisk(current_string);

  if (!strcmp(hdf_dataset->GetName(),"COMPONENTDATATYPE")) {
    anAttr = theStudy->NewBuilder()->FindOrCreateAttribute(aSO, "AttributeComment");
  } else if (!strcmp(hdf_dataset->GetName(),"Reference")) {
    theStudy->NewBuilder()->Addreference(aSO, theStudy->CreateObjectID(current_string));
    delete(current_string);
    hdf_dataset->CloseOnDisk();
    return;
  } else {
    MESSAGE("Read attribute "<<hdf_dataset->GetName())
    anAttr = theStudy->NewBuilder()->FindOrCreateAttribute(aSO, hdf_dataset->GetName());
  }

  if (!CORBA::is_nil(anAttr)) {
    anAttr->Restore(current_string);
    MESSAGE("Restoring attribute "<<hdf_dataset->GetName()<<" by string '"<<current_string<<"' done")
  } else {
    MESSAGE(hdf_dataset->GetName());
    MESSAGE("LoadAttributes: unknown types");
  }
  delete(current_string);
  hdf_dataset->CloseOnDisk();
}

//============================================================================
//Function : Translate_IOR_to_persistentID
//============================================================================
static void Translate_IOR_to_persistentID (SALOMEDS::Study_ptr        study,
					   SALOMEDS::StudyBuilder_ptr SB,
					   SALOMEDS::SObject_ptr      so,
					   SALOMEDS::Driver_ptr       engine,
					   CORBA::Boolean             isMultiFile,
					   CORBA::Boolean             isASCII)
{
  MESSAGE("In Translate_IOR_to_persistentID");
  SALOMEDS::ChildIterator_var itchild = study->NewChildIterator(so);
  CORBA::String_var ior_string;
  char* persistent_string = 0;
  char *curid=0;

  for (; itchild->More(); itchild->Next()) {
    SALOMEDS::SObject_var current = itchild->Value();
    SCRUTE(current->GetID());
    SALOMEDS::GenericAttribute_var SObj;
    if (current->FindAttribute(SObj, "AttributeIOR")) {
      SALOMEDS::AttributeIOR_var IOR = SALOMEDS::AttributeIOR::_narrow(SObj);
      ior_string = IOR->Value();
      SCRUTE(ior_string);
      
      persistent_string = engine->IORToLocalPersistentID (current,ior_string,isMultiFile, isASCII);
      
//       SB->AddAttribute (current, SALOMEDS::PersistentRef,persistent_string);
      SALOMEDS::AttributePersistentRef_var Pers = SALOMEDS::AttributePersistentRef::_narrow(SB->FindOrCreateAttribute (current, "AttributePersistentRef"));
      Pers->SetValue(persistent_string);
      SCRUTE(persistent_string);
      curid = current->GetID();
      MESSAGE("Translate " << curid <<
	      " to Persistent string "<<persistent_string);
      persistent_string = 0;
      curid = 0;
    }
    Translate_IOR_to_persistentID (study,SB,current,engine,isMultiFile, isASCII);
  }
  CORBA::string_free(persistent_string);
  CORBA::string_free(curid);
}

//============================================================================
//Function : BuildlTree
//============================================================================
static void BuildTree (SALOMEDS::Study_ptr theStudy,HDFgroup* hdf_current_group)
{
  hdf_current_group->OpenOnDisk();
  
  SALOMEDS::SObject_var aSO;
  char* Entry = hdf_current_group->GetName();
  if (strcmp(Entry,"STUDY_STRUCTURE") == 0) {
    MESSAGE("find the root of the document");
    aSO = theStudy->CreateObjectID("0:1");
  }
  else {
    aSO = theStudy->CreateObjectID(Entry);
    MESSAGE("BuildTree : Create a new label"<<Entry);
  }
  char name[HDF_NAME_MAX_LEN+1];
  Standard_Integer nbsons = hdf_current_group->nInternalObjects(); 
  
  for (Standard_Integer i=0; i<nbsons; i++) {
    hdf_current_group->InternalObjectIndentify(i,name);
    if (strncmp(name, "INTERNAL_COMPLEX",16) == 0) continue;
    hdf_object_type type = hdf_current_group->InternalObjectType(name);

    if  (type == HDF_DATASET) {
      MESSAGE("--> Dataset: Internal Object Name : " << name);
      HDFdataset* new_dataset = new HDFdataset(name,hdf_current_group);
      ReadAttributes(theStudy,aSO,new_dataset);      
      new_dataset = 0; // will be deleted by father destructor

    }
    else if (type == HDF_GROUP)   {
      MESSAGE( "--> Group: Internal Object Name : " << name);
      HDFgroup* new_group = new HDFgroup(name,hdf_current_group);
      BuildTree (theStudy, new_group);
      new_group = 0; // will be deleted by father destructor
    }
  }
  hdf_current_group->CloseOnDisk();
}


//============================================================================
/*! Function : SALOMEDS_StudyManager_i
 *  Purpose  : SALOMEDS_StudyManager_i constructor 
 */
//============================================================================
SALOMEDS_StudyManager_i::SALOMEDS_StudyManager_i(CORBA::ORB_ptr orb) 
{ 
  _orb = CORBA::ORB::_duplicate(orb);
  _OCAFApp = new SALOMEDS_OCAFApplication();  
  _name_service = new SALOME_NamingService(_orb);
  // Study directory creation in the naming service : to register all
  // open studies in the session
  _name_service->Create_Directory("/Study");
  _IDcounter = 0;
}

//============================================================================
/*! Function : ~SALOMEDS_StudyManager_i
 *  Purpose  : SALOMEDS_StudyManager_i destructor
 */
//============================================================================
SALOMEDS_StudyManager_i::~SALOMEDS_StudyManager_i()
{
  // Destroy directory to register open studies
  _name_service->Destroy_Directory("/Study");
  // Destroy OCAF document
  delete &_OCAFApp;
}

//============================================================================
/*! Function : register_name
 *  Purpose  : Register the study Manager in the naming service under the  
 *             context name
 */
//============================================================================
void SALOMEDS_StudyManager_i::register_name(char * name) {
  SALOMEDS::StudyManager_ptr g = SALOMEDS::StudyManager::_narrow(_this());
  _name_service->Register(g, name);
}


//============================================================================
/*! Function : NewStudy
 *  Purpose  : Create a New Study of name study_name
 */
//============================================================================
SALOMEDS::Study_ptr SALOMEDS_StudyManager_i::NewStudy(const char* study_name) 
{
  Handle(TDocStd_Document) Doc;
  _OCAFApp->NewDocument("SALOME_STUDY",Doc); 

  MESSAGE("NewStudy : Creating the CORBA servant holding it... ");
  SALOMEDS_Study_i *Study_servant = new SALOMEDS_Study_i(Doc,_orb,study_name); 
  SALOMEDS::Study_var Study = SALOMEDS::Study::_narrow(Study_servant->_this());

  //Study->StudyId( _OCAFApp->NbDocuments() ); 
  _IDcounter++;
  Study->StudyId( _IDcounter );

  // Register study in the naming service
  // Path to acces the study
  if(!_name_service->Change_Directory("/Study")) 
      MESSAGE( "Unable to access the study directory" )
  else
      _name_service->Register(Study, study_name);

  // Assign the value of the IOR in the study->root
  const char*  IORStudy = _orb->object_to_string(Study);
  SALOMEDS_IORAttribute::Set(Doc->Main().Root(),TCollection_ExtendedString((char*)IORStudy),_orb);

  // set Study properties
  SALOMEDS::AttributeStudyProperties_ptr aProp = Study->GetProperties();
  OSD_Process aProcess;
  Quantity_Date aDate = aProcess.SystemDate();
  aProp->SetCreationDate(CORBA::Long(aDate.Minute()), CORBA::Long(aDate.Hour()), CORBA::Long(aDate.Day()),
			 CORBA::Long(aDate.Month()), CORBA::Long(aDate.Year()));
  aProp->SetCreationMode("from scratch");
  aProp->SetUserName(aProcess.UserName().ToCString());

  return Study;
}

//============================================================================
/*! Function : Open
 *  Purpose  : Open a Study from it's persistent reference
 */
//============================================================================
SALOMEDS::Study_ptr  SALOMEDS_StudyManager_i::Open(const char* aUrl)
     throw(SALOME::SALOME_Exception)
{
  Unexpect aCatch(SalomeException);
  MESSAGE("Begin of SALOMEDS_StudyManager_i::Open");
  // open the HDFFile 
  HDFfile *hdf_file =0;         
  HDFgroup *hdf_group_study_structure =0;

  char* aHDFUrl;
  bool isASCII = false;
  if (HDFascii::isASCII(aUrl)) {
    isASCII = true;
    char* aResultPath = HDFascii::ConvertFromASCIIToHDF(aUrl);
    aHDFUrl = new char[strlen(aResultPath) + 19];
    sprintf(aHDFUrl, "%shdf_from_ascii.hdf", aResultPath);
    delete(aResultPath);
  } else {
    aHDFUrl = CORBA::string_dup(aUrl);
  }

  hdf_file = new HDFfile(aHDFUrl);
  try {
    hdf_file->OpenOnDisk(HDF_RDONLY);// mpv: was RDWR, but opened file can be write-protected too
  }
  catch (HDFexception)
    {
//        MESSAGE( "HDFexception ! " );
//        cerr << "HDFexception ! " << endl;
      delete aHDFUrl;
      char eStr[strlen(aUrl)+17];
      sprintf(eStr,"Can't open file %s",aUrl);
      THROW_SALOME_CORBA_EXCEPTION(CORBA::string_dup(eStr),SALOME::BAD_PARAM);
      
    } 
  MESSAGE("Open : Creating the CORBA servant holding it... ");

  // Temporary aStudyUrl in place of study name
  Handle(TDocStd_Document) Doc;
  _OCAFApp->NewDocument("SALOME_STUDY",Doc); 

  SALOMEDS_Study_i * Study_servant = new SALOMEDS_Study_i(Doc, _orb, aUrl);  
  SALOMEDS::Study_var Study = SALOMEDS::Study::_narrow(Study_servant->_this()); 

  //  Study->StudyId( _OCAFApp->NbDocuments() ); 
  _IDcounter++;
  Study->StudyId( _IDcounter );

  // Assign the value of the URL in the study object
  Study->URL (aUrl);
  SCRUTE(aUrl);

  // Assign the value of the IOR in the study->root
  CORBA::String_var IORStudy = _orb->object_to_string(Study);
  SALOMEDS_IORAttribute::Set(Doc->Main().Root(),
			     TCollection_ExtendedString(CORBA::string_dup(IORStudy)),_orb);

  SALOMEDS_PersRefAttribute::Set(Doc->Main(),(char*)aUrl); 

  if (!hdf_file->ExistInternalObject("STUDY_STRUCTURE")) {
    delete aHDFUrl;
    MESSAGE("SALOMEDS_StudyManager::Open : the study is empty");
    return Study;
  }

  //Create  the Structure of the OCAF Document
  hdf_group_study_structure = new HDFgroup("STUDY_STRUCTURE",hdf_file);

  Handle(TDF_Data) DF = Doc->GetData();

  try {
    BuildTree (Study,hdf_group_study_structure);
  }
  catch (HDFexception)
    {
//        MESSAGE( "HDFexception ! " );
//        cerr << "HDFexception ! " << endl;
      delete aHDFUrl;
      char eStr[strlen(aUrl)+17];
      sprintf(eStr,"Can't open file %s",aUrl);
      THROW_SALOME_CORBA_EXCEPTION(CORBA::string_dup(eStr),SALOME::BAD_PARAM);
    } 
  
  hdf_file->CloseOnDisk();

  // Register study in the naming service
  // Path to acces the study
  if(!_name_service->Change_Directory("/Study")) MESSAGE( "Unable to access the study directory" )
  else _name_service->Register(Study, CORBA::string_dup(Study->Name()));


  if (isASCII) {
    SALOMEDS::ListOfFileNames_var aFilesToRemove = new SALOMEDS::ListOfFileNames;
    aFilesToRemove->length(1);
    aFilesToRemove[0] = CORBA::string_dup(&(aHDFUrl[strlen(SALOMEDS_Tool::GetDirFromPath(aHDFUrl).c_str())]));
    SALOMEDS_Tool::RemoveTemporaryFiles(SALOMEDS_Tool::GetDirFromPath(aHDFUrl).c_str(), aFilesToRemove, true);
  }
  delete aHDFUrl;
  delete hdf_file; // all related hdf objects will be deleted
  return Study;
}



//============================================================================
/*! Function : Close
 *  Purpose  : Close a study.
 *             If the study hasn't been saved, ask the user to confirm the
 *             close action without saving 
 */
//============================================================================
void  SALOMEDS_StudyManager_i::Close(SALOMEDS::Study_ptr aStudy)
{
  if(aStudy->_is_nil()) return;
  
  aStudy->RemovePostponed(-1);
  
  // Destroy study name in the naming service
  if(_name_service->Change_Directory("/Study")) 
    _name_service->Destroy_Name(aStudy->Name());
  
  aStudy->Close();
}

//============================================================================
/*! Function : Save
 *  Purpose  : Save a Study to it's persistent reference
 */
//============================================================================
void SALOMEDS_StudyManager_i::Save(SALOMEDS::Study_ptr aStudy, CORBA::Boolean theMultiFile)
{
  CORBA::String_var url = aStudy->URL();
  if (url==NULL)
    MESSAGE( "No path specified to save the study. Nothing done")
  else
    {
      _SaveAs(url,aStudy, theMultiFile, false);
    }
}

void SALOMEDS_StudyManager_i::SaveASCII(SALOMEDS::Study_ptr aStudy, CORBA::Boolean theMultiFile)
{
  CORBA::String_var url = aStudy->URL();
  if (url==NULL)
    MESSAGE( "No path specified to save the study. Nothing done")
  else
    {
      _SaveAs(url,aStudy, theMultiFile, true);
    }
}

//=============================================================================
/*! Function : SaveAs
 *  Purpose  : Save a study to the persistent reference aUrl
 */
//============================================================================
void SALOMEDS_StudyManager_i::SaveAs(const char* aUrl, SALOMEDS::Study_ptr aStudy, CORBA::Boolean theMultiFile)
{
  _SaveAs(aUrl,aStudy,theMultiFile, false);

}

void SALOMEDS_StudyManager_i::SaveAsASCII(const char* aUrl, SALOMEDS::Study_ptr aStudy, CORBA::Boolean theMultiFile)
{
  _SaveAs(aUrl,aStudy,theMultiFile, true);
}

//============================================================================
/*! Function : GetOpenStudies
 *  Purpose  : Get name list of open studies in the session
 */
//============================================================================
SALOMEDS::ListOfOpenStudies*  SALOMEDS_StudyManager_i::GetOpenStudies()
{
  // MESSAGE("Begin of GetOpenStudies");
  SALOMEDS::ListOfOpenStudies_var _list_open_studies = new SALOMEDS::ListOfOpenStudies;
  _list_open_studies->length(0);
  vector<string> _list ;

  if(!_name_service->Change_Directory("/Study"))
    {
      MESSAGE("No active study in this session");
    }
  else
    {
      _list = _name_service->list_directory();
      _list_open_studies->length(_list.size());
      for (unsigned int ind=0; ind < _list.size();ind++)
	{
	  _list_open_studies[ind]=CORBA::string_dup(_list[ind].c_str());
	  SCRUTE(_list_open_studies[ind]) ;
	}
    }
  return _list_open_studies._retn();
}

//============================================================================
/*! Function : GetStudyByName
 *  Purpose  : Get a study from its name
 */
//============================================================================
SALOMEDS::Study_ptr  
SALOMEDS_StudyManager_i::GetStudyByName(const char* aStudyName) 
{
  SALOMEDS::Study_var Study;

  // Go to study directory and look for aStudyName
  if(!_name_service->Change_Directory("/Study"))
    {
      MESSAGE("No active study in this session");
      ASSERT(false); // Stop here...
    }
  
//   const char *theStudyName = this->_SubstituteSlash(aStudyName);
  const char* theStudyName = CORBA::string_dup(aStudyName);

  if(_name_service->Find(theStudyName)>0)
    {
    // Study found
    CORBA::Object_ptr obj= _name_service->Resolve(theStudyName) ;
    Study = SALOMEDS::Study::_narrow(obj);
    MESSAGE("Study " << theStudyName << " found in the naming service");
    }
  else  
    {
      Study = SALOMEDS::Study::_narrow( CORBA::Object::_nil()); 
      MESSAGE("Study " << theStudyName << " not found in the naming service");
    }
  return Study;
}

//============================================================================
/*! Function : GetStudyByID
 *  Purpose  : Get a study from its ID
 */
//============================================================================
SALOMEDS::Study_ptr  
SALOMEDS_StudyManager_i::GetStudyByID(CORBA::Short aStudyID) 
{
  SALOMEDS::Study_var Study;
  vector<string> _list ;

  if(!_name_service->Change_Directory("/Study"))
    {
      MESSAGE("No active study in this session");
    }
  else
    {
      _list = _name_service->list_directory();
      for (unsigned int ind=0; ind < _list.size();ind++)
	{
	  const char* theStudyName = CORBA::string_dup(_list[ind].c_str());
	  MESSAGE ( "GetStudyByID = " << theStudyName )

	  if(_name_service->Find(theStudyName)>0) {
	    CORBA::Object_ptr obj= _name_service->Resolve(theStudyName) ;
	    Study = SALOMEDS::Study::_narrow(obj);

	    MESSAGE ( " aStudyID : " << aStudyID << "-" << Study->StudyId() )

	    if ( aStudyID == Study->StudyId() ) {
	      MESSAGE("Study with studyID = " << aStudyID << " found in the naming service");
	      return Study;
	    }
	  } else {
	    Study = SALOMEDS::Study::_narrow( CORBA::Object::_nil()); 
	    MESSAGE("Study " << theStudyName << " not found in the naming service");
	  }
	}
      Study = SALOMEDS::Study::_narrow( CORBA::Object::_nil()); 
    }
  return Study;
}
//============================================================================
/*! Function : SaveAttributes
 *  Purpose  : Save attributes for object
 */
//============================================================================
static void SaveAttributes(SALOMEDS::SObject_ptr SO, HDFgroup *hdf_group_sobject) {
  int a;
  hdf_size size[1];
  SALOMEDS::ListOfAttributes_var anAttrList = SO->GetAllAttributes();
  for(a = anAttrList->length() - 1; a >= 0; a--) {
    if (strcmp(anAttrList[a]->Type(), "AttributeIOR") == 0) continue; // never write AttributeIOR to file
    if (strcmp(anAttrList[a]->Type(), "AttributeExternalFileDef") == 0) continue; // never write ExternalFileDef to file
    if (strcmp(anAttrList[a]->Type(), "AttributeFileType") == 0) continue; // never write FileType to file
    CORBA::String_var aSaveStr = CORBA::string_dup(anAttrList[a]->Store());
    size[0] = (hdf_int32) strlen(aSaveStr) + 1;
    HDFdataset *hdf_dataset = new HDFdataset(anAttrList[a]->Type(),hdf_group_sobject,HDF_STRING,size,1);
    hdf_dataset->CreateOnDisk();
    hdf_dataset->WriteOnDisk(aSaveStr);
    hdf_dataset->CloseOnDisk();
    //MESSAGE("********** Write Attribute "<<anAttrList[a]->Type()<<" : "<<aSaveStr<<" done");
    hdf_dataset=0; //will be deleted by hdf_sco_group destructor
  }

  // Reference attribute has no CORBA attribute representation, so, GetAllAttributes can not return this attribute
  SALOMEDS::SObject_var RefSO;
  if(SO->ReferencedObject(RefSO)) {
    CORBA::String_var attribute_reference = CORBA::string_dup(RefSO->GetID());
    size[0] = strlen(attribute_reference) + 1 ; 
    HDFdataset *hdf_dataset = new HDFdataset("Reference",hdf_group_sobject,HDF_STRING,size,1);
    hdf_dataset->CreateOnDisk();
    hdf_dataset->WriteOnDisk(attribute_reference);
    hdf_dataset->CloseOnDisk();
    hdf_dataset =0; // will be deleted by father hdf object destructor
  }
}

//=============================================================================
/*! Function : _SaveProperties
 *  Purpose  : save the study properties in HDF file
 */
//============================================================================
void SALOMEDS_StudyManager_i::_SaveProperties(SALOMEDS::Study_ptr aStudy, HDFgroup *hdf_group) {
  HDFdataset *hdf_dataset = 0;
  hdf_size size[1];
  hdf_int32 name_len;

  // add modifications list (user and date of save)
  SALOMEDS::AttributeStudyProperties_ptr aProp = aStudy->GetProperties();
  SALOMEDS::StudyBuilder_var SB= aStudy->NewBuilder();
//    SB->NewCommand();
  int aLocked = aProp->IsLocked();
  if (aLocked) aProp->SetLocked(Standard_False);
  OSD_Process aProcess;
  Quantity_Date aDate = aProcess.SystemDate();
  aProp->SetModification(aProcess.UserName().ToCString(),
			 CORBA::Long(aDate.Minute()), CORBA::Long(aDate.Hour()), CORBA::Long(aDate.Day()),
			 CORBA::Long(aDate.Month()), CORBA::Long(aDate.Year()));
  if (aLocked) aProp->SetLocked(Standard_True);
//    SB->CommitCommand();
  

  SALOMEDS::StringSeq_var aNames;
  SALOMEDS::LongSeq_var aMinutes, aHours, aDays, aMonths, aYears;
  aProp->GetModificationsList(aNames , aMinutes ,aHours, aDays, aMonths, aYears, true);
  int aLength, anIndex;
  for(aLength = 0, anIndex = aNames->length() - 1; anIndex >= 0; anIndex--) aLength += strlen(aNames[anIndex]) + 1;

  // string length: 1 byte = locked flag, 1 byte = modified flag, (12 + name length + 1) for each name and date, "zero" byte
  char* aProperty = new char[3 + aLength + 12 * aNames->length()];

  sprintf(aProperty,"%c%c",
	  (strlen(aProp->GetCreationMode()) != 0)?aProp->GetCreationMode()[0]:'0',
	  (aProp->IsLocked())?'l':'u');

  aLength = aNames->length();
  int a = 2;
  for(anIndex = 0; anIndex  < aLength; anIndex++) {
    sprintf(&(aProperty[a]),"%2d%2d%2d%2d%4d%s",
	    (int)(aMinutes[anIndex]),
	    (int)(aHours[anIndex]),
	    (int)(aDays[anIndex]),
	    (int)(aMonths[anIndex]),
	    (int)(aYears[anIndex]),
	    (char*)aNames[anIndex]);
    a = strlen(aProperty);
    aProperty[a++] = 1;
  }
  aProperty[a] = 0;

  name_len = (hdf_int32) a;
//    MESSAGE("*** Property: "<<aProperty);
  size[0] = name_len + 1 ; 
  hdf_dataset = new HDFdataset("AttributeStudyProperties",hdf_group,HDF_STRING,size,1);
  hdf_dataset->CreateOnDisk();
  hdf_dataset->WriteOnDisk(aProperty);
  MESSAGE("attribute StudyProperties " <<  aProperty << " wrote on file");
  hdf_dataset->CloseOnDisk();
  hdf_dataset=0; //will be deleted by hdf_sco_group destructor
  //delete(aProperty); 
  delete [] aProperty;
  aProp->SetModified(0);
}

//=============================================================================
/*! Function : _SaveAs
 *  Purpose  : save the study in HDF file
 */
//============================================================================
void SALOMEDS_StudyManager_i::_SaveAs(const char* aUrl, 
				      SALOMEDS::Study_ptr aStudy,
				      CORBA::Boolean theMultiFile,
				      CORBA::Boolean theASCII)
{
  // HDF File will be composed of differents part :
  // * For each ComponentDataType, all data created by the component
  //   Informations in data group hdf_group_datacomponent
  // * Study Structure -> Exactly what is contained in OCAF document
  //   Informations in data group hdf_group_study_structure

  HDFfile *hdf_file=0;         
  HDFgroup *hdf_group_study_structure =0;
  HDFgroup *hdf_sco_group =0;
  HDFgroup *hdf_sco_group2 =0;

  HDFgroup *hdf_group_datacomponent =0;
  HDFdataset *hdf_dataset =0;
  HDFattribute *hdf_attribute=0;
  hdf_size size[1];
  hdf_int32 name_len = 0;
  char *component_name = 0;
  char *attribute_name = 0;
  char *attribute_comment = 0;
  char *attribute_persistentref = 0;

  int aLocked = aStudy->GetProperties()->IsLocked();
  if (aLocked) aStudy->GetProperties()->SetLocked(false);

  SALOMEDS::StudyBuilder_var SB= aStudy->NewBuilder();

  ASSERT(!CORBA::is_nil(aStudy));
  try
    {
      // mpv 15.12.2003: for saving components we have to load all data from all modules

      SALOMEDS::SComponentIterator_var itcomponent1 = aStudy->NewComponentIterator();
      for (; itcomponent1->More(); itcomponent1->Next())
	{
	  SALOMEDS::SComponent_var sco = itcomponent1->Value();

	  // if there is an associated Engine call its method for saving
	  CORBA::String_var IOREngine;
	  try {
	    if (!sco->ComponentIOR(IOREngine)) {
	      SALOMEDS::GenericAttribute_var aGeneric;
	      SALOMEDS::AttributeComment_var aName;
	      if(sco->FindAttribute(aGeneric, "AttributeComment"))
		aName = SALOMEDS::AttributeComment::_narrow(aGeneric);
	    
	      if (!aName->_is_nil()) {
		
		CORBA::String_var aCompType = aName->Value();

		CORBA::String_var aFactoryType;
		if (strcmp(aCompType, "SUPERV") == 0) aFactoryType = "SuperVisionContainer";
		else aFactoryType = "FactoryServer";
		
		Engines::Component_var aComp =
		  SALOME_LifeCycleCORBA(_name_service).FindOrLoad_Component(aFactoryType, aCompType);
		if (aComp->_is_nil()) {
		  Engines::Component_var aComp =
		    SALOME_LifeCycleCORBA(_name_service).FindOrLoad_Component("FactoryServerPy", aCompType);
		}
		
		if (!aComp->_is_nil()) {
		  SALOMEDS::Driver_var aDriver = SALOMEDS::Driver::_narrow(aComp);
		  if (!CORBA::is_nil(aDriver)) {
		    SB->LoadWith(sco, aDriver);
		  }
		}
	      }
	    }
	  } catch(...) {
	    MESSAGE("Can not restore information to resave it");
	    return;
	  }
	}



      CORBA::String_var anOldName = aStudy->Name();
      aStudy->URL(aUrl);

      // To change for Save 
      // Do not have to do a new file but just a Open??? Rewrite all informations after erasing evrything??
      hdf_file = new HDFfile((char *)aUrl);
      hdf_file->CreateOnDisk();
      MESSAGE("File " << aUrl << " created");

      //-----------------------------------------------------------------------
      // 1 - Create a groupe for each SComponent and Update the PersistanceRef
      //-----------------------------------------------------------------------
      hdf_group_datacomponent = new HDFgroup("DATACOMPONENT",hdf_file);
      hdf_group_datacomponent->CreateOnDisk();

      SALOMEDS::SComponentIterator_var itcomponent = aStudy->NewComponentIterator();
      
      //SRN: Added 17 Nov, 2003
      SALOMEDS::SObject_var anAutoSaveSO = aStudy->FindObjectID(AUTO_SAVE_TAG);
      //SRN: End

      for (; itcomponent->More(); itcomponent->Next())
	{
	  SALOMEDS::SComponent_var sco = itcomponent->Value();
	  
	  CORBA::String_var scoid = sco->GetID();
	  hdf_sco_group = new HDFgroup(scoid,hdf_group_datacomponent);
	  hdf_sco_group->CreateOnDisk();

	  CORBA::String_var componentDataType = sco->ComponentDataType();
	  MESSAGE ( "Look for  an engine for data type :"<< componentDataType);

	  //SRN: Added 17 Nov 2003: If there is a specified attribute, the component peforms a special save	  
	  if(!CORBA::is_nil(anAutoSaveSO) && SB->IsGUID(sco, AUTO_SAVE_GUID)) {	    
       
	    SALOMEDS::GenericAttribute_var aGeneric;
	    SALOMEDS::AttributeTableOfString_var aTable;
	    if(anAutoSaveSO->FindAttribute(aGeneric, "AttributeTableOfString")) {
	      aTable = SALOMEDS::AttributeTableOfString::_narrow(aGeneric);
	      Standard_Integer nbRows = aTable->GetNbRows(), k, aTimeOut = 0;
              if(nbRows > 0 && aTable->GetNbColumns() > 1) {	

		SALOMEDS::StringSeq_var aRow;
		for(k=1; k<=nbRows; k++) {
		  aRow = aTable->GetRow(k);
		  if (strcmp(aRow[0], componentDataType) == 0) {
		    CORBA::String_var anEntry = CORBA::string_dup(aRow[1]);
		    SALOMEDS::SObject_var aCompSpecificSO = aStudy->FindObjectID(anEntry);
		    if(!CORBA::is_nil(aCompSpecificSO)) {
		      SALOMEDS::AttributeInteger_var anInteger;
		      if(aCompSpecificSO->FindAttribute(aGeneric, "AttributeInteger")) {
			anInteger = SALOMEDS::AttributeInteger::_narrow(aGeneric);
			anInteger->SetValue(-1);
			while(anInteger->Value() < 0) { sleep(2); if(++aTimeOut > AUTO_SAVE_TIME_OUT_IN_SECONDS) break; }
		      }  // if(aCompSpecificSO->FindAttribute(anInteger, "AttributeInteger"))
		    }  // if(!CORBA::is_nil(aCompSpecificSO)) 
		  }  // if (strcmp(aRow[0], componentDataType) == 0)
		}  // for

	      }  // if(nbRows > 0 && aTable->GetNbColumns() > 1)

	    }  // if(anAutoSaveSO->FindAttribute(aTable, "AttributeTableOfString")

	  }  // if(SB->IsGUID(AUTO_SAVE_GUID)

	  //SRN: End

	  CORBA::String_var IOREngine;
	  if (sco->ComponentIOR(IOREngine))
	    {
	      // we have found the associated engine to write the data 
	      MESSAGE ( "We have found an engine for data type :"<< componentDataType);
	      CORBA::Object_var obj = _orb->string_to_object(IOREngine);
	      SALOMEDS::Driver_var Engine = SALOMEDS::Driver::_narrow(obj) ;
	      
	      if (!CORBA::is_nil(Engine))
		{
		  MESSAGE ( "Save the data of type:"<< componentDataType);
		  MESSAGE("Engine :"<<Engine->ComponentDataType());

		  SALOMEDS::TMPFile_var aStream;

                  if (theASCII) aStream = Engine->SaveASCII(sco,SALOMEDS_Tool::GetDirFromPath(aUrl).c_str(),theMultiFile);
		  else aStream = Engine->Save(sco,SALOMEDS_Tool::GetDirFromPath(aUrl).c_str(),theMultiFile);

		  HDFdataset *hdf_dataset;
		  hdf_size aHDFSize[1];
		  if(aStream->length() > 0) {  //The component saved some auxiliary files, then put them into HDF file 

		    aHDFSize[0] = aStream->length();
		      
		    HDFdataset *hdf_dataset = new HDFdataset("FILE_STREAM", hdf_sco_group, HDF_STRING, aHDFSize, 1);
		    hdf_dataset->CreateOnDisk();
		    hdf_dataset->WriteOnDisk((unsigned char*) &aStream[0]);  //Save the stream in the HDF file
		    hdf_dataset->CloseOnDisk();
		  }
		  // store multifile state
		  aHDFSize[0] = 2;
		  hdf_dataset = new HDFdataset("MULTIFILE_STATE", hdf_sco_group, HDF_STRING, aHDFSize, 1);
		  hdf_dataset->CreateOnDisk();
		  hdf_dataset->WriteOnDisk((void*)(theMultiFile?"M":"S")); // save: multi or single
		  hdf_dataset->CloseOnDisk();
		  hdf_dataset=0; //will be deleted by hdf_sco_AuxFiles destructor		 

		  // store ASCII state
		  aHDFSize[0] = 2;
		  hdf_dataset = new HDFdataset("ASCII_STATE", hdf_sco_group, HDF_STRING, aHDFSize, 1);
		  hdf_dataset->CreateOnDisk();
		  hdf_dataset->WriteOnDisk((void*)(theASCII?"A":"B")); // save: ASCII or BINARY
		  hdf_dataset->CloseOnDisk();
		  hdf_dataset=0; //will be deleted by hdf_sco_AuxFiles destructor		 

		  Translate_IOR_to_persistentID (aStudy,SB,sco,Engine,theMultiFile, theASCII);
		  MESSAGE("After Translate_IOR_to_persistentID");
		  
		  // Creation of the persistance reference  attribute
		}
	    }
	  hdf_sco_group->CloseOnDisk();
	  hdf_sco_group=0; // will be deleted by hdf_group_datacomponent destructor
	}
      hdf_group_datacomponent->CloseOnDisk();
      hdf_group_datacomponent =0;  // will be deleted by hdf_file destructor


      //-----------------------------------------------------------------------
      //3 - Write the Study Structure
      //-----------------------------------------------------------------------
      hdf_group_study_structure = new HDFgroup("STUDY_STRUCTURE",hdf_file);
      hdf_group_study_structure->CreateOnDisk();

      // save component attributes
      SALOMEDS::SComponentIterator_var itcomp = aStudy->NewComponentIterator();
      for (; itcomp->More(); itcomp->Next()) 
	{
	  SALOMEDS::SComponent_var SC = itcomp->Value();
	  
	  CORBA::String_var scid = SC->GetID();
	  hdf_sco_group2 = new HDFgroup(scid,hdf_group_study_structure);
	  hdf_sco_group2->CreateOnDisk();
          SaveAttributes(SC, hdf_sco_group2);
	  // ComponentDataType treatment
	  component_name = SC->ComponentDataType();
	  MESSAGE("Component data type " << component_name << " treated");
	  
	  name_len = (hdf_int32) strlen(component_name);
	  size[0] = name_len +1 ; 
	  hdf_dataset = new HDFdataset("COMPONENTDATATYPE",hdf_sco_group2,HDF_STRING,size,1);
	  hdf_dataset->CreateOnDisk();
	  hdf_dataset->WriteOnDisk(component_name);
	  MESSAGE("component name " <<  component_name << " wrote on file");
	  hdf_dataset->CloseOnDisk();
	  hdf_dataset=0; //will be deleted by hdf_sco_group destructor
	  _SaveObject(aStudy, SC, hdf_sco_group2);
	  hdf_sco_group2->CloseOnDisk();
 	  hdf_sco_group2=0; // will be deleted by hdf_group_study_structure destructor
	  CORBA::string_free(component_name);	    
	}
      //-----------------------------------------------------------------------
      //4 - Write the Study UseCases Structure
      //-----------------------------------------------------------------------
      SALOMEDS::SObject_var aSO = aStudy->FindObjectID(USE_CASE_LABEL_ID);
      if (!aSO->_is_nil()) {
	HDFgroup *hdf_soo_group = new HDFgroup(USE_CASE_LABEL_ID,hdf_group_study_structure);
	hdf_soo_group->CreateOnDisk();
	SaveAttributes(aSO, hdf_soo_group);
	_SaveObject(aStudy, aSO, hdf_soo_group);
	MESSAGE("Use cases data structure writed");
	hdf_soo_group->CloseOnDisk();
	hdf_soo_group=0; // will be deleted by hdf_group_study_structure destructor
      }

      if (aLocked) aStudy->GetProperties()->SetLocked(true);
      //-----------------------------------------------------------------------
      //5 - Write the Study Properties
      //-----------------------------------------------------------------------
      name_len = (hdf_int32) strlen(aStudy->Name());
      size[0] = name_len +1 ; 
      hdf_dataset = new HDFdataset("STUDY_NAME",hdf_group_study_structure,HDF_STRING,size,1);
      hdf_dataset->CreateOnDisk();
      CORBA::String_var studid = aStudy->Name();
      hdf_dataset->WriteOnDisk(studid);
      MESSAGE("study name " << studid << " wrote on file");
      hdf_dataset->CloseOnDisk();
      hdf_dataset=0; // will be deleted by hdf_group_study_structure destructor

      _SaveProperties(aStudy, hdf_group_study_structure);

      hdf_group_study_structure->CloseOnDisk();
      hdf_file->CloseOnDisk();

      _name_service->Change_Directory("/Study");
      _name_service->Destroy_Name(anOldName);
      _name_service->Register(aStudy, aStudy->Name());

      aStudy->IsSaved(true);
      hdf_group_study_structure =0; // will be deleted by hdf_file destructor
      delete hdf_file; // recursively deletes all hdf objects...
    }
  catch (HDFexception)
    {
      MESSAGE( "HDFexception ! " )
    }
  if (theASCII) { // save file in ASCII format
    HDFascii::ConvertFromHDFToASCII(aUrl, true);
  }
}

//============================================================================
/*! Function : _SaveObject
 *  Purpose  :
 */
//============================================================================
void SALOMEDS_StudyManager_i::_SaveObject(SALOMEDS::Study_ptr aStudy, 
					  SALOMEDS::SObject_ptr SC, 
					  HDFgroup *hdf_group_datatype)
{
  // Write in group hdf_group_datatype all informations of SObject SC
  // Iterative function to parse all SObjects under a SComponent
  SALOMEDS::SObject_var RefSO;
  HDFgroup *hdf_group_sobject = 0;
  HDFdataset *hdf_dataset = 0;
  hdf_size size[1];
  hdf_int32 name_len = 0;

  SALOMEDS::ChildIterator_var itchild = aStudy->NewChildIterator(SC);
  for (; itchild->More(); itchild->Next()) 
    {
      SALOMEDS::SObject_var SO = itchild->Value();

      // mpv: don't save empty labels
      if (SO->GetAllAttributes()->length() == 0 && !SO->ReferencedObject(RefSO)) {
	SALOMEDS::ChildIterator_var subchild = aStudy->NewChildIterator(SC);
	if (!subchild->More()) {
	  continue;
	}
	subchild->InitEx(true);
	bool anEmpty = true;
	for (; subchild->More() && anEmpty; subchild->Next()) 
	  if (subchild->Value()->GetAllAttributes()->length() != 0 ||
              subchild->Value()->ReferencedObject(RefSO)) anEmpty = false;
	if (anEmpty) {
	  continue;
	}
      }

      CORBA::String_var scoid = CORBA::string_dup(SO->GetID());
      hdf_group_sobject = new HDFgroup(scoid,hdf_group_datatype);
      hdf_group_sobject->CreateOnDisk();
      SaveAttributes(SO, hdf_group_sobject);
      _SaveObject(aStudy,SO, hdf_group_sobject);
      hdf_group_sobject->CloseOnDisk();
      hdf_group_sobject =0; // will be deleted by father hdf object destructor

    }
}

//============================================================================
/*! Function : _SubstituteSlash
 *  Purpose  :
 */
//============================================================================

const char *SALOMEDS_StudyManager_i::_SubstituteSlash(const char *aUrl)
{
  ASSERT(1==0);
  TCollection_ExtendedString theUrl(CORBA::string_dup(aUrl));
  Standard_ExtCharacter val1 = ToExtCharacter('/');
  Standard_ExtCharacter val2 = ToExtCharacter(':');
  theUrl.ChangeAll(val1,val2);
  TCollection_AsciiString ch(theUrl);
  return CORBA::string_dup(ch.ToCString());
}

//============================================================================
/*! Function : CanCopy
 *  Purpose  : 
 */
//============================================================================
CORBA::Boolean SALOMEDS_StudyManager_i::CanCopy(SALOMEDS::SObject_ptr theObject) {
  SALOMEDS::SComponent_var aComponent = theObject->GetFatherComponent();
  if (aComponent->_is_nil()) return false;
  if (aComponent == theObject) return false;

  CORBA::String_var IOREngine;
  if (!aComponent->ComponentIOR(IOREngine)) return false;

  CORBA::Object_var obj = _orb->string_to_object(IOREngine);
  SALOMEDS::Driver_var Engine = SALOMEDS::Driver::_narrow(obj) ;
  if (CORBA::is_nil(Engine)) return false;
  Standard_Boolean a = Engine->CanCopy(theObject);
  return a;
}

//============================================================================
/*! Function : GetDocumentOfStudy
 *  Purpose  : 
 */
//============================================================================
Handle(TDocStd_Document) SALOMEDS_StudyManager_i::GetDocumentOfStudy(SALOMEDS::Study_ptr theStudy) {
  int a;
  int aNbDocs = _OCAFApp->NbDocuments(); 
  Handle(TDocStd_Document) aDocument;  
  for(a = 1; a <= aNbDocs ; a++) {
    _OCAFApp->GetDocument(a, aDocument);
    if (!aDocument.IsNull()) {
      SALOMEDS_SObject_i *  aSOServant = new SALOMEDS_SObject_i (aDocument->Main(),_orb);
      SALOMEDS::SObject_var aSO = SALOMEDS::SObject::_narrow(aSOServant->_this()); 
      SALOMEDS::Study_var aStudy = aSO->GetStudy();
      if(CORBA::is_nil(aStudy)) continue;  //The clipboard document ( hopefully :) )
      if (aStudy->StudyId() == theStudy->StudyId()) break;
      aDocument.Nullify();
    }
  }

  return aDocument;
}

//============================================================================
/*! Function : CopyLabel
 *  Purpose  : 
 */
//============================================================================
void SALOMEDS_StudyManager_i::CopyLabel(const SALOMEDS::Study_ptr theSourceStudy,
					const SALOMEDS::Driver_ptr theEngine,
					const Standard_Integer theSourceStartDepth,
					const TDF_Label& theSource,
					const TDF_Label& theDestinationMain) {
  int a;
  TDF_Label aTargetLabel = theDestinationMain;
  TDF_Label aAuxTargetLabel = theDestinationMain.Father().FindChild(2);
  for(a = theSource.Depth() - theSourceStartDepth; a > 0 ; a--) {
    TDF_Label aSourceLabel = theSource;
    for(int aNbFather = 1; aNbFather < a; aNbFather++) aSourceLabel = aSourceLabel.Father();
    aTargetLabel = aTargetLabel.FindChild(aSourceLabel.Tag());
    aAuxTargetLabel = aAuxTargetLabel.FindChild(aSourceLabel.Tag());
  }
  // iterate attributes
  TDF_AttributeIterator anAttrIterator(theSource);
  Handle(TDF_RelocationTable) aRT = new TDF_RelocationTable();
  for(; anAttrIterator.More(); anAttrIterator.Next()) {
    Handle(TDF_Attribute) anAttr = anAttrIterator.Value();
    if (!Handle(TDataStd_TreeNode)::DownCast(anAttr).IsNull()) continue; // never copy tree node attribute
    if (!Handle(SALOMEDS_TargetAttribute)::DownCast(anAttr).IsNull()) continue; // and target attribute
    
    if (!Handle(TDF_Reference)::DownCast(anAttr).IsNull()) { // reference copied as Comment in auxiliary tree
      TDF_Label aReferenced = Handle(TDF_Reference)::DownCast(anAttr)->Get();
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(aReferenced, anEntry);
      // store the value of name attribute of referenced label
      Handle(TDataStd_Name) aNameAttribute;
      if (aReferenced.FindAttribute(TDataStd_Name::GetID(), aNameAttribute)) {
	anEntry += " ";
	anEntry += aNameAttribute->Get();
      }
      TDataStd_Comment::Set(aAuxTargetLabel, TCollection_ExtendedString(anEntry));
      continue;
    }
    
    if (!Handle(SALOMEDS_IORAttribute)::DownCast(anAttr).IsNull()) { // IOR => ID and TMPFile of Engine
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(theSource, anEntry);
      SALOMEDS::SObject_var aSO = theSourceStudy->FindObjectID(anEntry.ToCString());
//        if (theEngine->CanCopy(aSO)) {
	CORBA::Long anObjID;
//  	TCollection_ExtendedString aResStr(strdup((char*)(theEngine->CopyFrom(aSO, anObjID))));
          SALOMEDS::TMPFile_var aStream = theEngine->CopyFrom(aSO, anObjID);
          int aLen = aStream->length();
	  TCollection_ExtendedString aResStr("");
	  for(a = 0; a < aLen; a++) {
	    aResStr += TCollection_ExtendedString(ToExtCharacter((Standard_Character)aStream[a]));
	  }
	  TDataStd_Integer::Set(aAuxTargetLabel, anObjID);
	  TDataStd_Name::Set(aAuxTargetLabel, aResStr);
//        }
      continue;
    }
    Handle(TDF_Attribute) aNewAttribute = anAttr->NewEmpty();
    aTargetLabel.AddAttribute(aNewAttribute);
    anAttr->Paste(aNewAttribute, aRT);
//      aRT->SetRelocation(anAttr, aNewAttribute);
  }
}

//============================================================================
/*! Function : Copy
 *  Purpose  :
 */
//============================================================================
CORBA::Boolean SALOMEDS_StudyManager_i::Copy(SALOMEDS::SObject_ptr theObject) {
  // adoptation for alliances datamodel copy: without IOR attributes !!!
  bool aStructureOnly; // copy only SObjects and attributes without component help
  SALOMEDS::GenericAttribute_var anAttribute;
  aStructureOnly = !theObject->FindAttribute(anAttribute, "AttributeIOR");

  // get component-engine
  SALOMEDS::Study_var aStudy = theObject->GetStudy();

  SALOMEDS::Driver_var Engine;
  if (!aStructureOnly) {
    SALOMEDS::SComponent_var aComponent = theObject->GetFatherComponent();
    CORBA::String_var IOREngine;
    if (!aComponent->ComponentIOR(IOREngine)) return false;

    CORBA::Object_var obj = _orb->string_to_object(IOREngine);
    Engine = SALOMEDS::Driver::_narrow(obj) ;
  }
  // CAF document of current study usage
  Handle(TDocStd_Document) aDocument = GetDocumentOfStudy(aStudy);
  if (aDocument.IsNull()) return false;
  // create new document for clipboard
  Handle(TDocStd_Document) aTargetDocument;
  _OCAFApp->NewDocument("SALOME_STUDY", aTargetDocument);
  // set component data type to the name attribute of root label
  if (!aStructureOnly) {
    TDataStd_Comment::Set(aTargetDocument->Main().Root(),
			  TCollection_ExtendedString(Engine->ComponentDataType()));
  }
  // set to the Root label integer attribute: study id
  TDataStd_Integer::Set(aTargetDocument->Main().Root(), aStudy->StudyId());
  // iterate all theObject's label children
  TDF_Label aStartLabel;
  char* aStartID = CORBA::string_dup(theObject->GetID());
  TDF_Tool::Label(aDocument->GetData(), aStartID, aStartLabel);
  delete(aStartID);
  Standard_Integer aSourceStartDepth = aStartLabel.Depth();
  
  // copy main source label
  CopyLabel(aStudy, Engine, aSourceStartDepth, aStartLabel, aTargetDocument->Main());

  // copy all subchildren of the main source label (all levels)
  TDF_ChildIterator anIterator(aStartLabel, Standard_True);
  for(; anIterator.More(); anIterator.Next()) {
    CopyLabel(aStudy, Engine, aSourceStartDepth, anIterator.Value(), aTargetDocument->Main());
  }
  // done: free old clipboard document and 
  if (!_clipboard.IsNull()) {
//      Handle(TDocStd_Owner) anOwner;
//      if (_clipboard->Main().Root().FindAttribute(TDocStd_Owner::GetID(), anOwner)) {
//        Handle(TDocStd_Document) anEmptyDoc;
//        anOwner->SetDocument(anEmptyDoc);
//      }
    _OCAFApp->Close(_clipboard);
  }
  _clipboard = aTargetDocument;

  return true;
}
//============================================================================
/*! Function : CanPaste
 *  Purpose  :
 */
//============================================================================
CORBA::Boolean SALOMEDS_StudyManager_i::CanPaste(SALOMEDS::SObject_ptr theObject) {
  if (_clipboard.IsNull()) return false;

  Handle(TDataStd_Comment) aCompName;
  if (!_clipboard->Main().Root().FindAttribute(TDataStd_Comment::GetID(), aCompName)) return false;
  Handle(TDataStd_Integer) anObjID;
  if (!_clipboard->Main().Father().FindChild(2).FindAttribute(TDataStd_Integer::GetID(), anObjID))
    return false;

  SALOMEDS::SComponent_var aComponent = theObject->GetFatherComponent();
  if (aComponent->_is_nil()) return false;
  
  CORBA::String_var IOREngine;
  if (!aComponent->ComponentIOR(IOREngine)) return false;
  
  CORBA::Object_var obj = _orb->string_to_object(IOREngine);
  SALOMEDS::Driver_var Engine = SALOMEDS::Driver::_narrow(obj) ;
  if (CORBA::is_nil(Engine)) return false;
  return Engine->CanPaste(TCollection_AsciiString(aCompName->Get()).ToCString(), anObjID->Get());
}
//============================================================================
/*! Function : PasteLabel
 *  Purpose  :
 */
//============================================================================
TDF_Label SALOMEDS_StudyManager_i::PasteLabel(const SALOMEDS::Study_ptr theDestinationStudy,
					      const SALOMEDS::Driver_ptr theEngine,
					      const TDF_Label& theSource,
					      const TDF_Label& theDestinationStart,
					      const int theCopiedStudyID,
					      const bool isFirstElement) {

  // get corresponding source, target and auxiliary labels
  TDF_Label aTargetLabel = theDestinationStart;
  TDF_Label aAuxSourceLabel = theSource.Root().FindChild(2);
  int a;
  if (!isFirstElement) {
    for(a = theSource.Depth() - 1; a > 0 ; a--) {
      TDF_Label aSourceLabel = theSource;
      for(int aNbFather = 1; aNbFather < a; aNbFather++) aSourceLabel = aSourceLabel.Father();
      aTargetLabel = aTargetLabel.FindChild(aSourceLabel.Tag());
      aAuxSourceLabel = aAuxSourceLabel.FindChild(aSourceLabel.Tag());
    }
  }

  // check auxiliary label for TMPFile => IOR
  Handle(TDataStd_Name) aNameAttribute;
  if (aAuxSourceLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttribute)) {
    Handle(TDataStd_Integer) anObjID;

    aAuxSourceLabel.FindAttribute(TDataStd_Integer::GetID(), anObjID);
    Handle(TDataStd_Comment) aComponentName;
    theSource.Root().FindAttribute(TDataStd_Comment::GetID(), aComponentName);
    CORBA::String_var aCompName = CORBA::string_dup(TCollection_AsciiString(aComponentName->Get()).ToCString());

    if (theEngine->CanPaste(aCompName, anObjID->Get())) {
      SALOMEDS::TMPFile_var aTMPFil = new SALOMEDS::TMPFile();
      TCollection_ExtendedString aTMPStr = aNameAttribute->Get();
      int aLen = aTMPStr.Length();
      aTMPFil->length(aLen);
      for(a = 0; a < aLen; a++) {
	aTMPFil[a] = ToCharacter(aTMPStr.Value(a+1));
      }
//        char* aTMPStr = strdup(TCollection_AsciiString(aNameAttribute->Get()).ToCString());
//        int aLen = strlen(aTMPStr);
//        SALOMEDS::TMPFile aTMPFil(aLen, aLen, (CORBA::Octet*)aTMPStr, 1);
      
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(aTargetLabel, anEntry);
      SALOMEDS::SObject_var aPastedSO = theDestinationStudy->FindObjectID(anEntry.ToCString());
      if (isFirstElement) {
	SALOMEDS::SObject_var aDestSO =
	  theEngine->PasteInto(aTMPFil.in(),
			       anObjID->Get(),
			       aPastedSO->GetFatherComponent());
	TDF_Tool::Label(theDestinationStart.Data(), aDestSO->GetID(), aTargetLabel);
      } else theEngine->PasteInto(aTMPFil.in(),anObjID->Get(),aPastedSO);
    }
  }

  // iterate attributes
  TDF_AttributeIterator anAttrIterator(theSource);
  Handle(TDF_RelocationTable) aRT = new TDF_RelocationTable();
  for(; anAttrIterator.More(); anAttrIterator.Next()) {
    Handle(TDF_Attribute) anAttr = anAttrIterator.Value();
    if (aTargetLabel.FindAttribute(anAttr->ID(), anAttr)) {
      aTargetLabel.ForgetAttribute(anAttr->ID());
      anAttr = anAttrIterator.Value();
    }
    Handle(TDF_Attribute) aNewAttribute = anAttr->NewEmpty();
    aTargetLabel.AddAttribute(aNewAttribute);
    anAttr->Paste(aNewAttribute, aRT);
//      aRT->SetRelocation(anAttr, aNewAttribute);
  }
  // check auxiliary label for Comment => reference or name attribute of the referenced object
  Handle(TDataStd_Comment) aCommentAttribute;
  if (aAuxSourceLabel.FindAttribute(TDataStd_Comment::GetID(), aCommentAttribute)) {
    char * anEntry = new char[aCommentAttribute->Get().Length() + 1];
    strcpy(anEntry, TCollection_AsciiString(aCommentAttribute->Get()).ToCString());
    char* aNameStart = strchr(anEntry, ' ');
    if (aNameStart) {
      *aNameStart = '\0';
      aNameStart++;
    }
    if (theCopiedStudyID == theDestinationStudy->StudyId()) { // if copy to the same study, reanimate reference
      TDF_Label aRefLabel;
      TDF_Tool::Label(aTargetLabel.Data(), anEntry, aRefLabel);
      TDF_Reference::Set(aTargetLabel, aRefLabel);
      SALOMEDS_TargetAttribute::Set(aRefLabel)->Append(aTargetLabel); // target attributes structure support
    } else {
      if (aNameStart) TDataStd_Name::Set(aTargetLabel, aNameStart);
      else TDataStd_Name::Set(aTargetLabel, TCollection_ExtendedString("Reference to:")+anEntry);
    }
    delete [] anEntry;
  }

  return aTargetLabel;
}
//============================================================================
/*! Function : Paste
 *  Purpose  :
 */
//============================================================================
SALOMEDS::SObject_ptr SALOMEDS_StudyManager_i::Paste(SALOMEDS::SObject_ptr theObject)
     throw(SALOMEDS::StudyBuilder::LockProtection)
{
  Unexpect aCatch(LockProtection);
  SALOMEDS::Study_var aStudy = theObject->GetStudy();

  // if study is locked, then paste can't be done
  if (aStudy->GetProperties()->IsLocked())
    throw SALOMEDS::StudyBuilder::LockProtection();

  // if there is no component name, then paste only SObjects and attributes: without component help
  Handle(TDataStd_Comment) aComponentName;
  bool aStructureOnly = !_clipboard->Main().Root().FindAttribute(TDataStd_Comment::GetID(), aComponentName);

  // get copied study ID
  Handle(TDataStd_Integer) aStudyIDAttribute;
  if (!_clipboard->Main().Root().FindAttribute(TDataStd_Integer::GetID(), aStudyIDAttribute))
    return SALOMEDS::SObject::_nil();
  int aCStudyID = aStudyIDAttribute->Get();

  // get component-engine
  SALOMEDS::Driver_var Engine;
  SALOMEDS::SComponent_var aComponent;
  if (!aStructureOnly) {
    aComponent = theObject->GetFatherComponent();
    CORBA::String_var IOREngine;
    if (!aComponent->ComponentIOR(IOREngine)) return SALOMEDS::SObject::_nil();
    CORBA::Object_var obj = _orb->string_to_object(IOREngine);
    Engine = SALOMEDS::Driver::_narrow(obj) ;
  }

  // CAF document of current study usage
  Handle(TDocStd_Document) aDocument = GetDocumentOfStudy(aStudy);
  if (aDocument.IsNull()) return SALOMEDS::SObject::_nil();
  // fill root inserted SObject
  TDF_Label aStartLabel;
  if (aStructureOnly) {
    TDF_Label anObjectLabel;
    TDF_Tool::Label(aDocument->GetData(), theObject->GetID(), anObjectLabel);
    aStartLabel = PasteLabel(aStudy, Engine, _clipboard->Main(), anObjectLabel, aCStudyID, false);
  } else {
    TDF_Label aComponentLabel;
    TDF_Tool::Label(aDocument->GetData(), aComponent->GetID(), aComponentLabel);
    aStartLabel = PasteLabel(aStudy, Engine, _clipboard->Main(), aComponentLabel, aCStudyID, true);
  }

  // paste all sublebels
  TDF_ChildIterator anIterator(_clipboard->Main(), Standard_True);
  for(; anIterator.More(); anIterator.Next()) {
    PasteLabel(aStudy, Engine, anIterator.Value(), aStartLabel, aCStudyID, false);
  }

  SALOMEDS_SObject_i *  so_servant = new SALOMEDS_SObject_i (aStartLabel, _orb);
  SALOMEDS::SObject_var so = SALOMEDS::SObject::_narrow(so_servant->_this()); 

  return so._retn();
}
