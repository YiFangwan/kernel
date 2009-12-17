//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : SALOME_EvalVariant.cxx
//  Author : Peter KURNEV
//  Module : SALOME

#include <SALOME_EvalVariant.hxx>

#include <stdlib.h>
#include <stdio.h>

//=======================================================================
//function : SALOME_EvalVariantData
//purpose  : 
//=======================================================================
SALOME_EvalVariantData::SALOME_EvalVariantData(const SALOME_EvalVariantData& other) 
{
  myType=other.myType;
  myValue.myPtr=NULL;
  //
  switch (myType) {
    case SALOME_EvalVariant_String: {
      SALOME_String& aStr=*(SALOME_String *)(other.myValue.myPtr);
      SALOME_String *pStr=new SALOME_String(aStr);
      myValue.myPtr=(void*)pStr;
    }
      break;

    case SALOME_EvalVariant_List: {
      SALOME_ListOfEvalVariant& aX=*(SALOME_ListOfEvalVariant*)(other.myValue.myPtr);
      SALOME_ListOfEvalVariant *p=new SALOME_ListOfEvalVariant(aX);
      myValue.myPtr=(void*)p;            
    }
      break;

    default:
      myValue=other.myValue;
      break;
  }
}
//=======================================================================
//function : operator=
//purpose  : 
//=======================================================================
SALOME_EvalVariantData& SALOME_EvalVariantData::operator=(const SALOME_EvalVariantData& other) 
{
  clear();
  myType=other.myType;
  switch (myType) {
    case SALOME_EvalVariant_String: {
      SALOME_String& aStr=*(string *)(other.myValue.myPtr);
      SALOME_String *pStr=new SALOME_String(aStr);
      myValue.myPtr=(void*)pStr;
    }
      break;

    case SALOME_EvalVariant_List: {
      SALOME_ListOfEvalVariant& aX=*(SALOME_ListOfEvalVariant*)(other.myValue.myPtr);
      SALOME_ListOfEvalVariant *p=new SALOME_ListOfEvalVariant(aX);
      myValue.myPtr=(void*)p;            
    }
      break;

    default:
      myValue=other.myValue;
      break;
  }
  return *this;
}
//=======================================================================
//function : operator==
//purpose  : 
//=======================================================================
bool SALOME_EvalVariantData::operator==(const SALOME_EvalVariantData& theOther) const
{
  bool bRet;
  //
  bRet=myType==SALOME_EvalVariant_Invalid || theOther.myType==SALOME_EvalVariant_Invalid;
  if (!bRet) {
    return bRet;
  }
  //
  bRet=myType==theOther.myType;
  if (!bRet) {
    return bRet;
  }
  //
  switch (myType) {
    case SALOME_EvalVariant_Boolean:
      bRet=myValue.myBoolean==theOther.myValue.myBoolean;
      break;
    //
    case SALOME_EvalVariant_Int:
      bRet=myValue.myInt==theOther.myValue.myInt;
      break;
    //
    case SALOME_EvalVariant_UInt:
      bRet=myValue.myUInt==theOther.myValue.myUInt;
      break;
    //
    case SALOME_EvalVariant_Double:
      bRet=myValue.myDouble==theOther.myValue.myDouble;
      break;
    //
    case SALOME_EvalVariant_String: {
      bool bIsNull, bIsNullOther;
      //
      bIsNull=!myValue.myPtr;
      bIsNullOther=!theOther.myValue.myPtr;
      if (bIsNull && bIsNullOther) {
          bRet=true;
      }
      else if (bIsNull && !bIsNullOther) {
        bRet=false;
      }
      else if (!bIsNull && bIsNullOther) {
        bRet=false;
      }
      else {
        SALOME_String& myStr=*(SALOME_String *)(myValue.myPtr);
        SALOME_String& aOther=*(SALOME_String *)(theOther.myValue.myPtr);
        bRet=myStr==aOther;
      }
    }
      break;
    //
    case SALOME_EvalVariant_List: {
      bool bIsNull, bIsNullOther;
      //
      bIsNull=!myValue.myPtr;
      bIsNullOther=!theOther.myValue.myPtr;
      if (bIsNull && bIsNullOther) {
          bRet=true;
      }
      else if (bIsNull && !bIsNullOther) {
        bRet=false;
      }
      else if (!bIsNull && bIsNullOther) {
        bRet=false;
      }
      else {
        size_t aNb1, aNb2;
        //
        const SALOME_ListOfEvalVariant& aL1=*(SALOME_ListOfEvalVariant*)(myValue.myPtr);
        const SALOME_ListOfEvalVariant& aL2=*(SALOME_ListOfEvalVariant*)(theOther.myValue.myPtr);
        aNb1=aL1.size();
        aNb2=aL2.size();
        bRet=aNb1==aNb2;
        if (!bRet) {
          break;
        }
        //
        SALOME_ListOfEvalVariant::const_iterator aIt1 = aL1.begin();
        SALOME_ListOfEvalVariant::const_iterator aIt2 = aL2.begin();
        for ( ; aIt1 != aL1.end(); ++aIt1, ++aIt2 ) {
          const SALOME_EvalVariant& aV1=*aIt1;
          const SALOME_EvalVariant& aV2=*aIt2;
          const SALOME_EvalVariantData& aD1=aV1.Data();
          const SALOME_EvalVariantData& aD2=aV2.Data();
          bRet=aD1==aD2;
          if (!bRet) {
            break;
          }
        }
      }
    }
      break;

    default:
      bRet=false;
      break;
  }
  return bRet;
}
/////////////////////////////////////////////////////////////////////////
//  
// class: SALOME_EvalVariant
//


//=======================================================================
//function : SALOME_EvalVariant
//purpose  : 
//=======================================================================
SALOME_EvalVariant::SALOME_EvalVariant(bool theValue)
{
  ChangeDataType()=SALOME_EvalVariant_Boolean;
  ChangeDataValue().myBoolean=theValue;
}
//=======================================================================
//function : SALOME_EvalVariant
//purpose  : 
//=======================================================================
SALOME_EvalVariant::SALOME_EvalVariant(int theValue)
{
  ChangeDataType()=SALOME_EvalVariant_Int;
  ChangeDataValue().myInt=theValue;
}
//=======================================================================
//function : SALOME_EvalVariant
//purpose  : 
//=======================================================================
SALOME_EvalVariant::SALOME_EvalVariant(uint theValue)
{
  ChangeDataType()=SALOME_EvalVariant_UInt;
  ChangeDataValue().myUInt=theValue;
}
//=======================================================================
//function : SALOME_EvalVariant
//purpose  : 
//=======================================================================
SALOME_EvalVariant::SALOME_EvalVariant(double theValue)
{
  ChangeDataType()=SALOME_EvalVariant_Double;
  ChangeDataValue().myDouble=theValue;
}
//=======================================================================
//function : SALOME_EvalVariant
//purpose  : 
//=======================================================================
SALOME_EvalVariant::SALOME_EvalVariant(const SALOME_String& theValue)
{
  ChangeDataType()=SALOME_EvalVariant_String;
  SALOME_String *p=new SALOME_String(theValue);
  ChangeDataValue().myPtr=(void*)p;
}
//=======================================================================
//function : SALOME_EvalVariant
//purpose  : 
//=======================================================================
SALOME_EvalVariant::SALOME_EvalVariant(const SALOME_ListOfEvalVariant& theValue)
{
  ChangeDataType()=SALOME_EvalVariant_List;
  SALOME_ListOfEvalVariant *p=new SALOME_ListOfEvalVariant(theValue);
  ChangeDataValue().myPtr=(void*)p;
}
//=======================================================================
//function : operator=
//purpose  : 
//=======================================================================
void SALOME_EvalVariant::operator=(const SALOME_String& theValue)
{
  clear();
  //
  ChangeDataType()=SALOME_EvalVariant_String;
  SALOME_String *p=new SALOME_String(theValue);
  ChangeDataValue().myPtr=(void*)p;
}
//=======================================================================
//function : operator=
//purpose  : 
//=======================================================================
void SALOME_EvalVariant::operator=(const bool theValue)
{
  clear();
  //
  ChangeDataType()=SALOME_EvalVariant_Boolean;
  ChangeDataValueBoolean()=theValue;
}
//=======================================================================
//function : operator=
//purpose  : 
//=======================================================================
void SALOME_EvalVariant::operator=(const int theValue)
{
  clear();
  //
  ChangeDataType()=SALOME_EvalVariant_Int;
  ChangeDataValueInt()=theValue;
}
//=======================================================================
//function : operator=
//purpose  : 
//=======================================================================
void SALOME_EvalVariant::operator=(const uint theValue)
{
  clear();
  //
  ChangeDataType()=SALOME_EvalVariant_UInt;
  ChangeDataValueUInt()=theValue;
}
//=======================================================================
//function : operator=
//purpose  : 
//=======================================================================
void SALOME_EvalVariant::operator=(const double theValue)
{
  clear();
  //
  ChangeDataType()=SALOME_EvalVariant_Double;
  ChangeDataValueDouble()=theValue;
}
//=======================================================================
//function : operator=
//purpose  : 
//=======================================================================
void SALOME_EvalVariant::operator=(const char* theValue)
{
  clear();
  //
  ChangeDataType()=SALOME_EvalVariant_String;
  SALOME_String *p=new SALOME_String(theValue);
  ChangeDataValue().myPtr=(void*)p;
}
//=======================================================================
//function : toBool
//purpose  : 
//=======================================================================
bool SALOME_EvalVariant::toBool() const
{
  SALOME_EvalVariantType aType=type();
  //
  if ( aType == SALOME_EvalVariant_Boolean ) {
	  return DataValueBoolean();
  }
  else if ( aType == SALOME_EvalVariant_Int ) {
	  return DataValueInt() != 0;
  }
  else if ( aType == SALOME_EvalVariant_UInt ) {
	  return DataValueUInt() != 0;
  }
  else if ( aType == SALOME_EvalVariant_Double ) {
	  return DataValueDouble() != 0.;
  }
  else if ( aType == SALOME_EvalVariant_String ) {
    SALOME_String aZero("0"), aFalse("False");
    const SALOME_String& aStr=DataValueString();
    return !(aStr==aZero || aStr==aFalse || aStr.empty());
  }
  return false;
}
//=======================================================================
//function : toInt
//purpose  : 
//=======================================================================
int SALOME_EvalVariant::toInt(bool *ok) const
{
  if( ok )
    *ok = true;

  SALOME_EvalVariantType aType=type();
  if (aType == SALOME_EvalVariant_Boolean ) {
	  return (int)DataValueBoolean();
  }
  else if (aType == SALOME_EvalVariant_Int ) {
	  return DataValueInt();
  }
  else if (aType == SALOME_EvalVariant_UInt ) {
	  return (int)DataValueUInt();
  }
  else if (aType == SALOME_EvalVariant_Double ) {
	  return (int)DataValueDouble();
  }
  else if (aType == SALOME_EvalVariant_String) {
    int iRet;
    const SALOME_String& aStr = DataValueString();
    bool res = sscanf( aStr.c_str(), "%i", &iRet ) == 1;
    if( ok )
      *ok = res;
    return iRet;
  }
  return 0;
}
//=======================================================================
//function : toUInt
//purpose  : 
//=======================================================================
uint SALOME_EvalVariant::toUInt(bool *ok) const
{
  int iX;
  //
  iX=toInt(ok);
  //
  return (uint)iX;
}
//=======================================================================
//function : toDouble
//purpose  : 
//=======================================================================
double SALOME_EvalVariant::toDouble(bool *ok) const
{
  if( ok )
    *ok = true;
  //
  SALOME_EvalVariantType aType=type();
  //
  if (aType == SALOME_EvalVariant_Boolean ) {
	  return (double)DataValueBoolean();
  }
  else if (aType == SALOME_EvalVariant_Int ) {
	  return (double)DataValueInt();
  }
  else if (aType == SALOME_EvalVariant_UInt ) {
	  return (double)DataValueUInt();
  }
  else if (aType == SALOME_EvalVariant_Double ) {
	  return (double)DataValueDouble();
  }
  else if (aType == SALOME_EvalVariant_String) {
    double dRet;
    //
    const SALOME_String& aStr = DataValueString();
    bool res = sscanf( aStr.c_str(), "%lf", &dRet ) == 1;
    if( ok )
      *ok = res;
    return dRet;
  }
  return 0.;
}

//=======================================================================
//function : toString
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalVariant::toString() const
{
  bool bOk;
  char buffer[32];
  SALOME_String aS;
  //
  SALOME_EvalVariantType aType=type();
  //
  if (aType == SALOME_EvalVariant_Boolean) {
    int iX;
    //
    iX=toInt(&bOk);
    aS= iX? "True" : "False";
  }
  else if (aType == SALOME_EvalVariant_Int) {
    sprintf(buffer, "%d", DataValueInt());
    aS=buffer;
  }
  else if (aType == SALOME_EvalVariant_UInt ) {
    sprintf(buffer, "%d", (int)DataValueUInt());
    aS=buffer;
  }
  else if (aType == SALOME_EvalVariant_Double ) {
    
    double aX;
    //
    aX=DataValueDouble();
    gcvt(aX, 12, buffer);
    aS=buffer;
  }
  else if (aType == SALOME_EvalVariant_String) {
    const SALOME_String& aStr=DataValueString();
    aS=aStr;
  }
  return aS;
}
//=======================================================================
//function : toList
//purpose  : 
//=======================================================================
SALOME_ListOfEvalVariant SALOME_EvalVariant::toList() const
{
  SALOME_ListOfEvalVariant aS;
  //
  SALOME_EvalVariantType aType=type();
  //
  if (aType == SALOME_EvalVariant_List) {
    aS=DataValueList();
  }
  return aS;
}
//=======================================================================
//function : contains
//purpose  : 
//=======================================================================
bool SALOME_EvalVariant::contains(const SALOME_ListOfEvalVariant& aL,
                        const SALOME_EvalVariant& aV)
{
  bool bRet;
  //
  bRet=false;
  SALOME_ListOfEvalVariant::const_iterator aIt = aL.begin();
  for ( ; aIt != aL.end(); ++aIt) {
    const SALOME_EvalVariant& aVx=*aIt;
    bRet=aVx==aV;
    if (bRet) {
      break;
    }
  }
  return bRet;
}
