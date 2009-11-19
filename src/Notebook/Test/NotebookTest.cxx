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
#include "NotebookTest.hxx"
//
#include <stdio.h>
//
#include <SALOME_Eval.hxx>
#include <SALOME_EvalVariant.hxx>
#include <SALOME_EvalExpr.hxx>
#include <SALOME_EvalParser.hxx>
//
using namespace std;
//
// ============================================================================
/*!
 * Set up the environment
 */
// ============================================================================
void NotebookTest::setUp()
{
}
// ============================================================================
/*!
 *  - delete trace classes
 */
// ============================================================================
void NotebookTest::tearDown()
{
}
// ============================================================================
/*!
 * Check expressions with int values
 */
// ============================================================================
void NotebookTest::testParameterInt()
{
  const int aNb=6;
  bool bOk;
  int j;
  int aResult[aNb], aX;
  SALOME_String aExp[aNb];
  SALOME_String aA, aB, aC;
  SALOME_EvalVariant aRVR, aRVA, aRVC, aRVB;
  SALOME_EvalVariantType aType;
  SALOME_EvalExprError aErr;
  //
  aExp[0]="a+b";
  aExp[1]="(a+b)/7";
  aExp[2]="((a+b)/7)*10";
  aExp[3]="-((a+b)/7)+a*b/8";
  aExp[4]="-((a+c)/4+(-b-c)*2)/2";
  aExp[5]="a*(a*(a*(a*(a*(a+1)+1)+1)+1)+1)";
  //
  aResult[0]=-7;
  aResult[1]=-1;
  aResult[2]=-10;
  aResult[3]=0;
  aResult[4]=5;
  aResult[5]=6;
  //
  aA="a";
  aB="b";
  aC="c";
  //
  aRVA=1;
  aRVB=-8;
  aRVC=15;
  //
  SALOME_EvalExpr aEvalExpr(aExp[0]);
  //
  SALOME_EvalParser* aParser=aEvalExpr.parser();
  //
  aParser->setParameter(aA, aRVA);
  aParser->setParameter(aB, aRVB);
  aParser->setParameter(aC, aRVC);
  //
  for (j=0; j<aNb; ++j) {
    aEvalExpr.setExpression(aExp[j]);
    //
    aRVR=aEvalExpr.calculate();
    //
    aErr=aEvalExpr.error();
    CPPUNIT_ASSERT(aErr==EvalExpr_OK);
    //
    aType=aRVR.type();
    CPPUNIT_ASSERT(aType==SALOME_EvalVariant_Int);

    aX=aRVR.toInt(&bOk);
    CPPUNIT_ASSERT(aX==aResult[j]);
  }
}
// ============================================================================
/*!
 * Check expressions with double values
 */
// ============================================================================
void NotebookTest::testParameterDouble()
{
  const int aNb=6;
  bool bOk;
  int j;
  double aResult[aNb], aX;
  SALOME_String aExp[aNb];
  SALOME_String aA, aB, aC;
  SALOME_EvalVariant aRVR, aRVA, aRVC, aRVB;
  SALOME_EvalVariantType aType;
  SALOME_EvalExprError aErr;
  //
  aExp[0]="a+b";
  aExp[1]="(a+b)/7";
  aExp[2]="((a+b)/7)*10.";
  aExp[3]="-((a+b)/7)+a*b/8.";
  aExp[4]="(-b+sqrt(b*b-4.*a*c))/(2.*a)";
  aExp[5]="a*(a*(a*(a*(a*(a+1.1)+1.1)+1.1)+1.1)+1.1)";
  //.1
  aResult[0]=-7.;
  aResult[1]=-1.;
  aResult[2]=-10.;
  aResult[3]=0.;
  aResult[4]=5.;
  aResult[5]=6.5;
  //
  aA="a";
  aB="b";
  aC="c";
  //
  aRVA=1.;
  aRVB=-8;
  aRVC=15.;
  //
  SALOME_EvalExpr aEvalExpr(aExp[0]);
  //
  SALOME_EvalParser* aParser=aEvalExpr.parser();
  //
  aParser->setParameter(aA, aRVA);
  aParser->setParameter(aB, aRVB);
  aParser->setParameter(aC, aRVC);
  //
  for (j=0; j<aNb; ++j) {
    aEvalExpr.setExpression(aExp[j]);
    //
    aRVR=aEvalExpr.calculate();
    aErr=aEvalExpr.error();
    //
    // 1. Check the result on validity
    CPPUNIT_ASSERT(aErr==EvalExpr_OK);
    //
    // 2. Check the type of the result
    aType=aRVR.type();
    CPPUNIT_ASSERT(aType==SALOME_EvalVariant_Double);
    //
    // 3. Check the contents of the result
    aX=aRVR.toDouble(&bOk);
    CPPUNIT_ASSERT(aX==aResult[j]);
  }
}
// ============================================================================
/*!
 * Check expressions with string values
 */
