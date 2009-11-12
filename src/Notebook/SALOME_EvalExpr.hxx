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
//  File   : SALOME_EvalExpr.hxx
//  Author : Peter KURNEV
//  Module : SALOME

#ifndef SALOME_EvalExpr_Header_File
#define SALOME_EvalExpr_Header_File

#include <SALOME_Eval.hxx>
#include <SALOME_EvalVariant.hxx>
#include <SALOME_EvalParser.hxx>
#include <SALOME_EvalSet.hxx>

class SALOME_EvalExpr
{
public:
  SALOME_EvalExpr(const RString& = RString());
  SALOME_EvalExpr( const bool, const RString& = RString() );
  ~SALOME_EvalExpr();

  SALOME_EvalVariant           calculate( const RString& = RString() );

  RString            expression() const;
  void               setExpression( const RString& );

  SALOME_EvalExprError              error() const;
  SALOME_EvalParser*     parser() const;

  bool               autoDeleteOperationSets() const;
  void               setAutoDeleteOperationSets( const bool );

  SALOME_ListOfPEvalSet  operationSets() const;
  SALOME_EvalSet*        operationSet( const RString& ) const;
  void               removeOperationSet(SALOME_EvalSet* );
  void               insertOperationSet(SALOME_EvalSet*, 
                                        const int = -1 );

private:
  void               intialize(const bool, 
                               const RString& );

private:
  RString            myExpr;
  SALOME_EvalParser*     myParser;
};

#endif
