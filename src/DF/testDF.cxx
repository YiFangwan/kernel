// Copyright (C) 2007-2020  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <cppunit/extensions/HelperMacros.h>
#include <sstream>

#include "DF_Application.hxx"
#include "DF_Document.hxx"
#include "DF_Attribute.hxx"
#include "DF_Label.hxx"
#include "DF_Container.hxx"

class DFTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( DFTest );
  CPPUNIT_TEST( test1 );
  CPPUNIT_TEST( test2 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void test1();
  void test2();
};

void DFTest::setUp()
{
}

void DFTest::tearDown()
{
}

void DFTest::test1()
{
  DF_Application* appli = new DF_Application;
  DF_Document* doc = appli->NewDocument("doc");
  
  DF_Label root = doc->Main();               //0:1
  DF_Label child = root.FindChild(3, true);  //0:1:3
  CPPUNIT_ASSERT(child.Entry() == "0:1:3");
  
  DF_Container* attr = DF_Container::Set(child);
  attr->SetInt("integer_value", 18);

  DF_Attribute* result = child.FindAttribute(DF_Container::GetID());
  CPPUNIT_ASSERT(result);
  attr = dynamic_cast<DF_Container*>(result);
  CPPUNIT_ASSERT(attr->HasIntID("integer_value"));
  CPPUNIT_ASSERT(attr->GetInt("integer_value") == 18);

  attr->SetInt("integer_value", 100);
  CPPUNIT_ASSERT(attr->GetInt("integer_value") == 100);

  child.ForgetAttribute(DF_Container::GetID());
  result = child.FindAttribute(DF_Container::GetID());
  CPPUNIT_ASSERT(!result);

  child = root.NewChild();                   //0:1:4
  child.NewChild();                          //0:1:4:1
  child.NewChild();                          //0:1:4:2
  CPPUNIT_ASSERT(child == child);
  CPPUNIT_ASSERT(child != root);

  child = root.NewChild();                   //0:1:5
  child.NewChild();                          //0:1:5:1
  child = root.NewChild();                   //0:1:6
  CPPUNIT_ASSERT(child.Entry() == "0:1:6");

  child = DF_Label::Label(child, "0:1:4:1"); //0:1:4:1
  CPPUNIT_ASSERT(child.Entry() == "0:1:4:1");
  attr = DF_Container::Set(child);
  attr->SetDouble("double_value", 1.23);
  CPPUNIT_ASSERT(attr->HasDoubleID("double_value"));
  attr->SetString("string_value", "custom string");
  CPPUNIT_ASSERT(attr->HasStringID("string_value"));
}

void DFTest::test2()
{
  DF_Application* appli = new DF_Application;
  DF_Document* doc = appli->NewDocument("doc");

  DF_Label root = doc->Main();                       //0:1
  CPPUNIT_ASSERT(root.Tag() == 1);
  CPPUNIT_ASSERT(root.Entry() == "0:1");

  DF_Label sco = root.NewChild();                    //0:1:1
  CPPUNIT_ASSERT(sco.Tag() == 1);
  CPPUNIT_ASSERT(sco.Entry() == "0:1:1");

  DF_Label so_3 = sco.FindChild(3, true);            //0:1:1:3
  CPPUNIT_ASSERT(so_3.Tag() == 3);
  CPPUNIT_ASSERT(so_3.Entry() == "0:1:1:3");

  DF_Label so_5 = sco.FindChild(5, true);            //0:1:1:5
  CPPUNIT_ASSERT(so_5.Tag() == 5);
  CPPUNIT_ASSERT(so_5.Entry() == "0:1:1:5");

  DF_Label so_5_1 = so_5.NewChild();                 //0:1:1:5:1
  CPPUNIT_ASSERT(so_5_1.Tag() == 1);
  CPPUNIT_ASSERT(so_5_1.Entry() == "0:1:1:5:1");

  DF_Label so_5_1_1 = so_5_1.NewChild();             //0:1:1:5:1:1
  CPPUNIT_ASSERT(so_5_1_1.Tag() == 1);
  CPPUNIT_ASSERT(so_5_1_1.Entry() == "0:1:1:5:1:1");

  DF_Label so_5_1_3 = so_5_1.FindChild(3, true);     //0:1:1:5:1:3
  CPPUNIT_ASSERT(so_5_1_3.Tag() == 3);
  CPPUNIT_ASSERT(so_5_1_3.Entry() == "0:1:1:5:1:3");

  DF_Label so_5_1_3_1 = so_5_1_3.NewChild();        //0:1:1:5:1:3:1
  CPPUNIT_ASSERT(so_5_1_3_1.Tag() == 1);
  CPPUNIT_ASSERT(so_5_1_3_1.Entry() == "0:1:1:5:1:3:1");

  DF_Label so_5_1_2 = so_5_1.FindChild(2, true);
  CPPUNIT_ASSERT(so_5_1_2.Tag() == 2);
  CPPUNIT_ASSERT(so_5_1_2.Entry() == "0:1:1:5:1:2");

  delete appli;    
}

// --- Registers the fixture into the 'registry'

CPPUNIT_TEST_SUITE_REGISTRATION( DFTest );

// --- generic Main program from Basic/Test

#include "BasicMainTest.hxx"
