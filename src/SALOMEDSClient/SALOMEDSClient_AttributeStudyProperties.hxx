//  File   : SALOMEDSClient_AttributeStudyProperties.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDSClient_AttributeStudyProperties_HeaderFile
#define SALOMEDSClient_AttributeStudyProperties_HeaderFile

#include <vector>
#include <string>

#include "SALOMEDSClient_GenericAttribute.hxx"

class SALOMEDSClient_AttributeStudyProperties: public virtual SALOMEDSClient_GenericAttribute
{
public:
  
  virtual void SetUserName(const char* theName) = 0;
  virtual char* GetUserName() = 0;
  virtual void SetCreationDate(int theMinute, int theHour, int theDay, int theMonth, int theYear) = 0;
  virtual bool GetCreationDate(int& theMinute, int& theHour, int& theDay, int& theMonth, int& theYear) = 0;
  virtual void SetCreationMode(const char* theMode) = 0;
  virtual char* GetCreationMode() = 0;
  virtual void SetModified(int theModified) = 0;
  virtual bool IsModified() = 0;
  virtual int GetModified() = 0;
  virtual void SetLocked(bool theLocked) = 0;
  virtual bool IsLocked() = 0;
  virtual void SetModification(const char* theName,
			       int theMinute,
			       int theHour,
			       int theDay,
			       int theMonth,
			       int theYear) = 0;
  virtual void GetModificationsList(std::vector<std::string>& theNames,
				    std::vector<int>& theMinutes,
				    std::vector<int>& theHours,
				    std::vector<int>& theDays,
				    std::vector<int>& theMonths,
				    std::vector<int>& theYears,
				    bool theWithCreator) = 0;

};


#endif
