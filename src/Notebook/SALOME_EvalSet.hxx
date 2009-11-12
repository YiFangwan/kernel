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
//  File   : SALOME_EvalSet.hxx
//  Author : Peter KURNEV
//  Module : SALOME


#ifndef SALOME_EvalSet_Header_File
#define SALOME_EvalSet_Header_File
//
#ifdef WNT
#pragma warning(disable : 4786)
#endif
//
#include <string>
#include <list>
#include <SALOME_EvalVariant.hxx>
//
using namespace std;
//
class SALOME_EvalSet;
typedef SALOME_EvalSet* SALOME_PEvalSet;
typedef list<SALOME_EvalSet*> SALOME_ListOfPEvalSet;

//=======================================================================
//class : SALOME_EvalSet
//purpose  : 
//=======================================================================
class SALOME_EvalSet
{
public:
  SALOME_EvalSet();
  //
  virtual ~SALOME_EvalSet();
  //
  virtual RString            name() const = 0;
  //
  virtual void               operationList( RStringList& ) const = 0;
  //
  virtual void               bracketsList(RStringList&, 
                                          bool open ) const = 0;
  //
  virtual bool               createValue(const RString&, 
                                          SALOME_EvalVariant& ) const;
  //
  virtual int                priority( const RString&, bool isBin ) const = 0;
  //
  virtual SALOME_EvalExprError  isValid(const RString&, 
                                     const SALOME_EvalVariantType,
                                     const SALOME_EvalVariantType) const = 0;

  virtual SALOME_EvalExprError  calculate(const RString&, 
                                       SALOME_EvalVariant&, 
                                       SALOME_EvalVariant&) const = 0;
  //
  static bool contains(const SALOME_ListOfPEvalSet&, 
                       const SALOME_EvalSet*);
  //
  static void insert  (SALOME_ListOfPEvalSet& aL, 
                       const int aIndex,
                       SALOME_EvalSet* pS);
  //
};
//

//
//=======================================================================
//class : SALOME_EvalSetBase
//purpose  : 
//=======================================================================
class SALOME_EvalSetBase : public SALOME_EvalSet
{
public:
  SALOME_EvalSetBase();
  //
  virtual ~SALOME_EvalSetBase();
  //
  virtual void               operationList( RStringList& ) const;
  virtual void               bracketsList ( RStringList&, bool open ) const;

  virtual 
    SALOME_EvalExprError     isValid(const RString&, 
                             const SALOME_EvalVariantType a,
                             const SALOME_EvalVariantType b) const;

  

protected: 
  void                       addTypes(const SALOME_ListOfEvalVariantType&);
  void                       addOperations(const RStringList& );

private:
  RStringList                myOpers;
  SALOME_ListOfEvalVariantType         myTypes;
};
//=======================================================================
//class : SALOME_EvalSetArithmetic
//purpose  : 
//=======================================================================
class SALOME_EvalSetArithmetic : public SALOME_EvalSetBase
{
public:
  SALOME_EvalSetArithmetic();
  virtual ~SALOME_EvalSetArithmetic();

  virtual bool               createValue(const RString&, 
                                         SALOME_EvalVariant& ) const;

  virtual int                priority(const RString&, 
                                      bool isBin ) const;

  virtual SALOME_EvalExprError calculate(const RString&, 
                                      SALOME_EvalVariant&, 
                                      SALOME_EvalVariant& ) const;

  static RString             Name();
  virtual RString            name() const;
};
//=======================================================================
//class : SALOME_EvalSetLogic
//purpose  : 
//=======================================================================
class SALOME_EvalSetLogic : public SALOME_EvalSetBase
{
public:
  SALOME_EvalSetLogic();

  virtual ~SALOME_EvalSetLogic();

  virtual bool               createValue(const RString&, 
                                         SALOME_EvalVariant& ) const;

  virtual int                priority(const RString&, 
                                      bool isBin ) const;

  virtual SALOME_EvalExprError calculate(const RString&, 
                                      SALOME_EvalVariant&, 
                                      SALOME_EvalVariant& ) const;

  static RString             Name();

  virtual RString            name() const;

private:
  int                        intValue(const SALOME_EvalVariant& ) const;
};
//=======================================================================
//class : SALOME_EvalSetMath
//purpose  : 
//=======================================================================
class SALOME_EvalSetMath : public SALOME_EvalSetBase
{
public:
  SALOME_EvalSetMath();
  virtual ~SALOME_EvalSetMath();

  virtual bool               createValue(const RString&, 
                                         SALOME_EvalVariant&) const;

  virtual int                priority   (const RString&, 
                                         bool isBin) const;

  virtual SALOME_EvalExprError  calculate  (const RString&, 
    SALOME_EvalVariant&, 
    SALOME_EvalVariant& ) const;

  static RString             Name();
  virtual RString            name() const;
};
//=======================================================================
//class : SALOME_EvalSetString
//purpose  : 
//=======================================================================
class SALOME_EvalSetString : public SALOME_EvalSetBase
{
public:
  SALOME_EvalSetString();
  virtual ~SALOME_EvalSetString();

  virtual bool              createValue(const RString&, 
    SALOME_EvalVariant& ) const;

  virtual int               priority( const RString&, 
    bool isBin ) const;

  virtual SALOME_EvalExprError calculate( const RString&, 
    SALOME_EvalVariant&, 
    SALOME_EvalVariant& ) const;

  static RString             Name();
  virtual RString            name() const;
};
//=======================================================================
//class : SALOME_EvalSetSets
//purpose  : 
//=======================================================================
class SALOME_EvalSetSets : public SALOME_EvalSetBase
{
public:
  typedef SALOME_ListOfEvalVariant  ValueSet;

public:
  SALOME_EvalSetSets();
  virtual ~SALOME_EvalSetSets();
  //
  static RString             Name();
  virtual RString            name() const;
  //
  virtual void bracketsList(RStringList&, bool open ) const;
  //
  virtual int priority(const RString&, bool isBin ) const;

  virtual SALOME_EvalExprError isValid(const RString&, 
                                    const SALOME_EvalVariantType,
                                    const SALOME_EvalVariantType) const;

  virtual SALOME_EvalExprError calculate(const RString&, 
                                      SALOME_EvalVariant&, 
                                      SALOME_EvalVariant& ) const;

  static void add   (ValueSet&, const SALOME_EvalVariant& );
  static void add   (ValueSet&, const ValueSet& );
  static void remove(ValueSet&, const SALOME_EvalVariant& );
  static void remove(ValueSet&, const ValueSet& );

};
//=======================================================================
//class : SALOME_EvalSetConst
//purpose  : 
//=======================================================================
class SALOME_EvalSetConst : public SALOME_EvalSet
{
public:
  SALOME_EvalSetConst();
  virtual ~SALOME_EvalSetConst();

  static  RString            Name();
  virtual RString            name() const;

  virtual bool               createValue( const RString&, SALOME_EvalVariant& ) const;

  virtual void               operationList( RStringList& ) const;

  virtual void               bracketsList( RStringList&, bool open ) const;

  virtual int                priority( const RString&, bool isBin ) const;
  
  virtual SALOME_EvalExprError isValid(const RString&, 
                                     const SALOME_EvalVariantType,
                                     const SALOME_EvalVariantType ) const;

  virtual SALOME_EvalExprError calculate(const RString&, 
                                      SALOME_EvalVariant&,
                                      SALOME_EvalVariant& ) const;
};

#endif
