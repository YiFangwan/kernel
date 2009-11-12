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
//  File   : SALOME_Eval.hxx
//  Author : Peter KURNEV
//  Module : SALOME


#ifndef SALOME_Eval_Header_File
#define SALOME_Eval_Header_File

#ifdef WNT
#pragma warning(disable : 4786)
#endif

#include <string>
#include <list>
#include <set>
//
using namespace std;

typedef string RString;
typedef list<string> RStringList;
typedef RStringList::iterator  ListIteratorOfRStringList;
typedef set<RString> SetOfRString;
typedef SetOfRString::iterator SetIteratorOfSetOfRString;
typedef pair< SetIteratorOfSetOfRString, bool > SetPair;

typedef enum
  {
    SALOME_EvalExpr_OK,               //!< No errors found
    SALOME_EvalExpr_OperandsNotMatch, //!< Types of arguments are invalid for this operation
    SALOME_EvalExpr_InvalidResult,    //!< Operation cannot find result (for example, division by zero)
    SALOME_EvalExpr_InvalidOperation, //!< Unknown operation
    SALOME_EvalExpr_OperationsNull,   //!< Internal operations pointer of parser is null
    SALOME_EvalExpr_InvalidToken,     //!< Invalid token (neither operation, nor parameter of value)
    SALOME_EvalExpr_CloseExpected,    //!< Closing bracket is expected
    SALOME_EvalExpr_ExcessClose,      //!< Extra closing bracket is found
    SALOME_EvalExpr_BracketsNotMatch, //!< Opening and closing brackets are of different type, e.g. [)
    SALOME_EvalExpr_StackUnderflow,   //!< There are no arguments in the stack for the operation
    SALOME_EvalExpr_ExcessData        //!< The parsing is finished, but there are more then one value in the stack
  } SALOME_EvalExprError;

#endif
