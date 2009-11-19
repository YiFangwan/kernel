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
//  File   : SALOME_EvalSet.cxx
//  Author : Peter KURNEV
//  Module : SALOME


#include <SALOME_EvalSet.hxx>
//
#include <math.h>
#include <SALOME_Eval.hxx>

int toInt(const SALOME_String& str, bool *ok);
double toDouble(const SALOME_String& str, bool *ok);
SALOME_String toLower(const SALOME_String& str);
SALOME_String toUpper(const SALOME_String& str);

//=======================================================================
//function : SALOME_EvalSet
//purpose  : 
//=======================================================================
SALOME_EvalSet::SALOME_EvalSet()
{
}
//=======================================================================
//function : SALOME_EvalSet::~SALOME_EvalSet
//purpose  : 
//=======================================================================
SALOME_EvalSet::~SALOME_EvalSet()
{
}
//=======================================================================
//function : SALOME_EvalSet::createValue
//purpose  : 
//=======================================================================
bool SALOME_EvalSet::createValue(const SALOME_String& str, SALOME_EvalVariant& val) const
{
  val=str;
  return false;
}
//=======================================================================
//function : SALOME_EvalSet::contains
//purpose  : 
//=======================================================================
bool SALOME_EvalSet::contains( const SALOME_ListOfEvalSet& aL, const SALOME_EvalSet* pS)
{
  bool bRet;
  //
  bRet=false;
  SALOME_ListOfEvalSet::const_iterator aIt=aL.begin();
  for(; aIt!=aL.end(); ++aIt) {
    const SALOME_EvalSet* pSx=*aIt;
    bRet=pSx==pS;
    if (bRet) {
      break;
    }
  }
  return bRet;
}
//=======================================================================
//function : SALOME_EvalSet::insert
//purpose  : 
//=======================================================================
void SALOME_EvalSet::insert(SALOME_ListOfEvalSet& aL, const int aIndex, SALOME_EvalSet* pS)
{
  int i;
  //
  if (!aIndex) {
    aL.push_front(pS);
  }
  else if (aIndex==aL.size()) {
    aL.push_back(pS);
  }
  else {
    SALOME_ListOfEvalSet::iterator aIt=aL.begin();
    for(i=0; aIt!=aL.end(); ++aIt, ++i) {
      //const SALOME_EvalSet* pSx=*aIt;
      if (i==aIndex) {
        aL.insert( aIt, pS );
        break;
      }
    }
  }
}
///////////////////////////////////
//=======================================================================
//function : SALOME_EvalSetBase
//purpose  : 
//=======================================================================
SALOME_EvalSetBase::SALOME_EvalSetBase()
{
}
//=======================================================================
//function : ~SALOME_EvalSetBase
//purpose  : 
//=======================================================================
SALOME_EvalSetBase::~SALOME_EvalSetBase()
{
}
//=======================================================================
//function : SALOME_EvalSetBase::operationList
//purpose  : 
//=======================================================================
void SALOME_EvalSetBase::operationList( SALOME_StringList& aList ) const
{
  SALOME_StringList::const_iterator aIt;
  //
  aIt=myOpers.begin();
  for (; aIt!=myOpers.end(); ++aIt) {
    aList.push_back(*aIt);
  }
}
//=======================================================================
//function : SALOME_EvalSetBase::bracketsList
//purpose  : 
//=======================================================================
void SALOME_EvalSetBase::bracketsList(SALOME_StringList& aList, 
                                  bool bOpen) const
{
  aList.push_back( bOpen ? "(" : ")" );
}
//=======================================================================
//function : SALOME_EvalSetBase::addOperations
//purpose  : 
//=======================================================================
void SALOME_EvalSetBase::addOperations(const SALOME_StringList& aList)
{
  SALOME_StringList::const_iterator aIt;
  SALOME_StringSet aSet;
  SALOME_SetPair aSetPair;
  //   
  aIt=aList.begin();
  for (; aIt != aList.end(); ++aIt)
  {
    aSetPair=aSet.insert(*aIt);
    if(aSetPair.second) { 
      myOpers.push_back( *aIt );
    }
  }
}
//=======================================================================
//function : SALOME_EvalSetBase::addTypes
//purpose  : 
//=======================================================================
void SALOME_EvalSetBase::addTypes(const SALOME_ListOfEvalVariantType& aList)
{
  SALOME_ListOfEvalVariantType::const_iterator aIt;
  SALOME_SetOfEvalVariantType aSet;
  SALOME_SetPairOfEvalVariantType aSetPair;
  //
  aIt=aList.begin(); 
  for (; aIt != aList.end(); ++aIt )  {
    aSetPair=aSet.insert(*aIt);
    if(aSetPair.second) { 
      myTypes.push_back(*aIt);
    }
  }
}
//=======================================================================
//function : SALOME_EvalSetBase::isValid
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalSetBase::isValid( const SALOME_String& op,
                                                  const SALOME_EvalVariantType t1, 
                                                  const SALOME_EvalVariantType t2 ) const
{
  bool bContainsT1, bContainsT2;
  SALOME_SetOfEvalVariantType aSet;
  SALOME_SetPairOfEvalVariantType aSetPair;
  SALOME_ListOfEvalVariantType::const_iterator aIt;
  SALOME_EvalVariantType aType;
  //
  aIt=myTypes.begin(); 
  for (; aIt != myTypes.end(); ++aIt )  {
    aType=*aIt;
    aSet.insert(aType);
  }
  //
  aSetPair=aSet.insert(t1);
  bContainsT1=!aSetPair.second;
  //
  aSetPair=aSet.insert(t2);
  bContainsT2=!aSetPair.second;
  //
  if ( ( t1 == SALOME_EvalVariant_Invalid || bContainsT1 ) &&
       ( t2 == SALOME_EvalVariant_Invalid || bContainsT2 ) &&
       ( t1 != SALOME_EvalVariant_Invalid || t2 != SALOME_EvalVariant_Invalid ) ) {
    if ( priority( op, t1 != SALOME_EvalVariant_Invalid && t2 != SALOME_EvalVariant_Invalid ) > 0 ) {
      return EvalExpr_OK;
    }
    else {
      return EvalExpr_InvalidOperation;
    }
  }
  return EvalExpr_OperandsNotMatch;
}
////////////////////////////////////////////////////////////////////////
//=======================================================================
//function : SALOME_EvalSetArithmetic::SALOME_EvalSetArithmetic
//purpose  : 
//=======================================================================
SALOME_EvalSetArithmetic::SALOME_EvalSetArithmetic()
: SALOME_EvalSetBase()
{
  SALOME_String aOp;
  SALOME_StringList aStringList;
  SALOME_ListOfEvalVariantType aTypes;
  //
  aOp="+";  aStringList.push_back(aOp);
  aOp="-";  aStringList.push_back(aOp);
  aOp="*";  aStringList.push_back(aOp);
  aOp="/";  aStringList.push_back(aOp);
  aOp="=";  aStringList.push_back(aOp);
  aOp="<";  aStringList.push_back(aOp);
  aOp=">";  aStringList.push_back(aOp);
  aOp="<=";  aStringList.push_back(aOp);
  aOp=">=";  aStringList.push_back(aOp);
  aOp="<>";  aStringList.push_back(aOp);
  aOp="!=";  aStringList.push_back(aOp);
  addOperations( aStringList );
  //
  aTypes.push_back( SALOME_EvalVariant_Int );
  aTypes.push_back( SALOME_EvalVariant_UInt );
  aTypes.push_back( SALOME_EvalVariant_Double );
  addTypes(aTypes);
}
//=======================================================================
//function : SALOME_EvalSetArithmetic::~SALOME_EvalSetArithmetic
//purpose  : 
//=======================================================================
SALOME_EvalSetArithmetic::~SALOME_EvalSetArithmetic()
{
}
//=======================================================================
//function : SALOME_EvalSetArithmetic::Name
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalSetArithmetic::Name()
{
  return "Arithmetic";
}
//=======================================================================
//function : SALOME_EvalSetArithmetic::name
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalSetArithmetic::name()const
{
  return Name();
}
//=======================================================================
//function : SALOME_EvalSetArithmetic::createValue
//purpose  : 
//=======================================================================
bool SALOME_EvalSetArithmetic::createValue(const SALOME_String& str, 
                                       SALOME_EvalVariant& val ) const
{
  bool ok;
  int iX;
  double dX;
  //
  ok = false;
  
  iX=toInt(str, &ok );
  if (ok) {
    SALOME_EvalVariant aRV(iX);
    val=aRV;
    return ok;
  }
  //
  dX=toDouble(str, &ok );
  if (ok) {
    SALOME_EvalVariant aRV(dX);
    val=aRV;
    return ok;
  }
  //
  ok = SALOME_EvalSetBase::createValue( str, val );
  return ok;
}
//=======================================================================
//function : SALOME_EvalSetArithmetic::priority
//purpose  : 
//=======================================================================
int SALOME_EvalSetArithmetic::priority(const SALOME_String& op, bool isBin) const
{
  if ( isBin )
  {
    if ( op == "<" || op == ">" || op == "=" ||
         op == "<=" || op == ">=" || op == "<>" || op == "!=" )
      return 1;
    else if ( op == "+" || op == "-" )
      return 2;
    else if( op == "*" || op == "/" )
      return 3;
    else
      return 0;
  }
  else if ( op == "+" || op == "-" )
    return 5;
  else
    return 0;
}
//=======================================================================
//function : SALOME_EvalSetArithmetic::calculate
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalSetArithmetic::calculate(const SALOME_String& op, 
                                                  SALOME_EvalVariant& v1, 
                                                  SALOME_EvalVariant& v2 ) const
{
  bool bValid1, bValid2, bOk;
  SALOME_EvalExprError err;
  SALOME_EvalVariantType aType1, aType2;  
  //  
  err = EvalExpr_OK;
  //
  bValid1=v1.isValid();
  bValid2=v2.isValid();  
  aType1=v1.type();
  aType2=v2.type();

  if ( bValid1 && bValid2 ) {// binary operations
    if ( ( aType1 == SALOME_EvalVariant_Int || aType1 == SALOME_EvalVariant_UInt ) &&
         ( aType2 == SALOME_EvalVariant_Int || aType2 == SALOME_EvalVariant_UInt ) )  {
      int _v1, _v2;
      //
      _v1 = v1.toInt(&bOk);
      _v2 = v2.toInt(&bOk);

      if ( op == "+" ) {
        v1 = _v1 + _v2;
      }
      else if ( op == "-" ){
        v1 = _v1 - _v2;
      }
      else if ( op == "*" ){
        v1 = _v1 * _v2;
        }
      else if ( op == "/" ) {
        if ( _v2 != 0 ) {
          if ( _v1 % _v2 == 0 ) {
            v1 = _v1 / _v2;
          }
          else {
            v1 = double( _v1 ) / double( _v2 );
          }
        }
        else {
          err = EvalExpr_InvalidResult;
        }
      }
      else if ( op == "<" ) {
        v1 = _v1 < _v2;
      }
      else if ( op == ">" ){
        v1 = _v1 > _v2;
      }
      else if ( op == "=" ){
        v1 = _v1 == _v2;
      }
      else if ( op == "<=" ){
        v1 = _v1 <= _v2;
      }
      else if ( op == ">=" ){
        v1 = _v1 >= _v2;
      }
      else if ( op == "<>" || op == "!=" ){
        v1 = _v1 != _v2;
      }
    }
    else if ( ( aType1 == SALOME_EvalVariant_Int || aType1 == SALOME_EvalVariant_Double ) &&
              ( aType2 == SALOME_EvalVariant_Int || aType2 == SALOME_EvalVariant_Double ) )   {
      double _v1 = v1.toDouble(&bOk);
      double _v2 = v2.toDouble(&bOk);

      if ( op == "+" ) {
        v1 = _v1 + _v2;
      }
      else if ( op == "-" ){
        v1 = _v1 - _v2;
      }
      else if ( op == "*" ){
          v1 = _v1 * _v2;
      }
      else if ( op == "/" ) {
        if ( _v2 != 0 ){ 
          v1 = _v1 / _v2;
        }
        else{
          err = EvalExpr_InvalidResult;
        }
      }
      else if ( op == "<" ){
        v1 = _v1 < _v2;
      }
      else if ( op == ">" ){
        v1 = _v1 > _v2;
      }
      else if ( op == "=" ){
        v1 = _v1 == _v2;
      }
      else if ( op == "<=" ){
        v1 = _v1 <= _v2;
      }
      else if ( op == ">=" ){
        v1 = _v1 >= _v2;
      }
      else if ( op == "<>" || op == "!=" ){
        v1 = _v1 != _v2;
      }
    }
  }
  
  else {// prefix operations
    if ( op == "-" ) {
      if ( aType2 == SALOME_EvalVariant_Int ){
        v2 = -v2.toInt(&bOk);
      }
      else if ( aType2 == SALOME_EvalVariant_Double ){
        v2 = -v2.toDouble(&bOk);
      }
    }
  }
  return err;
}
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//function : SALOME_EvalSetLogic::SALOME_EvalSetLogic
//purpose  : 
//=======================================================================
SALOME_EvalSetLogic::SALOME_EvalSetLogic()
: SALOME_EvalSetBase()
{
  SALOME_String aOp;
  SALOME_StringList aStringList;
  SALOME_ListOfEvalVariantType aTypes;
  //
  aOp="and"; aStringList.push_back(aOp);
  aOp="&&";  aStringList.push_back(aOp);
  aOp="or";  aStringList.push_back(aOp);
  aOp="||";  aStringList.push_back(aOp);
  aOp="xor"; aStringList.push_back(aOp);
  aOp="not"; aStringList.push_back(aOp);
  aOp="!";   aStringList.push_back(aOp);
  aOp="imp"; aStringList.push_back(aOp);
  aOp="=";   aStringList.push_back(aOp);
  addOperations( aStringList );
  //
  aTypes.push_back( SALOME_EvalVariant_Boolean );
  aTypes.push_back( SALOME_EvalVariant_Int );
  aTypes.push_back( SALOME_EvalVariant_UInt );
  addTypes( aTypes );
}
//=======================================================================
//function : SALOME_EvalSetLogic::~SALOME_EvalSetLogic
//purpose  : 
//=======================================================================
SALOME_EvalSetLogic::~SALOME_EvalSetLogic()
{
}
//=======================================================================
//function : SALOME_EvalSetLogic::Name
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalSetLogic::Name()
{
  return "Logic";
}
//=======================================================================
//function : SALOME_EvalSetLogic::name
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalSetLogic::name() const
{
  return Name();
}
//=======================================================================
//function : SALOME_EvalSetLogic::createValue
//purpose  : 
//=======================================================================
bool SALOME_EvalSetLogic::createValue(const SALOME_String& str, SALOME_EvalVariant& val)const
{
  bool ok = true;
  SALOME_String valStr = toLower(str);
  //
  if ( valStr == "true" || valStr == "yes" )
    val = SALOME_EvalVariant( true );
  else if ( valStr == "false" || valStr == "no" )
    val = SALOME_EvalVariant( false );
  else
    ok = SALOME_EvalSetBase::createValue( str, val );
  //
  return ok;
}
//=======================================================================
//function : SALOME_EvalSetLogic::priority
//purpose  : 
//=======================================================================
int SALOME_EvalSetLogic::priority(const SALOME_String& op, bool isBin) const
{
  if ( isBin )
  {
    if ( op == "and" || 
         op == "or" || 
         op == "xor" || 
         op == "&&" || 
         op == "||" || 
         op == "imp" )
      return 1;
    else if ( op == "=" )
      return 2;
    else 
      return 0;
  }
  else if ( op == "not" || op == "!" )
    return 5;
  else
    return 0;
}
//=======================================================================
//function : SALOME_EvalSetLogic::priority
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalSetLogic::calculate(const SALOME_String& op, SALOME_EvalVariant& v1, SALOME_EvalVariant& v2 ) const
{
  SALOME_EvalExprError err = EvalExpr_OK;
  //
  int val1 = intValue( v1 );
  int val2 = intValue( v2 );
  if ( v1.isValid() && v2.isValid() )  {
    if ( op == "and" || op == "&&" ) {
      v1 = val1 && val2;
    }
    else if ( op == "or" || op == "||" ){
      v1 = val1 || val2;
    }
    else if ( op == "xor" ){
      v1 = ( !val1 && val2 ) || ( val1 && !val2 );
    }
    else if ( op == "imp" ){
      v1 = !val1 || val2;
    }
    else if ( op == "=" ){
      v1 = val1 == val2;
    }
  }
  else if ( op == "not" || op == "!" ){
    v2 = !val2;
  }
  //
  return err;
}
//=======================================================================
//function : SALOME_EvalSetLogic::intValue
//purpose  : 
//=======================================================================
int SALOME_EvalSetLogic::intValue(const SALOME_EvalVariant& v) const
{
  bool bOk;
  int res = 0;
  switch ( v.type() )
  {
  case SALOME_EvalVariant_Boolean:
    res = v.toBool() ? 1 : 0;
    break;
  case SALOME_EvalVariant_Int:
  case SALOME_EvalVariant_UInt:
    res = v.toInt(&bOk);
    break;
  default:
    break;
  }
  return res;
}
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//function : SALOME_EvalSetMath::SALOME_EvalSetMath
//purpose  : 
//=======================================================================
SALOME_EvalSetMath::SALOME_EvalSetMath()
: SALOME_EvalSetBase()
{
  SALOME_String aOp;
  SALOME_StringList aStringList;
  SALOME_ListOfEvalVariantType aTypes;
  //
  aOp="sqrt";  aStringList.push_back(aOp);
  aOp="abs";  aStringList.push_back(aOp);
  aOp="sin";  aStringList.push_back(aOp);
  aOp="cos";  aStringList.push_back(aOp);
  aOp="rad2grad";  aStringList.push_back(aOp);
  aOp="grad2rad";  aStringList.push_back(aOp);
  addOperations( aStringList );
  //
  aTypes.push_back( SALOME_EvalVariant_Int );
  aTypes.push_back( SALOME_EvalVariant_Double );
  addTypes( aTypes );
}
//=======================================================================
//function : SALOME_EvalSetMath::~SALOME_EvalSetMath
//purpose  : 
//=======================================================================
SALOME_EvalSetMath::~SALOME_EvalSetMath()
{
}
//=======================================================================
//function : SALOME_EvalSetMath::Name
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalSetMath::Name()
{
  return "Math";
}
//=======================================================================
//function : SALOME_EvalSetMath::name
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalSetMath::name() const
{
  return Name();
}
//=======================================================================
//function : SALOME_EvalSetMath::createValue
//purpose  : 
//=======================================================================
bool SALOME_EvalSetMath::createValue(const SALOME_String& str, SALOME_EvalVariant& val) const
{
  bool ok = false;
  //
  val=toInt(str, &ok);
  if (!ok) {
    val=toDouble(str, &ok);
    if (!ok) {
      ok = SALOME_EvalSetBase::createValue(str, val);
    }
  }
  return ok;
}
//=======================================================================
//function : SALOME_EvalSetMath::priority
//purpose  : 
//=======================================================================
int SALOME_EvalSetMath::priority(const SALOME_String& op, bool isBin) const
{
  if ( isBin ){
    return 0;
  }
  else if ( op == "sqrt" || 
            op == "abs" || 
            op == "sin" ||
            op == "cos" || 
            op == "rad2grad"
            || op == "grad2rad" ){
    return 1;
  }
  return 0;
}
//=======================================================================
//function : SALOME_EvalSetMath::calculate
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalSetMath::calculate(const SALOME_String& op, SALOME_EvalVariant& v1, SALOME_EvalVariant& v2 ) const
{
  bool bOk;
  SALOME_EvalExprError err = EvalExpr_OK;

  double val = v2.toDouble(&bOk);
  if ( op == "sqrt" ) {
    if ( val >= 0 ) {
      v2 = sqrt( val );
    }
    else {
      err = EvalExpr_InvalidResult;
    }
  }
  else if ( op == "abs" )  {
    if ( v2.type() == SALOME_EvalVariant_Int ) {
      v2 = abs( v2.toInt(&bOk) );
    }
    else {
      v2 = fabs( v2.toDouble(&bOk) );
    }
  }
  //
  else if ( op == "sin" ) {
    v2 = sin( val );
  }
  else if ( op == "cos" ) {
    v2 = cos( val );
  }
  else if ( op == "grad2rad" ){
    v2 = val * 3.14159256 / 180.0;
  }
  else if ( op == "rad2grad" ) {
    v2 = val * 180.0 / 3.14159256;
  }

  return err;
}
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//function : SALOME_EvalSetString::SALOME_EvalSetString
//purpose  : 
//=======================================================================
SALOME_EvalSetString::SALOME_EvalSetString()
: SALOME_EvalSetBase()
{
  SALOME_String aOp;
  SALOME_StringList aStringList;
  SALOME_ListOfEvalVariantType aTypes;
  //
  aOp="+";  aStringList.push_back(aOp);
  aOp="=";  aStringList.push_back(aOp);
  aOp="<";  aStringList.push_back(aOp);
  aOp=">";  aStringList.push_back(aOp);
  aOp="<=";  aStringList.push_back(aOp);
  aOp=">=";  aStringList.push_back(aOp);
  aOp="<>";  aStringList.push_back(aOp);
  aOp="!=";  aStringList.push_back(aOp);
  aOp="length";  aStringList.push_back(aOp);
  aOp="lower";  aStringList.push_back(aOp);
  aOp="upper";  aStringList.push_back(aOp);
  addOperations( aStringList );

  aTypes.push_back( SALOME_EvalVariant_Int );
  aTypes.push_back( SALOME_EvalVariant_Double );
  aTypes.push_back( SALOME_EvalVariant_String );
  addTypes( aTypes );
}
//=======================================================================
//function : SALOME_EvalSetString::~SALOME_EvalSetString
//purpose  : 
//=======================================================================
SALOME_EvalSetString::~SALOME_EvalSetString()
{
}
//=======================================================================
//function : SALOME_EvalSetString::Name
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalSetString::Name()
{
  return "String";    
}
//=======================================================================
//function : SALOME_EvalSetString::name
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalSetString::name()const
{
  return Name();    
}
//=======================================================================
//function : SALOME_EvalSetString::createValue
//purpose  : 
//=======================================================================
bool SALOME_EvalSetString::createValue(const SALOME_String& str, SALOME_EvalVariant& val) const
{
  bool ok = false;
  const char* myString=str.c_str();
  int myLength=(int)strlen(myString);
  //
  if (myLength > 1 && 
      myString[0] == '\'' && 
      myString[str.length() - 1] == '\'' )  {
    char *pMid;
    int i, aNb; 
    //
    aNb=myLength-2;
    pMid=new char[aNb+1];
    //
    for(i=0; i<aNb; ++i) {
      pMid[i]=myString[i+1];
    }
    pMid[aNb]=0;
    val=pMid;
    //
    delete pMid;
    //val = str.mid( 1, str.length() - 2 );
    ok = true;
  }
  else {
    ok = SALOME_EvalSetBase::createValue( str, val );
  }
  return ok;
}
//=======================================================================
//function : SALOME_EvalSetString::priority
//purpose  : 
//=======================================================================
int SALOME_EvalSetString::priority(const SALOME_String& op, bool isBin) const
{
  if ( isBin )  {
    if ( op == "+" ) {
      return 2;
    }
    else if ( op == "=" || op == "<" || op == ">" ||
      op == "<=" || op == ">=" || op == "<>" || op == "!=" ){
      return 1;
    }
    else{
      return 0;
    }
  }
  else if ( op == "length" || op == "lower" || op=="upper" ){
    return 5;
  }
  return 0;
}
//=======================================================================
//function : SALOME_EvalSetString::calculate
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalSetString::calculate(const SALOME_String& op, SALOME_EvalVariant& v1, SALOME_EvalVariant& v2) const
{
  SALOME_EvalExprError err = EvalExpr_OK;
  if ( v1.isValid() && v2.isValid() )
  {
    SALOME_String _v1 = v1.toString();
    SALOME_String _v2 = v2.toString();
    if ( op == "+" )
      v1 = _v1 + _v2;
    else if ( op == "=" )
      v1 = _v1 ==_v2;
    else if ( op == "<" )
      v1 = _v1 < _v2;
    else if ( op == ">" )
      v1 = _v1 > _v2;
    else if ( op == "<>" || op == "!=" )
      v1 = _v1 != _v2;
    else if ( op == "<=" )
      v1 = _v1 < _v2 || _v1 == _v2;
    else if ( op == ">=" )
      v1 = _v1 > _v2 || _v1 == _v2;
  }
  else if ( !v1.isValid() && v2.isValid() )
  {
    SALOME_String val = v2.toString();
    if ( op == "length" )
      v2 = (int)val.length();
    else if ( op == "lower" ) {
      v2=toLower(val);
    }
    else if ( op == "upper" ) {
      v2=toUpper(val);
    }
  }
  return err;
}
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//function : SALOME_EvalSetSets::SALOME_EvalSetSets
//purpose  : 
//=======================================================================
SALOME_EvalSetSets::SALOME_EvalSetSets()
: SALOME_EvalSetBase()
{
  SALOME_String aOp;
  SALOME_StringList aStringList;
  SALOME_ListOfEvalVariantType aTypes;
  //
  aOp="{";  aStringList.push_back(aOp);
  aOp="}";  aStringList.push_back(aOp);
  aOp="<>";  aStringList.push_back(aOp);
  aOp="!=";  aStringList.push_back(aOp);
  aOp="+";  aStringList.push_back(aOp);
  aOp="-";  aStringList.push_back(aOp);
  aOp="*";  aStringList.push_back(aOp);
  aOp="in";  aStringList.push_back(aOp);
  aOp="count";  aStringList.push_back(aOp);
  addOperations( aStringList );
  //
  aTypes.push_back( SALOME_EvalVariant_List );
  addTypes( aTypes );
}
//=======================================================================
//function : SALOME_EvalSetSets::~SALOME_EvalSetSets
//purpose  : 
//=======================================================================
SALOME_EvalSetSets::~SALOME_EvalSetSets()
{
}
//=======================================================================
//function : SALOME_EvalSetSets::Name
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalSetSets::Name()
{
  return "Sets";
}
//=======================================================================
//function : SALOME_EvalSetSets::name
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalSetSets::name()const
{
  return Name();
}
//=======================================================================
//function : SALOME_EvalSetSets::bracketsList
//purpose  : 
//=======================================================================
void SALOME_EvalSetSets::bracketsList(SALOME_StringList& aList, bool bOpen ) const
{
  aList.push_back( bOpen ? "{" : "}" );
  SALOME_EvalSetBase::bracketsList(aList, bOpen);
}
//=======================================================================
//function : SALOME_EvalSetSets::bracketsList
//purpose  : 
//=======================================================================
int SALOME_EvalSetSets::priority(const SALOME_String& op, bool isBin) const
{
  if ( isBin )  {
    if ( op == "=" || op == "<>" || op == "!=" )
      return 1;
    else if ( op == "+" || op == "-" || op == "*" )
      return 2;
    else if ( op == "in" )
      return 3;
    else
      return 0;
  }
  else if ( op == "{" || op == "}" )
    return 5;
  else if ( op == "count" )
    return 4;
  else
    return 0;
}
//=======================================================================
//function : SALOME_EvalSetSets::isValid
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalSetSets::isValid(const SALOME_String& op,
                                                 const SALOME_EvalVariantType t1,
                                                 const SALOME_EvalVariantType t2 ) const
{
  if ( op == "{" ) {
    return EvalExpr_OK;
  }
  if ( op != "in" ) {
    return SALOME_EvalSetBase::isValid( op, t1, t2 );
  }
  if ( t1 != SALOME_EvalVariant_Invalid && t2 == SALOME_EvalVariant_List ) {
    return EvalExpr_OK;
  }
  //else
  return EvalExpr_OperandsNotMatch;
}
//=======================================================================
//function : SALOME_EvalSetSets::add
//purpose  : 
//=======================================================================
void SALOME_EvalSetSets::add(ValueSet& aSet, const SALOME_EvalVariant& v)
{
  
  if ( v.isValid() && !SALOME_EvalVariant::contains( aSet, v ) ) {
    aSet.push_back( v );
  }
}
//=======================================================================
//function : SALOME_EvalSetSets::add
//purpose  : 
//=======================================================================
void SALOME_EvalSetSets::add(ValueSet& s1, const ValueSet& s2)
{
  ValueSet::const_iterator anIt = s2.begin();
  for ( ; anIt != s2.end(); ++anIt )
    add( s1, *anIt );
}
//=======================================================================
//function : SALOME_EvalSetSets::remove
//purpose  : 
//=======================================================================
void SALOME_EvalSetSets::remove(ValueSet& aSet, const SALOME_EvalVariant& v)
{
  aSet.remove(v);
}
//=======================================================================
//function : SALOME_EvalSetSets::remove
//purpose  : 
//=======================================================================
void SALOME_EvalSetSets::remove(ValueSet& s1, const ValueSet& s2)
{
  ValueSet::const_iterator aIt = s2.begin();
  for (; aIt != s2.end(); ++aIt ) {
    s1.remove(*aIt);
  }
}
//=======================================================================
//function : SALOME_EvalSetSets::calculate
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalSetSets::calculate(const SALOME_String& op, 
                                                   SALOME_EvalVariant& v1, 
                                                   SALOME_EvalVariant& v2 ) const
{
  SALOME_EvalExprError err = EvalExpr_OK;
  // !!!
  if ( op != "{" )  {
    if ( op == "}" ) {
      ValueSet aNewList;
      add( aNewList, v1.toList() );
      v1 = aNewList;
    }
    else if (op == "=" || op == "<>" || op == "!=" || 
             op == "+" || op == "-" || op == "*" ) {
      ValueSet aNewList;
      add( aNewList, v1.toList() );
      if ( op == "=" || op == "<>" || op == "!=" || op == "-" ) {
        remove( aNewList, v2.toList() );
        if ( op == "=" ){
          v1 = aNewList.empty() && v1.toList().size() == v2.toList().size();
        }
        else if ( op == "<>" || op == "!=" ) {
          v1 = !aNewList.empty() || v1.toList().size() != v2.toList().size();
        }
        else{
          v1 = aNewList;
        }
      }
      else if ( op == "+" ) {
        add( aNewList, v2.toList() );
        v1 = aNewList;
      }
      else if ( op == "*" ) {
        ValueSet toDelete;
        add( toDelete, aNewList );
        remove( toDelete, v2.toList() );
        remove( aNewList, toDelete );
        v1 = aNewList;
      }
    }
    else if ( op== "count" ) {
      v2 = (int)v2.toList().size();
    }
    else if ( op == "in" ) {
      if ( v1.type() == SALOME_EvalVariant_List ) {
        bool res = true;
        ValueSet lst1 = v1.toList();
        ValueSet lst2 = v2.toList();
        ValueSet::const_iterator anIt = lst1.begin();
        for (; anIt != lst1.end() && res; ++anIt ){
          res = SALOME_EvalVariant::contains(lst2, *anIt );
        }
        v1 = res;
      }
      else {
        SALOME_ListOfEvalVariant aL=v2.toList();
        bool bContains=SALOME_EvalVariant::contains(aL, v1);
        v1=SALOME_EvalVariant(bContains);
      }
    }
  }
  
  return err;
}
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//function : SALOME_EvalSetConst::SALOME_EvalSetConst
//purpose  : 
//=======================================================================
SALOME_EvalSetConst::SALOME_EvalSetConst()
: SALOME_EvalSet()
{
}
//=======================================================================
//function : SALOME_EvalSetConst::~SALOME_EvalSetConst
//purpose  : 
//=======================================================================
SALOME_EvalSetConst::~SALOME_EvalSetConst()
{
}
//=======================================================================
//function : SALOME_EvalSetConst::Name
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalSetConst::Name()
{
  return "Const";
}
//=======================================================================
//function : SALOME_EvalSetConst::name
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalSetConst::name() const
{
  return Name();
}
//=======================================================================
//function : SALOME_EvalSetConst::createValue
//purpose  : 
//=======================================================================
bool SALOME_EvalSetConst::createValue(const SALOME_String& str, SALOME_EvalVariant& val) const
{
  bool ok = true;
  if ( str == "pi" )            // PI number
    val = SALOME_EvalVariant( 3.141593 );
  else if ( str == "exp" )      // Exponent value (e)
    val = SALOME_EvalVariant( 2.718282 );
  else if ( str == "g" )        // Free fall acceleration (g)
    val = SALOME_EvalVariant( 9.80665 );
  else
    ok = false;

  return ok;
}
//=======================================================================
//function : SALOME_EvalSetConst::operationList
//purpose  : 
//=======================================================================
void SALOME_EvalSetConst::operationList(SALOME_StringList&) const
{
}
//=======================================================================
//function : SALOME_EvalSetConst::bracketsList
//purpose  : 
//=======================================================================
void SALOME_EvalSetConst::bracketsList(SALOME_StringList& , bool) const
{
}
//=======================================================================
//function : SALOME_EvalSetConst::priority
//purpose  : 
//=======================================================================
int SALOME_EvalSetConst::priority(const SALOME_String& /*op*/, bool /*isBin*/ ) const
{
  return 0;
}
//=======================================================================
//function : SALOME_EvalSetConst::isValid
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalSetConst::isValid(const SALOME_String& /*op*/, 
                                                  const SALOME_EvalVariantType /*t1*/,
                                                  const SALOME_EvalVariantType /*t2*/ ) const
{
  return EvalExpr_InvalidOperation;
}
//=======================================================================
//function : SALOME_EvalSetConst::calculate
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalSetConst::calculate(const SALOME_String&, 
                                             SALOME_EvalVariant&, 
                                             SALOME_EvalVariant& ) const
{
  return EvalExpr_InvalidOperation;
}

