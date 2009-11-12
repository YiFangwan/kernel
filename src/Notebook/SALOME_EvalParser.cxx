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
//  File   : SALOME_EvalParser.cxx
//  Author : Peter KURNEV
//  Module : SALOME

#include <SALOME_EvalParser.hxx>

#include <stack>
#include <list>

static
  bool contains(const RStringList& aLS,
              const RString& aS);
static
  RString trimmed(const RString& str);

static
  RString trimmed(const RString& str, const char aWhat);

static
  bool isSpace(const char aC);
  
static
  int indexOf(const RStringList& aLS,
              const RString& aS);

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
SALOME_EvalParser::SALOME_EvalParser()
: myAutoDel( false )
{
  setError( SALOME_EvalExpr_OK );
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
SALOME_EvalParser::~SALOME_EvalParser()
{
  if (autoDeleteOperationSets()) {
    //qDeleteAll( mySets );// !!!
    SALOME_ListOfPEvalSet::const_iterator aIt = mySets.begin(); 
    for (; aIt != mySets.end() ; ++aIt )  {
      SALOME_EvalSet* pEvalSet=*aIt;
      if (pEvalSet) {
        delete pEvalSet;
        pEvalSet=NULL;
      }
    }
  }
}
//=======================================================================
//function : operationSets
//purpose  : 
//=======================================================================
SALOME_ListOfPEvalSet SALOME_EvalParser::operationSets() const
{
  return mySets;
}
//=======================================================================
//function : operationSet
//purpose  : 
//=======================================================================
SALOME_EvalSet* SALOME_EvalParser::operationSet(const RString& theName) const
{
  SALOME_EvalSet* pSet;
  //
  pSet = NULL;
  SALOME_ListOfPEvalSet::const_iterator it = mySets.begin(); 
  for (; it != mySets.end() && !pSet; ++it )  {
    if ( (*it)->name()==theName ) {
      pSet = *it;
    }
  }
  return pSet;
}
//=======================================================================
//function : insertOperationSet
//purpose  : 
//=======================================================================
void SALOME_EvalParser::insertOperationSet(SALOME_EvalSet* theSet, 
                                       const int idx)
{
  if (SALOME_EvalSet::contains(mySets, theSet)) {
    return;
  }
  //  
  int iSize=(int)mySets.size();
  int index = idx < 0 ? iSize : idx;
  if (index>iSize) {
    index=iSize;
  }
  SALOME_EvalSet::insert(mySets, index, theSet);
}
//=======================================================================
//function : removeOperationSet
//purpose  : 
//=======================================================================
void SALOME_EvalParser::removeOperationSet(SALOME_EvalSet* theSet)
{
  //mySets.removeAll( theSet );
  mySets.remove(theSet);
}
//=======================================================================
//function : autoDeleteOperationSets
//purpose  : 
//=======================================================================
bool SALOME_EvalParser::autoDeleteOperationSets() const
{
  return myAutoDel;
}
//=======================================================================
//function : setAutoDeleteOperationSets
//purpose  : 
//=======================================================================
void SALOME_EvalParser::setAutoDeleteOperationSets( const bool theOn )
{
  myAutoDel = theOn;
}
//=======================================================================
//function : search
//purpose  : 
//=======================================================================
int SALOME_EvalParser::search(const RStringList& aList, 
                          const RString& aStr,
                          int offset, 
                          int& matchLen, 
                          int& listind )
{
  int min = -1;
  int ind = 0;
  int pos;
  //static const basic_string <char>::size_type npos = -1;
  //
  RStringList::const_iterator aIt = aList.begin(), aLast = aList.end();
  for (ind = 0; aIt != aLast; aIt++, ind++ ) {
    const RString& aStrX=*aIt;
    
    pos=(int)aStr.find(aStrX, offset);
    if ( pos >= 0 && 
         ( min < 0 || min > pos || 
          ( min == pos && matchLen < (int)aStrX.length())
         )
       )  {
      min = pos;
      listind = ind;
      matchLen = (int)aStrX.length();
    }
    
  }
  if ( min < 0 ){
    matchLen = 0;
  }
  return min;
}
//=======================================================================
//function : note
//purpose  : 
//=======================================================================
RString SALOME_EvalParser::note(const RString& aStr, int pos, int len )
{
  RString aStr1;
  //
  aStr1=aStr.substr(pos, len);
  aStr1=trimmed(aStr1);

  return aStr1;
}

//=======================================================================
//function : prepare
//purpose  : 
//=======================================================================
bool SALOME_EvalParser::prepare(const RString& expr, Postfix& post)
{
  int pos = 0;
  int len =(int)expr.length();
  stack<int> aBracketStack;
  RStringList anOpers, anOpenBr, aCloseBr;
  if ( !checkOperations() )
    return false;

  bracketsList( anOpenBr, true );
  bracketsList( aCloseBr, false );
  operationList( anOpers );
  
  while ( pos < len && error() == SALOME_EvalExpr_OK )  {
    PostfixItem item;
    char aC=expr[pos];
    //
    while ( pos < len && isSpace(aC) ) {
      pos++;
    }
    //
    if ( pos >= len ) {
      break;
    }
    //
    int mBrLen = 0, mLen = 0, br_ind = -1, op_ind = -1;
    int oPos = search( anOpenBr, expr, pos, mBrLen, br_ind );
    int cPos = oPos == pos ? -1 : search( aCloseBr, expr, pos, mBrLen, br_ind );
    int opPos = search( anOpers, expr, pos, mLen, op_ind );
    //
    if ( aC=='\'')  {
      int vpos = pos + 1;
      while ( vpos < (int)expr.length() && expr[vpos] != '\'' ) {
        vpos++;
      }
      //
      mLen = vpos - pos + 1;
      //
      int res = createValue( note( expr, pos, mLen ), item.myValue );
      item.myType = res ? Value : Param;
      post.push_back( item );
      pos = vpos + 1;
      continue;
    }
    //
    if ( oPos == pos )  {
      aBracketStack.push( br_ind );
      item.myValue = note( expr, pos, mBrLen );
      item.myType = Open;
      post.push_back( item );
    }
    else if ( cPos == pos )  {
      if ( aBracketStack.size() == 0 )  {
        setError( SALOME_EvalExpr_ExcessClose );
        break;
      }
      if ( br_ind != aBracketStack.top() )  {
        setError( SALOME_EvalExpr_BracketsNotMatch );
        break;
      }
      else {
        aBracketStack.pop();
        item.myValue = note( expr, pos, mBrLen );
        item.myType = Close;
        post.push_back( item );
      }
    }
    else {
      mBrLen = 0;
    }
    //
    if ( opPos == pos ) {
      mBrLen = 0;
      item.myValue = note( expr, pos, mLen );
      item.myType = Binary;

      if ( oPos == pos ) {
        insert(post, (int)(post.size()-1), item);
      }
      else
        post.push_back( item );
    }
    else  {
      mLen = 0;
      if ( oPos != pos && cPos != pos )   {
	      int i; 
        for ( i = pos + 1; i < (int)expr.length(); i++ )  {
          if ( isSpace(expr[i]) ) {
            break;
          }
        }
        //
        int vpos = i;
        if ( oPos >= 0 && oPos < vpos ) {
          vpos = oPos;
        }
        if ( cPos >= 0 && cPos < vpos ){
          vpos = cPos;
        }
        if ( opPos >= 0 && opPos < vpos ){
          vpos = opPos;
        }

        while( vpos < (int)expr.length() && 
               ( isalpha(expr[vpos]) || 
                 isdigit(expr[vpos]) || 
                 expr[vpos]=='_' ) 
                ) {
          vpos++;
        }
        //
        mLen = vpos - pos;
        bool res = createValue( note( expr, pos, mLen ), item.myValue );
        item.myType = res ? Value : Param;
        post.push_back( item );
      }
    }
    pos += mBrLen + mLen;
  }

  //Bracket checking
  int brValue = 0;
  for ( Postfix::iterator anIt = post.begin(); anIt != post.end(); ++anIt ) {
    if ( (*anIt).myType == Open ){
      brValue++;
    }
    else if ( (*anIt).myType == Close ) {
      if ( brValue > 0 ){
        brValue--;
      }
      else {
        setError( SALOME_EvalExpr_ExcessClose );
        break;
      }
    }
  }
  //
  if ( brValue > 0 ) {
    setError( SALOME_EvalExpr_CloseExpected );
  }
  //
  return error() == SALOME_EvalExpr_OK;
}
//=======================================================================
//function : setOperationTypes
//purpose  : 
//=======================================================================
bool SALOME_EvalParser::setOperationTypes( Postfix& post )
{
  if ( !checkOperations() )
    return false;

  RStringList anOpen, aClose;
  //
  bracketsList( anOpen, true );
  bracketsList( aClose, false );

  Postfix::iterator aPrev, aNext;
  Postfix::iterator anIt = post.begin();
  for (; anIt != post.end(); ++anIt )  {
    aPrev = anIt;
    aPrev--;
    aNext = anIt;
    aNext++;
    if ( (*anIt).myType != Binary ){
      continue;
    }
    //
    if ( ( anIt == post.begin() || (*aPrev).myType == Open ||
           (*aPrev).myType == Pre || (*aPrev).myType == Binary ) &&  aNext != post.end() &&
         ( (*aNext).myType == Value || (*aNext).myType == Param ||
           (*aNext).myType == Open  || (*aNext).myType == Binary ) )
      (*anIt).myType = Pre;
    //
    else if ( anIt != post.begin() && ( (*aPrev).myType == Close || (*aPrev).myType == Param ||
                                        (*aPrev).myType == Value || (*aPrev).myType == Pre ||
                                        (*aPrev).myType == Post || (*aPrev).myType == Binary ) &&
              ( aNext == post.end() || (*aNext).myType == Close ) )
      (*anIt).myType = Post;
    //
    SALOME_EvalVariant& aRV=(*anIt).myValue;
    RString  aRVS=aRV.toString();
    //
    if ( contains(anOpen, aRVS) ) {
      (*anIt).myType = Pre;
    }
    else if ( contains(aClose, aRVS) ) {
      (*anIt).myType = Post;
    }
  }

  return error() == SALOME_EvalExpr_OK;
}
//=======================================================================
//function : globalBrackets
//purpose  : 
//=======================================================================
int SALOME_EvalParser::globalBrackets(const Postfix& post, 
                                  int f, 
                                  int l )
{
  int i;
  int start_br = 0;
  int fin_br = 0;
  int br = 0;
  int br_num = 0;
  int min_br_num = (l-f+1)*5;

  PostfixItem *pPost;
  pPost=new PostfixItem[post.size()];
  //
  Postfix::const_iterator aIt = post.begin();
  for (i=0; aIt != post.end(); ++aIt, ++i)  {
    const PostfixItem& aPostfixItem=*aIt;
    pPost[i]=aPostfixItem;
  }

  for( i=f; i<=l; i++ )
    if( pPost[ i ].myType==SALOME_EvalParser::Open )
      start_br++;
    else
      break;
  for( i=l; i>=f; i-- )
    if( pPost[ i ].myType==SALOME_EvalParser::Close )
      fin_br++;
    else
      break;

  br = start_br<fin_br ? start_br : fin_br;
  for( i=f+br; i<=l-br; i++ )
  {
    if( pPost[i].myType==SALOME_EvalParser::Open )
      br_num++;
    else if( pPost[i].myType==SALOME_EvalParser::Close )
      br_num--;
    if( br_num<min_br_num )
      min_br_num = br_num;
  }
  
  //delete pPost;
  delete [] (PostfixItem *)pPost;
  return br+min_br_num;
}
//=======================================================================
//function : sort
//purpose  : 
//=======================================================================
bool SALOME_EvalParser::sort(const Postfix& post, 
                         Postfix& res, 
                         const RStringList& anOpen,
                         const RStringList& aClose, 
                         int f, 
                         int l)
{
  if ( l < f ){
    return true;
  }
  if ( f < 0 ) {
    f = 0;
  }
  if ( l < 0 ) {
    l = (int)(post.size() - 1);
  }

  int br = globalBrackets( post, f, l );
  f += br;
  l -= br;

  if ( f == l && f >= 0 ) {
    res.push_back(at(post,f));
  }
  if ( l <= f ) {
    return true;
  }

  if ( !checkOperations() ) {
    return false;
  }

  int min = -1;
  list<int> argmin;
  list<PostfixItemType> min_types;
  //
  for ( int i = 0, j = f; j <= l; i++, j++ )  {
    const PostfixItem& item = at(post,j);//!!!
    PostfixItemType tt = item.myType;
    if ( tt == Binary || tt == Pre || tt == Post )  {
      int cur_pr = priority( item.myValue.toString(), tt == Binary );
      if ( cur_pr > 0 ) {
        if ( min < 0 || min >= cur_pr ) {
          if ( min == cur_pr ) {
            argmin.push_back( f + i );
            min_types.push_back( tt );
          }
          else {
            min = cur_pr;
            argmin.clear();
            argmin.push_back( f + i );
            min_types.clear(); 
            min_types.push_back( tt );
          }
        }
      }
      else {
        setError( SALOME_EvalExpr_InvalidOperation );
        break;
      }
    }
    else if ( tt == Open ){
      RString opBr = item.myValue.toString();
      int ind, brValue = 0;
      ind = indexOf(anOpen, opBr );
      while ( j <= l ) {
        const PostfixItem& anItem = at(post,j);
        if ( anItem.myType == Open )
          brValue++;

        if ( anItem.myType == Close ) {
          brValue--;
          RString clBr = anItem.myValue.toString();
          if ( indexOf(aClose, clBr ) == ind && brValue == 0 ){
            break;
          }
        }
        i++; j++;
      }
      //
      if ( brValue > 0 )      {
        setError( SALOME_EvalExpr_CloseExpected );
        break;
      }
    }
  }
  //
  if ( error() == SALOME_EvalExpr_OK ) {
    if ( min >= 0 )  {
      Postfix one;
      list<Postfix> parts;
      list<int>::const_iterator anIt = argmin.begin(), aLast = argmin.end();
      bool ok = sort( post, one, anOpen, aClose, f, *anIt - 1 );
      parts.push_back( one );
      one.clear();
      for ( ; anIt != aLast && ok; anIt++ )      {
        list<int>::const_iterator aNext = anIt; aNext++;
        ok = sort( post, one, anOpen, aClose, *anIt + 1, aNext == aLast ? l : *aNext - 1 );
        parts.push_back( one );
        one.clear();
      }
      //
      if ( !ok ){
        return false;
      }
      //
      stack<PostfixItem> aStack;
      list<Postfix>::const_iterator aPIt = parts.begin();
      list<PostfixItemType>::const_iterator aTIt = min_types.begin();
      //
      append(res,*aPIt);
      aPIt++;
      anIt = argmin.begin();
      for ( ; anIt != aLast; anIt++, aPIt++, aTIt++ )  {
        if ( *aTIt == Pre ) {
          if (!contains(anOpen, at(post,*anIt).myValue.toString())) {
            append(res,*aPIt);
            aStack.push( at(post, *anIt) );
          }
          else {
            res.push_back( at(post, *anIt) );
            append(res,*aPIt);
          }
        }
        else {
          append(res,*aPIt);
          while (!aStack.empty() ) {
            res.push_back( aStack.top() );
            aStack.pop();
          }
          res.push_back( at(post, *anIt) );
        }
      }
      while ( !aStack.empty() ) {
        res.push_back( aStack.top() );
        aStack.pop();
      }
    }
    else { //there are no operations
      for ( int k = f; k <= l; k++ ) {
        const PostfixItem& aItem=at(post, k);
        if ( aItem.myType==Value || aItem.myType == Param ){
          res.push_back( aItem );
        }
      }
    }
  }
  return error() == SALOME_EvalExpr_OK;
}
//=======================================================================
//function : parse
//purpose  : 
//=======================================================================
bool SALOME_EvalParser::parse( const RString& expr )
{
  myPostfix.clear();

  if ( !checkOperations() )
    return false;

  Postfix p;
  RStringList opens, closes;

  setError( SALOME_EvalExpr_OK );
  bracketsList( opens, true );
  bracketsList( closes, false );

  return prepare( expr, p ) && setOperationTypes( p ) && sort( p, myPostfix, opens, closes );
}
//=======================================================================
//function : calculate
//purpose  : 
//=======================================================================
bool SALOME_EvalParser::calculate(const RString& op, 
                              SALOME_EvalVariant& v1, 
                              SALOME_EvalVariant& v2 )
{
  SALOME_EvalExprError aErr;
  SALOME_EvalVariantType aType1, aType2;
  //
  aType1=v1.type();
  aType2=v2.type();
  //
  aErr = isValid( op, aType1, aType2 );
  if ( aErr == SALOME_EvalExpr_OK ){
    aErr=calculation( op, v1, v2 );
    setError( aErr );
  }
  else{
    setError( aErr );
  }
  return error() == SALOME_EvalExpr_OK;
}
//=======================================================================
//function : calculate
//purpose  : 
//=======================================================================
SALOME_EvalVariant SALOME_EvalParser::calculate()
{
  if ( !checkOperations() )
    return SALOME_EvalVariant();

  setError( SALOME_EvalExpr_OK );
  //
  RStringList anOpen, aClose;
  PostfixItemType aType;
  //
  bracketsList( anOpen, true );
  bracketsList( aClose, false );

  stack<SALOME_EvalVariant> aStack;
  Postfix::iterator anIt = myPostfix.begin(), aLast = myPostfix.end();
  for ( ; anIt != aLast && error() == SALOME_EvalExpr_OK; anIt++ )  {
    const PostfixItem& aPostfixItem=*anIt; 
    aType=aPostfixItem.myType;
    RString nn = aPostfixItem.myValue.toString();
    //
    if ( aType == Param ) {
      if ( hasParameter( nn ) )  {
        SALOME_EvalVariant& v = myParams[nn];
        if ( v.isValid() ) {
          aStack.push( v );
        }
        else {
          setError( SALOME_EvalExpr_InvalidToken );
        }
      }
      else {
        setError( SALOME_EvalExpr_InvalidToken );
      }
    }
    //
    else if ( aType == Value ) {
      aStack.push( (*anIt).myValue );
    }
    //
    else if ( aType == Pre || aType == Post )  {
      if ( contains(anOpen, nn ) )  {
        SALOME_EvalVariant inv;
        if ( calculate( nn, inv, inv ) ) {
          aStack.push( SALOME_EvalVariant() );
        }
      }
      else if ( contains(aClose, nn ) )  {
        SALOME_ListOfEvalVariant aSet;
        while ( true ) {
          if ( aStack.empty() ) {
            setError( SALOME_EvalExpr_StackUnderflow );
            break;
          }
          if ( aStack.top().isValid() ) {
            aSet.push_back( aStack.top() );
            aStack.pop();
          }
          else {
            aStack.pop();
            break;
          }
        }

        SALOME_EvalVariant qSet = aSet, inv;
        if ( calculate( nn, qSet, inv ) ) {
          aStack.push( aSet );
        }
      }
      else if ( aStack.size() >= 1 )  {
        SALOME_EvalVariant inv;
        SALOME_EvalVariant *v1, *v2;
        v1=&aStack.top(); 
        v2 = &inv; //"post-" case
        if ( aType == Pre ) {
          v2 = &aStack.top();
          v1 = &inv;
        }
        calculate( nn, *v1, *v2 );
      }
      else {
        setError( SALOME_EvalExpr_StackUnderflow );
      }
    }//else if ( aType== Pre || aType == Post )  {
    else if ( aType == Binary )  {
      if ( aStack.size() >= 2 )  {
        SALOME_EvalVariant v2= aStack.top();
        aStack.pop();
        calculate( nn, aStack.top(), v2 );
      }
      else {
        setError( SALOME_EvalExpr_StackUnderflow );
      }
    }
  }
  //
  SALOME_EvalVariant res;
  if ( error() == SALOME_EvalExpr_OK )  {
    int count;
    
    count = (int)aStack.size();
    if ( count == 0 ) {
      setError( SALOME_EvalExpr_StackUnderflow );
    }
    else if( count == 1 ) {
      res = aStack.top();
    }
    else{
      setError( SALOME_EvalExpr_ExcessData );
    }
  }
  return res;
}
//=======================================================================
//function : calculate
//purpose  : 
//=======================================================================
SALOME_EvalVariant SALOME_EvalParser::calculate( const RString& expr )
{
  setExpression( expr );
  return calculate();
}
//=======================================================================
//function : setExpression
//purpose  : 
//=======================================================================
bool SALOME_EvalParser::setExpression( const RString& expr )
{
  return parse( expr );
}
//=======================================================================
//function : hasParameter
//purpose  : 
//=======================================================================
bool SALOME_EvalParser::hasParameter( const RString& name ) const
{
  bool bRet;
  RString aStr;
  ParamMap::const_iterator aIt;
  aStr=trimmed(name);
  aIt=myParams.find(aStr);
  bRet=(aIt!=myParams.end());
  return bRet;
}
//=======================================================================
//function : setParameter
//purpose  : 
//=======================================================================
void SALOME_EvalParser::setParameter( const RString& name, const SALOME_EvalVariant& value )
{
  RString aStr;
  //
  aStr=trimmed(name);
  PairParamMap aPair(aStr, value);
  myParams.insert(aPair);
}
//=======================================================================
//function : removeParameter
//purpose  : 
//=======================================================================
bool SALOME_EvalParser::removeParameter( const RString& name )
{
  int iRet;
  RString aStr;
  //
  aStr=trimmed(name);
  iRet=(int)myParams.erase(aStr);
  return iRet? true : false;
  
}
//=======================================================================
//function : parameter
//purpose  : 
//=======================================================================
SALOME_EvalVariant SALOME_EvalParser::parameter( const RString& theName ) const
{
  SALOME_EvalVariant res;
  ParamMap::const_iterator aIt;
  RString aStr;
  aStr=trimmed(theName);
  aIt=myParams.find(theName);
  if (aIt!=myParams.end()) {
    const PairParamMap& aVT=*aIt;
    const SALOME_EvalVariant& aRV=aVT.second;
    res=aRV;
  }
  return res;
}
//=======================================================================
//function : firstInvalid
//purpose  : 
//=======================================================================
bool SALOME_EvalParser::firstInvalid( RString& name ) const
{
  ParamMap::const_iterator aIt = myParams.begin();
  for (; aIt != myParams.end(); aIt++ )  {
    const PairParamMap& aVT=*aIt;
    const SALOME_EvalVariant& aRV=aVT.second;
    if ( !aRV.isValid() )  {
      name = aVT.first;
      return true;
    }
  }
  return false;
}
//=======================================================================
//function : removeInvalids
//purpose  : 
//=======================================================================
void SALOME_EvalParser::removeInvalids()
{
  ParamMap aM;
  //
  ParamMap::const_iterator aIt = myParams.begin();
  for (; aIt != myParams.end(); aIt++ )  {
    const PairParamMap& aVT=*aIt;
    const SALOME_EvalVariant& aRV=aVT.second;
    if (aRV.isValid() )  {
      aM.insert(aVT);
    }
  }
  myParams.clear();
  myParams=aM;
}
//=======================================================================
//function : error
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalParser::error() const
{
  return myError;
}
//=======================================================================
//function : setError
//purpose  : 
//=======================================================================

void SALOME_EvalParser::setError(SALOME_EvalExprError err)
{
  myError = err;
}
//=======================================================================
//function : dump
//purpose  : 
//=======================================================================
RString SALOME_EvalParser::dump() const
{
  return dump( myPostfix );
}
//=======================================================================
//function : dump
//purpose  : 
//=======================================================================
RString SALOME_EvalParser::dump( const Postfix& post ) const
{
  RString res;
  //
  if ( !checkOperations() ) {
    return res;
  }
  //
  Postfix::const_iterator anIt = post.begin();
  for (; anIt != post.end(); anIt++ )  {
    if ((*anIt).myType == Value && 
        (*anIt).myValue.type() == SALOME_EvalVariant_String ) {
      res += "'" + (*anIt).myValue.toString() + "'";
    }
    else{
      res += (*anIt).myValue.toString();
    }
    if ( (*anIt).myType == Pre ){
      res += "(pre)";
    }
    else if ( (*anIt).myType == Post ){
      res += "(post)";
    }
    else if ( (*anIt).myType == Binary ){
      res += "(bin)";
    }

    res += "_";
  }

  return res;
}
//=======================================================================
//function : parameters
//purpose  : 
//=======================================================================
RStringList SALOME_EvalParser::parameters() const
{
  RStringList lst;
  Postfix::const_iterator anIt = myPostfix.begin();
  for (; anIt != myPostfix.end(); anIt++ )  {
    if ( (*anIt).myType == Param )    {
      RString name = (*anIt).myValue.toString();
      if ( !contains(lst, name ) ) {
        lst.push_back( name );
      }
    }
  }
  return lst;
}
//=======================================================================
//function : clearParameters
//purpose  : 
//=======================================================================
void SALOME_EvalParser::clearParameters()
{
  myParams.clear();
}
//=======================================================================
//function : toString
//purpose  : 
//=======================================================================
RString SALOME_EvalParser::toString( const SALOME_ListOfEvalVariant& theList )
{
  RString res = "set : [ ";
  SALOME_ListOfEvalVariant::const_iterator aIt = theList.begin();
  for ( ; aIt != theList.end(); aIt++ )
    res += (*aIt).toString() + " ";
  res += "]";
  return res;
}
//=======================================================================
//function : operationList
//purpose  : 
//=======================================================================
void SALOME_EvalParser::operationList( RStringList& theList ) const
{
  SALOME_ListOfPEvalSet::const_iterator aIt = mySets.begin();
  for (; aIt != mySets.end(); ++aIt )  {
    RStringList custom;
    SALOME_EvalSet* aSet = *aIt;
    aSet->operationList( custom );
    RStringList::const_iterator sIt = custom.begin();
    for ( ; sIt != custom.end(); ++sIt )    {
      if ( !contains(theList, *sIt ) ) {
        theList.push_back( *sIt );
      }
    }
  }
}
//=======================================================================
//function : operationList
//purpose  : 
//=======================================================================
void SALOME_EvalParser::bracketsList(RStringList& theList, 
                                 bool open ) const
{ 
  SALOME_ListOfPEvalSet::const_iterator it = mySets.begin();
  for (; it != mySets.end(); ++it ) {
    RStringList custom;
    SALOME_EvalSet* aSet = *it;
    aSet->bracketsList( custom, open );
    RStringList::const_iterator sIt = custom.begin(); 
    for (; sIt != custom.end(); ++sIt )    {
      if ( !contains(theList, *sIt ) )
        theList.push_back( *sIt );
    }
  }
}
//=======================================================================
//function : createValue
//purpose  : 
//=======================================================================
bool SALOME_EvalParser::createValue(const RString& str, 
                                SALOME_EvalVariant& val) const
{
  bool ok = false;
  //
  SALOME_ListOfPEvalSet::const_iterator it = mySets.begin(); 
  for (; it != mySets.end() && !ok; ++it ) {
    ok = (*it)->createValue( str, val );
  }
  return ok;
}
//=======================================================================
//function : priority
//purpose  : 
//=======================================================================
int SALOME_EvalParser::priority(const RString& op, 
                            bool isBin ) const
{
  int i = 0;
  int priority = 0;
  SALOME_ListOfPEvalSet::const_iterator it = mySets.begin();
  for (; it != mySets.end() && priority <= 0; ++it, i++ ){
    priority = (*it)->priority( op, isBin );
  }
  return priority > 0 ? priority + i * 50 : 0;
}
//=======================================================================
//function : isValid
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalParser::isValid(const RString& op,
                                         const SALOME_EvalVariantType t1, 
                                         const SALOME_EvalVariantType t2 ) const
{
  SALOME_EvalExprError err = SALOME_EvalExpr_OK;
  //
  SALOME_ListOfPEvalSet::const_iterator it = mySets.begin();
  for (; it != mySets.end(); ++it )  {
    SALOME_EvalSet *pSet=*it;
    err = pSet->isValid( op, t1, t2 );
    if ( err == SALOME_EvalExpr_OK ){
      break;
    }
  }
  return err;
}
//=======================================================================
//function : calculation
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalParser::calculation(const RString& op, 
                                             SALOME_EvalVariant& v1, 
                                             SALOME_EvalVariant& v2 ) const
{
  SALOME_EvalVariant nv1, nv2;
  SALOME_EvalSet *pSet;
  SALOME_EvalVariantType aType1, aType2;
  SALOME_EvalExprError aErr;
  //
  aType1=v1.type();
  aType2=v2.type();
  //
  SALOME_ListOfPEvalSet::const_iterator aIt = mySets.begin();
  for (; aIt != mySets.end(); ++aIt )  {
    pSet=*aIt;
    nv1 = v1;
    nv2 = v2;
    aErr=pSet->isValid( op, aType1, aType2);
    if ( aErr == SALOME_EvalExpr_OK ) {
      aErr = pSet->calculate( op, nv1, nv2 );
      if ( aErr == SALOME_EvalExpr_OK || aErr == SALOME_EvalExpr_InvalidResult )  {
        v1 = nv1;
        v2 = nv2;
        return aErr;
      }
    }
  }
  //
  return SALOME_EvalExpr_InvalidOperation;
}
//=======================================================================
//function : checkOperations
//purpose  : 
//=======================================================================
bool SALOME_EvalParser::checkOperations() const
{
  if ( mySets.size() )
    return true;

  SALOME_EvalParser* that = (SALOME_EvalParser*)this;
  that->setError( SALOME_EvalExpr_OperationsNull );
  return false;
}
//=======================================================================
//function : append
//purpose  : 
//=======================================================================
void SALOME_EvalParser::append(Postfix& aL,
                           const Postfix& aL1)
{
  Postfix::const_iterator aIt=aL1.begin();
  for(; aIt!=aL1.end(); ++aIt) {
    aL.push_back(*aIt);
  }
}
//=======================================================================
//function : at
//purpose  : 
//=======================================================================
const SALOME_EvalParser::PostfixItem& SALOME_EvalParser::at(const Postfix& aL, 
                                                    const int aIndex)
{
  int i;
  Postfix::const_iterator aIt=aL.begin();
  for(i=0; aIt!=aL.end(); ++aIt, ++i) {
    if (i==aIndex) {
      break;
    }
  }
  return *aIt;
}
//=======================================================================
//function : insert
//purpose  : 
//=======================================================================
void SALOME_EvalParser::insert(Postfix& aL, 
                            const int aIndex,
                            PostfixItem& pS)
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
    Postfix::iterator aIt=aL.begin();
    for(i=0; aIt!=aL.end(); ++aIt, ++i) {
      if (i==aIndex) {
        aL.insert( aIt, pS );
        break;
      }
    }
  }
}
//=======================================================================
//function : indexOf
//purpose  : 
//=======================================================================
int indexOf(const RStringList& aLS,
            const RString& aS)
{
  int i, iRet;
  RStringList::const_iterator aIt;
  //
  iRet=-1;
  //
  aIt=aLS.begin();
  for (i=0; aIt!=aLS.end(); ++aIt, ++i) {
    const RString aSx=*aIt;
    if (aSx==aS) {
      iRet=i;
      break;
    }
  }
  return iRet;
}
//=======================================================================
//function : contains
//purpose  : 
//=======================================================================
bool contains(const RStringList& aLS,
              const RString& aS)
{
  bool bRet;
  //
  bRet=false;
  RStringList::const_iterator aIt;
  //
  aIt=aLS.begin();
  for (; aIt!=aLS.end(); ++aIt) {
    const RString aSx=*aIt;
    if (aSx==aS) {
      bRet=!bRet;
      break;
    }
  }
  return bRet;
}


