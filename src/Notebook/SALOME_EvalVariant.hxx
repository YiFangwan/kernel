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
//  File   : SALOME_EvalVariant.hxx
//  Author : Peter KURNEV
//  Module : SALOME

#ifndef SALOME_EvalVariant_Header_File
#define SALOME_EvalVariant_Header_File
//
#ifdef WNT
#pragma warning(disable : 4786)
#endif
//
#include <list>
#include <set>
#include <string>
#include <SALOME_Eval.hxx>
//
using namespace std;
//
typedef unsigned int uint;
//

class SALOME_EvalVariant;
class SALOME_EvalVariantData;
typedef list<SALOME_EvalVariant> SALOME_ListOfEvalVariant;
//
//  SALOME_EvalVariantType
enum SALOME_EvalVariantType {
  SALOME_EvalVariant_Invalid,
  SALOME_EvalVariant_Boolean,
  SALOME_EvalVariant_Int,
  SALOME_EvalVariant_UInt,
  SALOME_EvalVariant_Double,
  SALOME_EvalVariant_String,
  SALOME_EvalVariant_List 
};
//
typedef list<SALOME_EvalVariantType> SALOME_ListOfEvalVariantType;
//
typedef set<SALOME_EvalVariantType> SALOME_SetOfEvalVariantType;
typedef SALOME_SetOfEvalVariantType::iterator SALOME_SetIteratorOfSetOfEvalVariantType;
typedef pair<SALOME_SetIteratorOfSetOfEvalVariantType, bool> SALOME_SetPairOfEvalVariantType;
//
//=======================================================================
//union   : SALOME_EvalVariantValue
//purpose  : 
//=======================================================================
union SALOME_EvalVariantValue {
  bool   myBoolean;
	int    myInt;
	uint   myUInt;
	double myDouble;
	void*  myPtr;
};
//=======================================================================
//class : SALOME_EvalVariantData
//purpose  : 
//=======================================================================
class SALOME_EvalVariantData {
  public:
    SALOME_EvalVariantData(){
      myType=SALOME_EvalVariant_Invalid;
      myValue.myBoolean=false;
      myValue.myInt=0;
      myValue.myUInt=0;
      myValue.myDouble=0.;
      myValue.myPtr=NULL;
    };
    //
    ~SALOME_EvalVariantData(){
      clear();
    };
    //
    SALOME_EvalVariantData(const SALOME_EvalVariantData& other); 
    SALOME_EvalVariantData& operator=(const SALOME_EvalVariantData& other); 
    bool operator==(const SALOME_EvalVariantData& other) const; 
    //
    void clear(){
      if (myType==SALOME_EvalVariant_String) {
	if (myValue.myPtr) {
	  RString *pStr=(RString *)myValue.myPtr;
          delete pStr;
          myValue.myPtr=NULL;
        }
	
      }
      else if (myType==SALOME_EvalVariant_List) {
        if (myValue.myPtr) {
	  SALOME_ListOfEvalVariant *p=(SALOME_ListOfEvalVariant *)myValue.myPtr;
          delete p;
          myValue.myPtr=NULL;
        }
      }
      else {
        myValue.myBoolean=false;
	myValue.myInt=0;
	myValue.myUInt=0;
	myValue.myDouble=0.;
	myValue.myPtr=NULL;
      }
    };
    //
    SALOME_EvalVariantType   myType;
    SALOME_EvalVariantValue  myValue;
};
//=======================================================================
//class : SALOME_EvalVariant
//purpose  : 
//=======================================================================
class SALOME_EvalVariant
{
  public:  
    SALOME_EvalVariant(){
    }; 
    //
    ~SALOME_EvalVariant(){
      myData.clear();
    };
    //
    SALOME_EvalVariant(const SALOME_EvalVariant& other) :
      myData(other.myData) {
    };
    //  
    SALOME_EvalVariant& operator=(const SALOME_EvalVariant& other) {
      myData=other.myData;
      return *this;
    };
    //
    bool operator==(const SALOME_EvalVariant& theOther) const {
      return myData==theOther.myData;;
    };
    //
    ////  
    void operator=(const RString& aString); 
    void operator=(const bool theValue); 
    void operator=(const int theValue); 
    void operator=(const uint theValue); 
    void operator=(const double theValue); 
    void operator=(const char* theValue); 
    ////
    SALOME_EvalVariant(bool b);
    SALOME_EvalVariant(int i);
    SALOME_EvalVariant(uint ui);
    SALOME_EvalVariant(double d);
    SALOME_EvalVariant(const RString& s);
    SALOME_EvalVariant(const SALOME_ListOfEvalVariant& s);
    //
    SALOME_EvalVariantType type() const {
      return DataType();
    };
    //
    bool isValid() const {
      return type()!=SALOME_EvalVariant_Invalid;
    }
    //
    bool isNull() const {
      return !isValid();
    };
    //
    void clear() {
      myData.clear();
    };
    //
    bool toBool() const;
    int  toInt(bool *ok = 0) const;
    uint toUInt(bool *ok = 0) const;
    double toDouble(bool *ok = 0) const;
    RString toString() const;
    SALOME_ListOfEvalVariant toList() const; 
    //
    const SALOME_EvalVariantData& Data() const{
      return myData;
    };
    //
    static bool contains(const SALOME_ListOfEvalVariant& aL,
                         const SALOME_EvalVariant& aV);
    //
  protected:
    SALOME_EvalVariantType& ChangeDataType() {
      return ChangeData().myType;
    };
    //
    SALOME_EvalVariantData& ChangeData() {
      return myData;
    };
    //
    SALOME_EvalVariantValue& ChangeDataValue() {
      return ChangeData().myValue;
    };
    //
    bool& ChangeDataValueBoolean() {
      return ChangeDataValue().myBoolean;
    };
    //
    int& ChangeDataValueInt() {
      return ChangeDataValue().myInt;
    };
    //
    uint& ChangeDataValueUInt() {
      return ChangeDataValue().myUInt;
    };
    //
    double& ChangeDataValueDouble() {
      return ChangeDataValue().myDouble;
    };
    //
    const SALOME_EvalVariantType& DataType() const{
      return Data().myType;
    };
    //
    const SALOME_EvalVariantValue& DataValue() const{
      return Data().myValue;
    };
    //
    bool DataValueBoolean() const {
      return DataValue().myBoolean;
    };
    //
    int DataValueInt() const {
      return DataValue().myInt;
    };
    //
    uint DataValueUInt() const {
      return DataValue().myUInt;
    };
    //
    double DataValueDouble() const {
      return DataValue().myDouble;
    };
    //
    const RString& DataValueString() const {
      return *((string *)(DataValue().myPtr));
    };
    //
    const SALOME_ListOfEvalVariant& DataValueList() const {
      return *((SALOME_ListOfEvalVariant *)(DataValue().myPtr));
    };
    //
  protected:
    SALOME_EvalVariantData myData;// data itself
};

#endif