// ============================================================================
void NotebookTest::testParameterString()
{
  const int aNb=8;
  //bool bOk;
  int j;
  SALOME_String aResult[aNb], aX;
  SALOME_String aExp[aNb];
  SALOME_String aA, aB, aC, aD;
  SALOME_EvalVariant aRVR, aRVA, aRVC, aRVB, aRVD;
  SALOME_EvalVariantType aType, aTypeR[aNb];
  SALOME_EvalExprError aErr;
  //
  aExp[0]="a+b+c";
  aExp[1]="upper(a+b+c)";
  aExp[2]="lower(a+b+c)";
  aExp[3]="length(a+b+c)";
  aExp[4]="(a+b+c)=d";
  aExp[5]="lower(a+b+c)=upper(d)";
  aExp[6]="a+b+c<d";
  aExp[7]="a+b+c>=d";
  //
  aResult[0]="The quick brown fox jumps over a lazy dog";
  aResult[1]="THE QUICK BROWN FOX JUMPS OVER A LAZY DOG";
  aResult[2]="the quick brown fox jumps over a lazy dog";
  aResult[3]="41";
  aResult[4]="true";
  aResult[5]="false";
  aResult[6]="false";
  aResult[7]="true";
  //
  aTypeR[0]=SALOME_EvalVariant_String;
  aTypeR[1]=SALOME_EvalVariant_String;
  aTypeR[2]=SALOME_EvalVariant_String;
  aTypeR[3]=SALOME_EvalVariant_Int;
  aTypeR[4]=SALOME_EvalVariant_Boolean;
  aTypeR[5]=SALOME_EvalVariant_Boolean;
  aTypeR[6]=SALOME_EvalVariant_Boolean;
  aTypeR[7]=SALOME_EvalVariant_Boolean;
  //
  aA="a";
  aB="b";
  aC="c";
  aD="d";
  //
  aRVA="The quick brown fo";
  aRVB="x jumps ove";
  aRVC="r a lazy dog";
  aRVD=aResult[0];
  //
  SALOME_EvalExpr aEvalExpr(aExp[0]);
  //
  SALOME_EvalParser* aParser=aEvalExpr.parser();
  //
  aParser->setParameter(aA, aRVA);
  aParser->setParameter(aB, aRVB);
  aParser->setParameter(aC, aRVC);
  aParser->setParameter(aD, aRVD);
  //
  for (j=0; j<aNb; ++j) {
    aEvalExpr.setExpression(aExp[j]);
    //
    aRVR=aEvalExpr.calculate(); 
    aErr=aEvalExpr.error();
    //
    // 1. Check the result on validity
    CPPUNIT_ASSERT(aErr==EvalExpr_OK);
    //
    // 2. Check the type of the result
    aType=aRVR.type();
    CPPUNIT_ASSERT(aType==aTypeR[j]);
    //
    // 3. Check the contents of the result
    aX=aRVR.toString();
    CPPUNIT_ASSERT(aX==aResult[j]);
  }
}
// ============================================================================
/*!
 * Check expressions with booleanvalues
 */
// ============================================================================
void NotebookTest::testParameterBoolean()
{
  const int aNb=4;
  int j;
  bool aResult[aNb], aX;
  SALOME_String aExp[aNb];
  SALOME_String aA, aB, aC, aD;
  SALOME_EvalVariant aRVR, aRVA, aRVC, aRVB, aRVD;
  SALOME_EvalVariantType aType, aTypeR[aNb];
  SALOME_EvalExprError aErr;
  //
  aExp[0]="a&&b";
  aExp[1]="a||c";
  aExp[2]="!c&&!d";
  aExp[3]="!(a&&(!c&&!d)||b)";
  //
  aResult[0]=true;
  aResult[1]=true;
  aResult[2]=true;
  aResult[3]=false;
  //
  aTypeR[0]=SALOME_EvalVariant_Boolean;
  aTypeR[1]=SALOME_EvalVariant_Boolean;
  aTypeR[2]=SALOME_EvalVariant_Boolean;
  aTypeR[3]=SALOME_EvalVariant_Boolean;
  //
  aA="a";
  aB="b";
  aC="c";
  aD="d";
  //
  aRVA=true;
  aRVB=true;
  aRVC=false;
  aRVD=false;
  //
  SALOME_EvalExpr aEvalExpr(aExp[0]);
  //
  SALOME_EvalParser* aParser=aEvalExpr.parser();
  //
  aParser->setParameter(aA, aRVA);
  aParser->setParameter(aB, aRVB);
  aParser->setParameter(aC, aRVC);
  aParser->setParameter(aD, aRVD);
  //
  for (j=0; j<aNb; ++j) {
    aEvalExpr.setExpression(aExp[j]);
    //
    aRVR=aEvalExpr.calculate(); 
    aErr=aEvalExpr.error();
    //
    // 1. Check the result on validity
    CPPUNIT_ASSERT(aErr==EvalExpr_OK);
    //
    // 2. Check the type of the result
    aType=aRVR.type();
    CPPUNIT_ASSERT(aType==aTypeR[j]);
    //
    // 3. Check the contents of the result
    aX=aRVR.toBool();
    CPPUNIT_ASSERT(aX==aResult[j]);
  }
}
