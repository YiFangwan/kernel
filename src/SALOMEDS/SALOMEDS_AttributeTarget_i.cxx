//  File   : SALOMEDS_AttributeTarget_i.cxx
//  Author : Sergey RUIN
//  Module : SALOME


using namespace std;
#include "SALOMEDS_AttributeTarget_i.hxx"
#include "SALOMEDSImpl_SObject.hxx"
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
    SALOMEDS_SObject_i* anSO = new SALOMEDS_SObject_i(new SALOMEDSImpl_SObject(anIter.Value()), _orb);
    aSList[index] = anSO->_this();
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
