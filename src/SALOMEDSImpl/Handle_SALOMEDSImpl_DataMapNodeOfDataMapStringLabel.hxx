//  SALOME SALOMEDSImpl : data structure of SALOME and sources of Salome data server 
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
//  File   : Handle_SALOMEDSImpl_DataMapNodeOfDataMapStringLabel.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef _Handle_SALOMEDSImpl_DataMapNodeOfDataMapStringLabel_HeaderFile
#define _Handle_SALOMEDSImpl_DataMapNodeOfDataMapStringLabel_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_TCollection_MapNode_HeaderFile
#include <Handle_TCollection_MapNode.hxx>
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(TCollection_MapNode);
class SALOMEDSImpl_DataMapNodeOfDataMapStringLabel;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel);

class Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel) : public Handle(TCollection_MapNode) {
  public:
    inline void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    inline void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    inline void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }
//    inline void  operator delete(void *anAddress, size_t size) 
//      { 
//        if (anAddress) Standard::Free((Standard_Address&)anAddress,size); 
//      }
    Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel)():Handle(TCollection_MapNode)() {} 
    Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel)(const Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel)& aHandle) : Handle(TCollection_MapNode)(aHandle) 
     {
     }

    Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel)(const SALOMEDSImpl_DataMapNodeOfDataMapStringLabel* anItem) : Handle(TCollection_MapNode)((TCollection_MapNode *)anItem) 
     {
     }

    Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel)& operator=(const Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel)& operator=(const SALOMEDSImpl_DataMapNodeOfDataMapStringLabel* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    SALOMEDSImpl_DataMapNodeOfDataMapStringLabel* operator->() 
     {
      return (SALOMEDSImpl_DataMapNodeOfDataMapStringLabel *)ControlAccess();
     }

    SALOMEDSImpl_DataMapNodeOfDataMapStringLabel* operator->() const 
     {
      return (SALOMEDSImpl_DataMapNodeOfDataMapStringLabel *)ControlAccess();
     }

   Standard_EXPORT ~Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel)();
 
   Standard_EXPORT static const Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif
