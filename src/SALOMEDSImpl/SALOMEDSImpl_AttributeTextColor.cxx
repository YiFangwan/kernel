//  File   : SALOMEDSImpl_AttributeTextColor.cxx
//  Author : Sergey RUIN
//  Module : SALOME

using namespace std;
#include "SALOMEDSImpl_AttributeTextColor.hxx"
#include <Standard_GUID.hxx>

IMPLEMENT_STANDARD_HANDLE( SALOMEDSImpl_AttributeTextColor, SALOMEDSImpl_GenericAttribute )
IMPLEMENT_STANDARD_RTTIEXT( SALOMEDSImpl_AttributeTextColor, SALOMEDSImpl_GenericAttribute )

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& SALOMEDSImpl_AttributeTextColor::GetID () 
{
  static Standard_GUID SALOMEDSImpl_AttributeTextColorID ("12837189-8F52-11d6-A8A3-0001021E8C7F");
  return SALOMEDSImpl_AttributeTextColorID;
}


//=======================================================================
//function : constructor
//purpose  : 
//=======================================================================
SALOMEDSImpl_AttributeTextColor::SALOMEDSImpl_AttributeTextColor()
:SALOMEDSImpl_GenericAttribute("AttributeTextColor")
{
  myValue = new TColStd_HArray1OfReal(1, 3, RealFirst());
}

//=======================================================================
//function : SetTextColor
//purpose  :
//=======================================================================   
void SALOMEDSImpl_AttributeTextColor::SetTextColor(const Standard_Real R, const Standard_Real G, const Standard_Real B)
{
   CheckLocked();
   if(myValue.IsNull()) return;  
   Backup(); 

   myValue->SetValue(1, R);
   myValue->SetValue(2, G);
   myValue->SetValue(3, B);
}

//=======================================================================
//function : TextColor
//purpose  :
//=======================================================================
Handle(TColStd_HArray1OfReal) SALOMEDSImpl_AttributeTextColor::TextColor()
{
  return myValue; 
}

//=======================================================================
//function : ChangeArray
//purpose  : 
//=======================================================================
void SALOMEDSImpl_AttributeTextColor::ChangeArray(const Handle(TColStd_HArray1OfReal)& newArray)
{
  Backup();

  for(int i = 1; i <= 3; i++)
    myValue->SetValue(i, newArray->Value(i));
}    

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& SALOMEDSImpl_AttributeTextColor::ID () const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) SALOMEDSImpl_AttributeTextColor::NewEmpty () const
{  
  return new SALOMEDSImpl_AttributeTextColor(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void SALOMEDSImpl_AttributeTextColor::Restore(const Handle(TDF_Attribute)& with) 
{
  Handle(TColStd_HArray1OfReal) s = Handle(SALOMEDSImpl_AttributeTextColor)::DownCast (with)->TextColor ();
  ChangeArray(s);
  return;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void SALOMEDSImpl_AttributeTextColor::Paste (const Handle(TDF_Attribute)& into,
                                             const Handle(TDF_RelocationTable)& RT) const
{
  Handle(SALOMEDSImpl_AttributeTextColor)::DownCast (into)->ChangeArray (myValue);
}



TCollection_AsciiString SALOMEDSImpl_AttributeTextColor::Save() 
{
  char *Val = new char[75];
  sprintf(Val, "%f %f %f", (float)myValue->Value(1), 
                           (float)myValue->Value(2), 
			   (float)myValue->Value(3));
  TCollection_AsciiString ret(Val);
  delete Val;
  return ret;
}

void SALOMEDSImpl_AttributeTextColor::Load(const TCollection_AsciiString& value) 
{
  float r, g, b;
  sscanf(value.ToCString(), "%f %f %f", &r, &g, &b);
  myValue->SetValue(1, r);
  myValue->SetValue(2, g);
  myValue->SetValue(3, b);
}
