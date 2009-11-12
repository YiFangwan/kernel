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
//  File   : SALOME_EvalParser.hxx
//  Author : Peter KURNEV
//  Module : SALOME

#ifndef SALOME_EvalParser_Header_File
#define SALOME_EvalParser_Header_File
//
#ifdef WNT
#pragma warning(disable : 4786)
#endif
//
#include <list>
#include <map>
//
#include <SALOME_EvalVariant.hxx>
#include <SALOME_EvalSet.hxx>
#include <SALOME_Eval.hxx>

class SALOME_EvalParser
{
public:
  SALOME_EvalParser();
  virtual ~SALOME_EvalParser();

  SALOME_EvalVariant           calculate();
  SALOME_EvalVariant           calculate( const RString& );
  bool               setExpression( const RString& );

  SALOME_ListOfPEvalSet  operationSets() const;
  SALOME_PEvalSet        operationSet( const RString& ) const;
  void               removeOperationSet(SALOME_PEvalSet );
  void               insertOperationSet(SALOME_PEvalSet, 
                                        const int = -1 );

  bool               autoDeleteOperationSets() const;
  void               setAutoDeleteOperationSets( const bool );

  virtual void       clearParameters();
  virtual bool       removeParameter( const RString& name );
  virtual SALOME_EvalVariant   parameter( const RString& name ) const;
  virtual bool       hasParameter( const RString& name ) const;
  virtual void       setParameter( const RString& name, 
                                   const SALOME_EvalVariant& value );
  RStringList        parameters() const;

  SALOME_EvalExprError error() const;

  bool               firstInvalid( RString& ) const;
  void               removeInvalids();
  RString            dump() const;

  static RString     toString( const SALOME_ListOfEvalVariant& );

protected:
  //! Types of postfix representation elements
  typedef enum  {
    Value, //!< Value (number, string, etc.)
    Param, //!< Parameter
    Open,  //!< Open bracket
    Close, //!< Close bracket
    Pre,   //!< Unary prefix operation
    Post,  //!< Unary postfix operation
    Binary //!< Binary operation
  } PostfixItemType;

  //! Postfix representation element
  typedef struct  {
    SALOME_EvalVariant          myValue;
    PostfixItemType   myType;
  } PostfixItem;

  typedef list<PostfixItem>       Postfix;   //!< postfix representation
  typedef SALOME_ListOfPEvalSet       SetList;   //!< list of operations
  typedef map <RString, SALOME_EvalVariant> ParamMap;  //!< parameter-to-value map
  typedef map <RString, SALOME_EvalVariant>::value_type PairParamMap;

protected:
  RString            dump( const Postfix& ) const;

  virtual bool       prepare( const RString&, Postfix& );

  virtual bool       setOperationTypes( Postfix& );

  virtual bool       sort(const Postfix&, 
                          Postfix&, 
                          const RStringList&,
                          const RStringList&, 
                          int f = -1, 
                          int l = -1 );

  virtual bool       parse( const RString& );
  virtual void       setError( const SALOME_EvalExprError );

  bool               calculate(const RString&, 
                               SALOME_EvalVariant&, 
                               SALOME_EvalVariant& );

  static int         search(const RStringList&, 
                            const RString&,
                            int offset, 
                            int& matchLen, 
                            int& listind );

  static RString     note(const RString& str, 
                          int pos, 
                          int len );

  static int         globalBrackets(const Postfix&, 
                                    int, 
                                    int );

private:
  void               operationList(RStringList& ) const;

  void               bracketsList(RStringList&, 
                                  bool ) const;

  bool               createValue(const RString&, 
                                 SALOME_EvalVariant& ) const;

  int                priority(const RString&, 
                              bool isBin ) const;

  SALOME_EvalExprError isValid(const RString&,
                            const SALOME_EvalVariantType, 
                            const SALOME_EvalVariantType ) const;

  SALOME_EvalExprError calculation(const RString&, 
                                 SALOME_EvalVariant&, 
                                 SALOME_EvalVariant& ) const;

  bool               checkOperations() const;
  ////////////////////////////////////
  void insert(Postfix& aL, 
            const int aIndex,
            PostfixItem& aItem);

  const PostfixItem& at(const Postfix& aL, 
                        const int aIndex);

  void SALOME_EvalParser::append(Postfix& aL,
                           const Postfix& aL1);
private:
  SALOME_ListOfPEvalSet mySets;
  SALOME_EvalExprError myError;
  ParamMap          myParams;
  Postfix           myPostfix;
  bool              myAutoDel;
};

#endif
