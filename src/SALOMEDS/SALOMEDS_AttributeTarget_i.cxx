//  File   : SALOMEDS_AttributeTarget_i.cxx
//  Author : Sergey RUIN
//  Module : SALOME


using namespace std;
#include "SALOMEDS_AttributeTarget_i.hxx"
#include "SALOMEDSImpl_SObject.hxx"
#include "SALOMEDSImpl_Study.hxx"
#include "SALOMEDS.hxx"

#include <TDF_LabelList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>

void SALOMEDS_AttributeTarget_i::Add(SALOMEDS::SObject_ptr anObject) 
{
  SALOMEDS::Locker lock; 
  TDF_Label aLabel;
  TDF_Tool::Label(_impl->Label().Data(),anObject->GetID(),aLabel,1);
  (Handle(SALOMEDSImpl_AttributeTarget)::DownCast(_impl))->Append(aLabel);
}

SALOMEDS::Study::ListOfSObject* SALOMEDS_AttributeTarget_i::Get() 
{
  SALOMEDS::Locker lock; 
  TDF_LabelList aLList;
  SALOMEDS::Study::ListOfSObject_var aSList = new SALOMEDS::Study::ListOfSObject;
  (Handle(SALOMEDSImpl_AttributeTarget)::DownCast(_impl))->Get(aLList);
  if (aLList.Extent() == 0) 
    return aSList._retn();
  aSList->length(aLList.Extent());
  TDF_ListIteratorOfLabelList anIter(aLList);
  int index;
  for(index=0;anIter.More();anIter.Next(),index++) {
    SALOMEDS::SObject_var anSO = SALOMEDS_SObject_i::New(SALOMEDSImpl_Study::GetStudy(anIter.Value())->GetSObject(anIter.Value()), _orb);
    aSList[index] = anSO;
  }
  return aSList._retn();
}

void SALOMEDS_AttributeTarget_i::Remove(SALOMEDS::SObject_ptr anObject) 
{
  SALOMEDS::Locker lock; 
  TDF_Label aLabel;
  TDF_Tool::Label(_impl->Label().Data(),anObject->GetID(),aLabel,1);
  (Handle(SALOMEDSImpl_AttributeTarget)::DownCast(_impl))->Remove(aLabel);
}
