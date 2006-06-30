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
//  File   : SALOMEDS_Attributes.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDS_ATTRIBUTES__H__
#define __SALOMEDS_ATTRIBUTES__H__

#include "SALOMEDSImpl_GenericAttribute.hxx"
#include "SALOMEDSImpl_AttributeName.hxx"
#include "SALOMEDSImpl_AttributeComment.hxx"
#include "SALOMEDSImpl_AttributeInteger.hxx"
#include "SALOMEDSImpl_AttributeReal.hxx"
#include "SALOMEDSImpl_AttributeTreeNode.hxx"
#include "SALOMEDSImpl_AttributeUserID.hxx"
#include "SALOMEDSImpl_AttributeReference.hxx"
#include "SALOMEDSImpl_AttributeSequenceOfReal.hxx"
#include "SALOMEDSImpl_AttributeSequenceOfInteger.hxx" 
#include "SALOMEDSImpl_AttributeIOR.hxx"
#include "SALOMEDSImpl_AttributePersistentRef.hxx" 
#include "SALOMEDSImpl_AttributeDrawable.hxx"
#include "SALOMEDSImpl_AttributeSelectable.hxx"
#include "SALOMEDSImpl_AttributeExpandable.hxx"
#include "SALOMEDSImpl_AttributeOpened.hxx"
#include "SALOMEDSImpl_AttributeTextColor.hxx"
#include "SALOMEDSImpl_AttributeTextHighlightColor.hxx"
#include "SALOMEDSImpl_AttributePixMap.hxx"
#include "SALOMEDSImpl_AttributeLocalID.hxx"
#include "SALOMEDSImpl_AttributeTarget.hxx"
#include "SALOMEDSImpl_AttributeTableOfInteger.hxx"
#include "SALOMEDSImpl_AttributeTableOfReal.hxx"
#include "SALOMEDSImpl_AttributeTableOfString.hxx"
#include "SALOMEDSImpl_AttributeStudyProperties.hxx"
#include "SALOMEDSImpl_AttributePythonObject.hxx"
#include "SALOMEDSImpl_AttributeExternalFileDef.hxx"
#include "SALOMEDSImpl_AttributeFileType.hxx"
#include "SALOMEDSImpl_AttributeFlags.hxx"
#include "SALOMEDSImpl_AttributeGraphic.hxx"
#include "SALOMEDSImpl_AttributeParameter.hxx"

#include "SALOMEDS_GenericAttribute_i.hxx"
#include "SALOMEDS_AttributeName_i.hxx"
#include "SALOMEDS_AttributeComment_i.hxx"
#include "SALOMEDS_AttributeInteger_i.hxx"
#include "SALOMEDS_AttributeReal_i.hxx"
#include "SALOMEDS_AttributeTreeNode_i.hxx"
#include "SALOMEDS_AttributeUserID_i.hxx"
#include "SALOMEDS_AttributeSequenceOfReal_i.hxx"
#include "SALOMEDS_AttributeSequenceOfInteger_i.hxx" 
#include "SALOMEDS_AttributeIOR_i.hxx"
#include "SALOMEDS_AttributePersistentRef_i.hxx" 
#include "SALOMEDS_AttributeDrawable_i.hxx"
#include "SALOMEDS_AttributeSelectable_i.hxx"
#include "SALOMEDS_AttributeExpandable_i.hxx"
#include "SALOMEDS_AttributeOpened_i.hxx"
#include "SALOMEDS_AttributeTextColor_i.hxx"
#include "SALOMEDS_AttributeTextHighlightColor_i.hxx"
#include "SALOMEDS_AttributePixMap_i.hxx"
#include "SALOMEDS_AttributeLocalID_i.hxx"
#include "SALOMEDS_AttributeTarget_i.hxx"
#include "SALOMEDS_AttributeTableOfInteger_i.hxx"
#include "SALOMEDS_AttributeTableOfReal_i.hxx"
#include "SALOMEDS_AttributeTableOfString_i.hxx"
#include "SALOMEDS_AttributeStudyProperties_i.hxx"
#include "SALOMEDS_AttributePythonObject_i.hxx"
#include "SALOMEDS_AttributeExternalFileDef_i.hxx"
#include "SALOMEDS_AttributeFileType_i.hxx"
#include "SALOMEDS_AttributeFlags_i.hxx"
#include "SALOMEDS_AttributeGraphic_i.hxx"
#include "SALOMEDS_AttributeParameter_i.hxx"

#define __CreateCORBAAttribute(TypeOfAttr,OutAttribute,CORBA_Name) if (strcmp(TypeOfAttr, #CORBA_Name) == 0) { \
    Handle(SALOMEDSImpl_##CORBA_Name) A = Handle(SALOMEDSImpl_##CORBA_Name)::DownCast(theAttr); \
    SALOMEDS_##CORBA_Name##_i* Attr = new SALOMEDS_##CORBA_Name##_i(A, theOrb); \
    attr_servant = Attr; \
    OutAttribute = Attr->CORBA_Name::_this(); \
  }


#define __CreateGenericCORBAAttribute(TypeOfAttr,OutAttribute) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeReal) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeInteger) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeSequenceOfReal) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeSequenceOfInteger) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeName) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeComment) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeIOR) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributePixMap) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeLocalID) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeTableOfInteger) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeTableOfReal) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeTableOfString) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributePythonObject) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributePersistentRef) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeDrawable) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeSelectable) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeExpandable) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeOpened) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeTextColor) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeTextHighlightColor) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeTarget) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeStudyProperties) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeExternalFileDef) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeFileType) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeFlags) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeGraphic) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeTreeNode) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeUserID) \
__CreateCORBAAttribute(TypeOfAttr,OutAttribute,AttributeParameter)

#endif
