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

typedef std::string                 SALOME_String;
typedef std::list<SALOME_String>    SALOME_StringList;
typedef SALOME_StringList::iterator SALOME_ListIteratorOfStringList;
typedef std::set<SALOME_String>     SALOME_StringSet;
typedef SALOME_StringSet::iterator  SALOME_SetIteratorOfStringSet;
typedef std::pair< SALOME_SetIteratorOfStringSet, bool > SALOME_SetPair;
  
typedef enum
{
  EvalExpr_OK,               //!< No errors found
  EvalExpr_OperandsNotMatch, //!< Types of arguments are invalid for this operation
  EvalExpr_InvalidResult,    //!< Operation cannot find result (for example, division by zero)
  EvalExpr_InvalidOperation, //!< Unknown operation
  EvalExpr_OperationsNull,   //!< Internal operations pointer of parser is null
  EvalExpr_InvalidToken,     //!< Invalid token (neither operation, nor parameter of value)
  EvalExpr_CloseExpected,    //!< Closing bracket is expected
  EvalExpr_ExcessClose,      //!< Extra closing bracket is found
  EvalExpr_BracketsNotMatch, //!< Opening and closing brackets are of different type, e.g. [)
  EvalExpr_StackUnderflow,   //!< There are no arguments in the stack for the operation
  EvalExpr_ExcessData        //!< The parsing is finished, but there are more then one value in the stack
} SALOME_EvalExprError;

#endif