/////////////////////////////////////////////////////////////////////////
//=======================================================================
//function : toUpper
//purpose  : 
//=======================================================================
SALOME_String toUpper(const SALOME_String& str)
{
  char aC;
  const char* mystring=str.c_str();
  size_t mylength, i;
  SALOME_String aRet;
  //
  aRet=str;
  if(mystring) {
    mylength=strlen(mystring);
    for (i=0; i < mylength; i++) {
      aC=(char)toupper(mystring[i]);
      aRet.at(i)=aC;
    }
  }
  return aRet;
}
//=======================================================================
//function : tolower
//purpose  : 
//=======================================================================
SALOME_String toLower(const SALOME_String& str)
{
  char aC;
  const char* mystring=str.c_str();
  size_t mylength, i;
  SALOME_String aRet;
  //
  aRet=str;
  if(mystring) {
    mylength=strlen(mystring);
    for (i=0; i < mylength; i++) {
      aC=(char)tolower(mystring[i]);
      aRet.at(i)=aC;
    }
  }
  return aRet;
}
//=======================================================================
//function : toDouble
//purpose  : 
//=======================================================================
double toDouble(const SALOME_String& str, bool *ok)
{
  char *ptr;
  const char* mystring=str.c_str();
  double value;
  //
  value=0.;
  *ok=false;
  if(mystring) {
    value=strtod(mystring,&ptr);
    if (ptr != mystring) {
      *ok=true;
      return value;
    }
    else{
      value=0.;
    }
  }
  return value;
}
//=======================================================================
//function : toInt
//purpose  : 
//=======================================================================
int toInt(const SALOME_String& str, bool *ok)
{
  char *ptr;
  const char* mystring=str.c_str();
  size_t mylength, i;
  int value;
  //
  value=0;
  *ok=false;
  if(mystring) {
    mylength=strlen(mystring);
    value = (int)strtol(mystring, &ptr,10); 
    //
    if (ptr != mystring) {
      for (i=0; i < mylength; i++) {
        if (mystring[i] == '.') {
          value=0;
          return value;
        }
      }
      *ok=true;
      return value;
    }
  }
  return value;
}
