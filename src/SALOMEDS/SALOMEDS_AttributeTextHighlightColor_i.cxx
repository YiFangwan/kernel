// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : SALOMEDS_AttributeTextHighlightColor_i.cxx
//  Author : Sergey RUIN
//  Module : SALOME


#include "SALOMEDS_AttributeTextHighlightColor_i.hxx"
#include "SALOMEDS.hxx"
#include <TColStd_HArray1OfReal.hxx>

using namespace std;

SALOMEDS::Color SALOMEDS_AttributeTextHighlightColor_i::TextHighlightColor() 
{
  SALOMEDS::Locker lock;
  SALOMEDS::Color TextHighlightColor;
  Handle(TColStd_HArray1OfReal) anArray=Handle(SALOMEDSImpl_AttributeTextHighlightColor)::DownCast(_impl)->TextHighlightColor();
  if (anArray.IsNull() || anArray->Length()!=3) { 
    TextHighlightColor.R = 0;
    TextHighlightColor.G = 0;
    TextHighlightColor.B = 0;
  }
  else {
    TextHighlightColor.R = anArray->Value(1);
    TextHighlightColor.G = anArray->Value(2);
    TextHighlightColor.B = anArray->Value(3);
  }
  return TextHighlightColor;
}

void SALOMEDS_AttributeTextHighlightColor_i::SetTextHighlightColor(const SALOMEDS::Color& value) 
{
  SALOMEDS::Locker lock;
  CheckLocked();
  Handle(TColStd_HArray1OfReal) anArray = new TColStd_HArray1OfReal(1,3);
  anArray->SetValue(1,  value.R);
  anArray->SetValue(2, value.G);
  anArray->SetValue(3, value.B);
  Handle(SALOMEDSImpl_AttributeTextHighlightColor)::DownCast(_impl)->ChangeArray(anArray);
}
