//  File   : SALOMEDS_AttributeStudyProperties.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributeStudyProperties.hxx"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_HSequenceOfExtendedString.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

SALOMEDS_AttributeStudyProperties
::SALOMEDS_AttributeStudyProperties(const Handle(SALOMEDSImpl_AttributeStudyProperties)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeStudyProperties::SALOMEDS_AttributeStudyProperties(SALOMEDS::AttributeStudyProperties_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeStudyProperties::~SALOMEDS_AttributeStudyProperties()
{
}
  
void SALOMEDS_AttributeStudyProperties::SetUserName(const char* theName)
{
  TCollection_AsciiString aName((char*)theName);
  if(_isLocal) Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->SetUserName(aName);
  else SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->SetUserName(aName.ToCString());
}

char* SALOMEDS_AttributeStudyProperties::GetUserName()
{
  TCollection_AsciiString aName;
  if(_isLocal) aName = Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->GetCreatorName();
  else aName = SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->GetUserName();
  return aName.ToCString();
}

void SALOMEDS_AttributeStudyProperties::SetCreationDate(int theMinute, int theHour, int theDay, int theMonth, int theYear)
{
  if(_isLocal) 
    Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->SetModificationDate(theMinute, 
											      theHour, 
											      theDay, 
											      theMonth, 
											      theYear);
  else 
    SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->SetCreationDate(theMinute, 
									      theHour, 
									      theDay, 
									      theMonth, 
									      theYear);
}

bool SALOMEDS_AttributeStudyProperties::GetCreationDate(int& theMinute, 
							int& theHour, 
							int& theDay, 
							int& theMonth, 
							int& theYear)
{
  bool ret;
  if(_isLocal) 
    ret = Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->GetCreationDate(theMinute, 
												theHour, 
												theDay, 
												theMonth, 
												theYear);
  else {
    CORBA::Long aMinute, anHour, aDay, aMonth, anYear;
    ret = SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->GetCreationDate(aMinute, 
										    anHour, 
										    aDay, 
										    aMonth, 
										    anYear);
    theMinute = (int)aMinute; theHour = (int)anHour; theDay = (int)aDay; theMonth = (int)aMonth; theYear = (int)anYear;
  }
  return ret;
}
 
void SALOMEDS_AttributeStudyProperties::SetCreationMode(const char* theMode)
{
  TCollection_AsciiString aMode((char*)theMode); 
  if(_isLocal) {
    if(aMode == "from scratch")
      Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->SetCreationMode(1);
    else if(aMode == "copy from")
      Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->SetCreationMode(2);
    else //Not defined
      Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->SetCreationMode(0);
  }
  else SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->SetCreationMode(aMode.ToCString());
}
 
char* SALOMEDS_AttributeStudyProperties::GetCreationMode()
{
  TCollection_AsciiString aMode;
  if(_isLocal) {
    int mode  = Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->GetCreationMode();
    if(mode == 1) aMode = "from scratch";
    if(mode == 2) aMode = "copy from";
  }
  else aMode = SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->GetCreationMode();
  return aMode.ToCString();  
}

void SALOMEDS_AttributeStudyProperties::SetModified(int theModified)
{
  if(_isLocal) Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->SetModified(theModified);
  else SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->SetModified(theModified);
}
 
bool SALOMEDS_AttributeStudyProperties::IsModified()
{
  bool ret;
  if(_isLocal) ret = Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->IsModified();
  else ret = SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->IsModified();
  return ret;
}

int SALOMEDS_AttributeStudyProperties::GetModified()
{
  int isModified;
  if(_isLocal) isModified = Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->GetModified();
  else isModified = SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->GetModified();
  return isModified;
}

void SALOMEDS_AttributeStudyProperties::SetLocked(bool theLocked)
{
  if(_isLocal) Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->SetLocked(theLocked);
  else SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->SetLocked(theLocked);
}
 
bool SALOMEDS_AttributeStudyProperties::IsLocked()
{
  bool ret;
  if(_isLocal) ret = Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->IsLocked();
  else ret = SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->IsLocked();
  return ret;
}
 
void SALOMEDS_AttributeStudyProperties::SetModification(const char* theName,
							int theMinute,
							int theHour,
							int theDay,
							int theMonth,
							int theYear)
{
  TCollection_AsciiString aName((char*)theName);
  if(_isLocal) {
    Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->SetUserName(aName);
    Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->SetModificationDate(theMinute, 
											      theHour, 
											      theDay, 
											      theMonth, 
											      theYear);
  }
  else SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->SetModification(aName.ToCString(), 
										 theMinute, 
										 theHour, 
										 theDay, 
										 theMonth, 
										 theYear);  
}

void SALOMEDS_AttributeStudyProperties::GetModificationsList(std::vector<std::string>& theNames,
							     std::vector<int>& theMinutes,
							     std::vector<int>& theHours,
							     std::vector<int>& theDays,
							     std::vector<int>& theMonths,
							     std::vector<int>& theYears,
							     bool theWithCreator)
{
 
  int i, aLength;
  
  if(_isLocal) {
    Handle(TColStd_HSequenceOfExtendedString) aNames;
    Handle(TColStd_HSequenceOfInteger) aMinutes, aHours, aDays, aMonths, aYears;
    aNames = Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->GetUserNames();
    Handle(SALOMEDSImpl_AttributeStudyProperties)::DownCast(_local_impl)->GetModificationDates(aMinutes, 
											       aHours, 
											       aDays, 
											       aMonths, 
											       aYears);
    aLength = aNames->Length()-((theWithCreator)?0:1);  
    for(i = 0; i < aLength; i++)
    {
      theNames.push_back(TCollection_AsciiString(aNames->Value(i + 1 + ((theWithCreator)?0:1))).ToCString());
      theMinutes.push_back(aMinutes->Value(i + 1 + ((theWithCreator)?0:1)));
      theHours.push_back(aHours->Value(i + 1 + ((theWithCreator)?0:1)));
      theDays.push_back(aDays->Value(i + 1 + ((theWithCreator)?0:1)));
      theMonths.push_back(aMonths->Value(i + 1 + ((theWithCreator)?0:1)));
      theYears.push_back(aYears->Value(i + 1 + ((theWithCreator)?0:1)));
    }  
  }
  else {
    SALOMEDS::StringSeq_var aNames;
    SALOMEDS::LongSeq_var aMinutes, aHours, aDays, aMonths, aYears;
    SALOMEDS::AttributeStudyProperties::_narrow(_corba_impl)->GetModificationsList(aNames.out(), 
										   aMinutes.out(), 
										   aHours.out(), 
										   aDays.out(), 
										   aMonths.out(), 
										   aYears.out(),
										   theWithCreator);
    aLength = aNames->length();
    for(i = 0; i<aLength; i++) {
      theNames.push_back(aNames[i].in());
      theMinutes.push_back(aMinutes[i]);
      theHours.push_back(aHours[i]);
      theDays.push_back(aDays[i]);
      theMonths.push_back(aMonths[i]);
      theYears.push_back(aYears[i]);
    }
  }
}

