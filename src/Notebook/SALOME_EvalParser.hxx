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

#ifdef WNT
#pragma warning(disable : 4786)
#endif

#include <list>
#include <map>
#include <SALOME_EvalVariant.hxx>
#include <SALOME_EvalSet.hxx>
#include <SALOME_Eval.hxx>

class SALOME_EvalParser
{
public:
  SALOME_EvalParser();
  virtual ~SALOME_EvalParser();

  bool                       setExpression ( const SALOME_String& theExpr );
  SALOME_EvalExprError       error         () const;
  SALOME_EvalVariant         calculate     ();
  SALOME_EvalVariant         calculate     ( const SALOME_String& theExpr );
  void                       substitute    ( const SALOME_String& theParamName, SALOME_EvalParser* theNewExpr );
  SALOME_String              reverseBuild  () const;
  bool                       firstInvalid  ( SALOME_String& theItem ) const;
  void                       removeInvalids();

  SALOME_ListOfEvalSet       operationSets             () const;
  SALOME_EvalSet*            operationSet              ( const SALOME_String& theName ) const;
  void                       removeOperationSet        ( SALOME_EvalSet* theSet );
  void                       insertOperationSet        ( SALOME_EvalSet* theSet, const int theIndex = -1 );
  bool                       autoDeleteOperationSets   () const;
  void                       setAutoDeleteOperationSets( const bool isAutoDel );

  SALOME_String              dump() const;

  virtual void               clearParameters();
  virtual bool               removeParameter( const SALOME_String& theName );
  virtual SALOME_EvalVariant parameter      ( const SALOME_String& theName ) const;
  virtual bool               hasParameter   ( const SALOME_String& theName ) const;
  virtual void               setParameter   ( const SALOME_String& theName, const SALOME_EvalVariant& theValue );
  SALOME_StringList          parameters     () const;
  bool                       isMonoParam    () const;

  static SALOME_String       toString( const SALOME_ListOfEvalVariant& theList );

protected:
  //! Types of postfix representation elements
  typedef enum 
  {
    Value, //!< Value (number, string, etc.)
    Param, //!< Parameter
    Open,  //!< Open bracket
    Close, //!< Close bracket
    Pre,   //!< Unary prefix operation
    Post,  //!< Unary postfix operation
    Binary //!< Binary operation
  } PostfixItemType;

  //! Postfix representation element
  typedef struct 
  {
    SALOME_EvalVariant myValue;
    PostfixItemType    myType;
  } PostfixItem;

  typedef list<PostfixItem>    Postfix;   //!< postfix representation
  typedef SALOME_ListOfEvalSet SetList;   //!< list of operations
  typedef map <SALOME_String, SALOME_EvalVariant> ParamMap;  //!< parameter-to-value map
  typedef map <SALOME_String, SALOME_EvalVariant>::value_type PairParamMap;

protected:
  SALOME_String dump( const Postfix& ) const;

  virtual bool   prepare          ( const SALOME_String& theExpr, Postfix& thePostfix );
  virtual bool   setOperationTypes( Postfix& thePostfix );
  virtual bool   sort             ( const Postfix& theInitPostfix, Postfix& theResPostfix, const SALOME_StringList& theOpen,
                                    const SALOME_StringList& theClose, int theFirst = -1, int theLast = -1 );
  virtual bool   parse            ( const SALOME_String& theExpr );
  virtual void   setError         ( const SALOME_EvalExprError theErr );

         bool          calculate     ( const SALOME_String& theExpr, SALOME_EvalVariant& theArg1, SALOME_EvalVariant& theArg2 );
  static int           search        ( const SALOME_StringList& theList, const SALOME_String& theItem, int theOffset,
                                       int& theMatchLen, int& theIndex );
  static SALOME_String note          ( const SALOME_String& theStr, int thePos, int theLen );
  static int           globalBrackets( const Postfix& thePostfix, int theFirst, int theLast );

private:
  void                 operationList  ( SALOME_StringList& theRes ) const;
  void                 bracketsList   ( SALOME_StringList& theOpens, SALOME_StringList& theCloses ) const;
  bool                 createValue    ( const SALOME_String& theStr, SALOME_EvalVariant& theRes ) const;
  int                  priority       ( const SALOME_String& theOp, bool isBin ) const;
  SALOME_EvalExprError isValid        ( const SALOME_String& theOp, SALOME_EvalVariantType theArg1, SALOME_EvalVariantType theArg2 ) const;
  SALOME_EvalExprError calculation    ( const SALOME_String& theOp, SALOME_EvalVariant& theArg1, SALOME_EvalVariant& theArg2 ) const;
  bool                 checkOperations() const;

  static void               insert( Postfix& thePostfix, const int theIndex, const PostfixItem& theItem );
  static const PostfixItem& at    ( const Postfix& thePostfix, const int theIndex );
  static void               append( Postfix& thePostfix, const Postfix& theAppendedPostfix );

private:
  friend class RebultExprItem;

  SALOME_ListOfEvalSet  mySets;
  SALOME_EvalExprError  myError;
  ParamMap              myParams;
  Postfix               myPostfix;
  bool                  myAutoDel;
};

#endif
