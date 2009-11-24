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
//  File   : SALOME_EvalExpr.cxx
//  Author : Peter KURNEV
//  Module : SALOME



#include <SALOME_EvalExpr.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
SALOME_EvalExpr::SALOME_EvalExpr( const SALOME_String& expr )
{
  myParser=NULL;
  initialize( true, expr );
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
SALOME_EvalExpr::SALOME_EvalExpr( const bool stdSets, const SALOME_String& expr )
{
  myParser=NULL;
  initialize( stdSets, expr );
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
SALOME_EvalExpr::~SALOME_EvalExpr()
{
  if (myParser) {
    delete myParser;
  }
}
//=======================================================================
//function : error
//purpose  : 
//=======================================================================
SALOME_EvalExprError SALOME_EvalExpr::error() const
{
  return myParser->error();
}
//=======================================================================
//function : initialize
//purpose  : 
//=======================================================================
void SALOME_EvalExpr::initialize( const bool stdSets, const SALOME_String& expr )
{
  if (myParser) {
    delete myParser;
  }

  myParser = new SALOME_EvalParser();
  if ( stdSets )  {
    myParser->setAutoDeleteOperationSets( true );
    myParser->insertOperationSet( new SALOME_EvalSetLogic() );
    myParser->insertOperationSet( new SALOME_EvalSetArithmetic() );
    myParser->insertOperationSet( new SALOME_EvalSetString() );
    myParser->insertOperationSet( new SALOME_EvalSetMath() );
    myParser->insertOperationSet( new SALOME_EvalSetSets() );
    myParser->insertOperationSet( new SALOME_EvalSetConst() );
  }
  setExpression( expr );
}
//=======================================================================
//function : calculate
//purpose  : 
//=======================================================================
SALOME_EvalVariant SALOME_EvalExpr::calculate(const SALOME_String& expr)
{
  if ( !expr.empty() ) {
    setExpression( expr );
  }
  return myParser->calculate();
}
//=======================================================================
//function : expression
//purpose  : 
//=======================================================================
SALOME_String SALOME_EvalExpr::expression() const
{
  return myExpr;
}
//=======================================================================
//function : setExpression
//purpose  : 
//=======================================================================
void SALOME_EvalExpr::setExpression(const SALOME_String& expr)
{
  if ( expr == expression() ){
    return;
  }
  myExpr = expr;
  myParser->setExpression( myExpr );
}
//=======================================================================
//function : parser
//purpose  : 
//=======================================================================
SALOME_EvalParser* SALOME_EvalExpr::parser() const
{
  return myParser;
}

//=======================================================================
//function : operationSets
//purpose  : 
//=======================================================================
SALOME_ListOfEvalSet SALOME_EvalExpr::operationSets() const
{
  return myParser->operationSets();
}
//=======================================================================
//function : insertOperationSet
//purpose  : 
//=======================================================================
void SALOME_EvalExpr::insertOperationSet(SALOME_EvalSet* theSet, const int idx)
{
  myParser->insertOperationSet(theSet, idx);
}
//=======================================================================
//function : removeOperationSet
//purpose  : 
//=======================================================================
void SALOME_EvalExpr::removeOperationSet(SALOME_EvalSet* theSet)
{
  myParser->removeOperationSet(theSet);
}
//=======================================================================
//function : operationSet
//purpose  : 
//=======================================================================
SALOME_EvalSet* SALOME_EvalExpr::operationSet(const SALOME_String& name) const
{
  return myParser->operationSet( name );
}
//=======================================================================
//function : autoDeleteOperationSets
//purpose  : 
//=======================================================================
bool SALOME_EvalExpr::autoDeleteOperationSets() const
{
  return myParser->autoDeleteOperationSets();
}

//=======================================================================
//function : setAutoDeleteOperationSets
//purpose  : 
//=======================================================================
void SALOME_EvalExpr::setAutoDeleteOperationSets(const bool on)
{
  myParser->setAutoDeleteOperationSets( on );
}

//=======================================================================
//function : substitute
//purpose  : 
//=======================================================================
void SALOME_EvalExpr::substitute( const SALOME_String& theParamName, const SALOME_EvalExpr& theExpr )
{
  myParser->substitute( theParamName, theExpr.parser() );
  myExpr = myParser->rebuildExpression();
}
