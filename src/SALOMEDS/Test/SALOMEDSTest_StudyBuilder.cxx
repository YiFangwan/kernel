// Copyright (C) 2006  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

/*!
 * Check all methods of SALOMEDS_StudyBuilder
 * Use code of SALOMEDS_StudyBuilder.cxx
 */

void SALOMEDSTest::testStudyBuilder()
{
  //Create or find the Study manager
  _PTR(StudyManager) sm ( new SALOMEDS_StudyManager(_sm) );

  CPPUNIT_ASSERT(sm);

  //Create a new study
  _PTR(Study) study = sm->NewStudy("TestStudyBuilder");

  CPPUNIT_ASSERT(study);

  //Create Study Builder
  _PTR(StudyBuilder) studyBuilder = study->NewBuilder();

  //Check the StudyBuilder creation
  CPPUNIT_ASSERT(studyBuilder);

  //Check method NewComponent
  _PTR(SComponent) sco1 = studyBuilder->NewComponent("Test");
  CPPUNIT_ASSERT(sco1 && sco1->ComponentDataType() == "Test");

  //Check method DefineComponentInstance
  string ior = _orb->object_to_string(_sm);
  studyBuilder->DefineComponentInstance(sco1, ior);
  string newior;
  sco1->ComponentIOR(newior);
  CPPUNIT_ASSERT(newior == ior);

  //Check method RemoveComponent
  studyBuilder->RemoveComponent(sco1);
  _PTR(SComponent) sco2 = study->FindComponent("Test");
  CPPUNIT_ASSERT(!sco2);


  _PTR(SComponent) sco3 = studyBuilder->NewComponent("NewComp");
  CPPUNIT_ASSERT(sco3);

  //Check method NewObject
  _PTR(SObject) so1 = studyBuilder->NewObject(sco3);
  CPPUNIT_ASSERT(so1);
  string id1 = so1->GetID();

  //Check method NewObjectToTag
  _PTR(SObject) so2 = studyBuilder->NewObjectToTag(so1, 2);
  CPPUNIT_ASSERT(so2 && so2->Tag() == 2);
  string id2 = so2->GetID();

  //Check method FindOrCreateAttribute
  _PTR(SObject) so3 = studyBuilder->NewObject(sco3);
  CPPUNIT_ASSERT(so3);
  _PTR(AttributeName) an3 = studyBuilder->FindOrCreateAttribute(so3, "AttributeName");
  CPPUNIT_ASSERT(an3);

  //Check method FindAttribute
  _PTR(GenericAttribute) ga;
  CPPUNIT_ASSERT(studyBuilder->FindAttribute(so3, ga, "AttributeName"));

  //Check method RemoveObject
  studyBuilder->RemoveObject(so3);
  CPPUNIT_ASSERT(!studyBuilder->FindAttribute(so3, ga, "AttributeName"));

  //Check method RemoveObjectWithChildren
  _PTR(AttributeName) an2 = studyBuilder->FindOrCreateAttribute(so2, "AttributeName");
  CPPUNIT_ASSERT(an2);
  studyBuilder->RemoveObjectWithChildren(so1);
  CPPUNIT_ASSERT(!studyBuilder->FindAttribute(so2, ga, "AttributeName"));

  //Check method RemoveAttribute
  _PTR(AttributeName) an1 = studyBuilder->FindOrCreateAttribute(so1, "AttributeName");
  CPPUNIT_ASSERT(an1);
  CPPUNIT_ASSERT(studyBuilder->FindAttribute(so1, ga, "AttributeName"));
  studyBuilder->RemoveAttribute(so1, "AttributeName");
  CPPUNIT_ASSERT(!studyBuilder->FindAttribute(so1, ga, "AttributeName"));

  //Check method Addreference
  studyBuilder->Addreference(so2, so1);
  _PTR(SObject) refSO;
  CPPUNIT_ASSERT(so2->ReferencedObject(refSO) && refSO->GetID() == so1->GetID());

  //Check method RemoveReference
  studyBuilder->RemoveReference(so2);
  CPPUNIT_ASSERT(!so2->ReferencedObject(refSO));

  //Check method SetGUID and IsGUID
  string value = "0e1c36e6-379b-4d90-ab3b-17a14310e648";
  studyBuilder->SetGUID(so1, value);
  CPPUNIT_ASSERT(studyBuilder->IsGUID(so1, value));

  //Check method UndoLimit (set/get)
  studyBuilder->UndoLimit(10);
  CPPUNIT_ASSERT(studyBuilder->UndoLimit() == 10);

  //Check transactions methods: NewCommand, CommitCommand, AbortCommand, 
  //HasOpenedCommand, Undo, Redo, GetAvailableUndos, GetAvailableRedos
  _PTR(SObject) so4 = studyBuilder->NewObject(sco3);
  CPPUNIT_ASSERT(so4);
  studyBuilder->NewCommand();
  CPPUNIT_ASSERT(studyBuilder->HasOpenCommand());
  _PTR(AttributeName) an4 = studyBuilder->FindOrCreateAttribute(so4, "AttributeName");
  CPPUNIT_ASSERT(an4);
  an4->SetValue("command1");
  studyBuilder->CommitCommand();
  CPPUNIT_ASSERT(!studyBuilder->HasOpenCommand());
  studyBuilder->NewCommand();
  an4->SetValue("command2");
  studyBuilder->AbortCommand();
  CPPUNIT_ASSERT(an4->Value() == "command1");
  studyBuilder->NewCommand();
  an4->SetValue("command2");
  studyBuilder->CommitCommand();
  studyBuilder->Undo();
  CPPUNIT_ASSERT(an4->Value() == "command1");
  CPPUNIT_ASSERT(studyBuilder->GetAvailableRedos());
  studyBuilder->Redo();
  CPPUNIT_ASSERT(an4->Value() == "command2");
  CPPUNIT_ASSERT(studyBuilder->GetAvailableUndos());

  //Check method SetName
  studyBuilder->SetName(so1, "new name");
  CPPUNIT_ASSERT(so1->GetName() == "new name");

  //Check method SetComment
  studyBuilder->SetComment(so1, "new comment");
  CPPUNIT_ASSERT(so1->GetComment() == "new comment");

  //Check method SetIOR
  studyBuilder->SetIOR(so1, ior);
  CPPUNIT_ASSERT(so1->GetIOR() == ior);

  sm->Close(study);

  //Check method LoadWith
  _PTR(Study) study2 = sm->NewStudy("Study2");

  SALOME_NamingService NS(_orb);
  CORBA::Object_var obj = SALOME_LifeCycleCORBA(&NS).FindOrLoad_Component("SuperVisionContainer", "SUPERV");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj));

  MESSAGE("Created a new GEOM component");

  SALOMEDS::Driver_var drv = SALOMEDS::Driver::_narrow(obj);
  CPPUNIT_ASSERT(!CORBA::is_nil(drv));
  _PTR(StudyBuilder) sb2 = study2->NewBuilder();
  _PTR(SComponent) sco = sb2->NewComponent("SUPERV");
  ior = _orb->object_to_string(drv);
  sb2->DefineComponentInstance(sco, ior);

  sm->SaveAs("srn_SALOMEDS_UnitTests.hdf", study2, false);
  sm->Close(study2);

  _PTR(Study) study3 = sm->Open("srn_SALOMEDS_UnitTests.hdf");
  _PTR(StudyBuilder) sb3 = study3->NewBuilder();
  _PTR(SComponent) aComp = study3->FindComponent("SUPERV");
  CPPUNIT_ASSERT(aComp);

  CORBA::Object_var obj2 = SALOME_LifeCycleCORBA(&NS).FindOrLoad_Component("SuperVisionContainer", "SUPERV");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj2));
  SALOMEDS::Driver_var drv2 = SALOMEDS::Driver::_narrow(obj2);
  ior = _orb->object_to_string(drv2);

  bool isRaised = false;
  try {
    sb3->LoadWith(aComp, ior);
  }
  catch(...) {
    isRaised = true;
  }

  CPPUNIT_ASSERT(!isRaised);

  ior = "";
  aComp->ComponentIOR(ior);
  CPPUNIT_ASSERT(!ior.empty());

  system("rm -f srn_SALOMEDS_UnitTests.hdf");

  //Check method AddDirectory
  _PTR(AttributeName) na1 = sb3->FindOrCreateAttribute(aComp, "AttributeName");
  na1->SetValue("Component");

  isRaised = false;
  try {
    sb3->AddDirectory("/Component/Dir1");
  } catch(...) {
    isRaised = true;
  }

  CPPUNIT_ASSERT(!isRaised);
  _PTR(SObject) so5 = study3->FindObjectByPath("/Component/Dir1");
  CPPUNIT_ASSERT(so5);

  isRaised = false;
  try {
    sb3->AddDirectory("/Component/Dir1"); //Attempt to create the same directory
  } catch(...) {
    isRaised = true;
  }
  CPPUNIT_ASSERT(isRaised);

  isRaised = false;
  try {
    sb3->AddDirectory("/MyComponent/Dir1"); //Attempt to create the invalid directory
  } catch(...) {
    isRaised = true;
  }
  CPPUNIT_ASSERT(isRaised);

  sm->Close(study3);
}