//=======================================================================
//function : isSpace
//purpose  : 
//=======================================================================
bool isSpace(const char aC)
{
  bool bRet;
  char aWhat[]={
    '\t', '\n', '\v', '\f', '\r', ' '
  };
  int i, aNb;
  //
  bRet=false;
  aNb=sizeof(aWhat)/sizeof(aWhat[0]);
  for (i=0; i<aNb; ++i) {
    bRet=(aC==aWhat[i]);
    if (bRet) {
      break;
    }
  }
  return bRet;
}
//=======================================================================
//function : trimmed
//purpose  : 
//=======================================================================
RString trimmed(const RString& str)
{
  char aWhat[]={
    '\t', '\n', '\v', '\f', '\r', ' '
  };
  int i, aNb;
  RString aRet;
  //
  aRet=str;
  aNb=sizeof(aWhat)/sizeof(aWhat[0]);
  for (i=0; i<aNb; ++i) {
    aRet=trimmed(aRet, aWhat[i]);
  }
  return aRet;
}
//=======================================================================
//function : trimmed
//purpose  : 
//=======================================================================
RString trimmed(const RString& str, const char aWhat)
{
  char aX[2];
  size_t mylength, i;
  RString aRet;
  //  
  const char* mystring=str.c_str();
  if(!mystring) {
    return aRet;
  }
  //  
  mylength=strlen(mystring);
  if (!mylength) {
    return aRet;
  }
  //
  aX[1]=0;
  for (i=0; i<mylength; ++i) {
    char aC=mystring[i];
    if (aC != aWhat) {
      aX[0]=aC;
      aRet.append(aX);
    }
  }
  return aRet;
}


