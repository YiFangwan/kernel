#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
# 
#  This library is free software; you can redistribute it and/or 
#  modify it under the terms of the GNU Lesser General Public 
#  License as published by the Free Software Foundation; either 
#  version 2.1 of the License. 
# 
#  This library is distributed in the hope that it will be useful, 
#  but WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
#  Lesser General Public License for more details. 
# 
#  You should have received a copy of the GNU Lesser General Public 
#  License along with this library; if not, write to the Free Software 
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
# 
#  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
#
#
#
#  File   : salome_study.py
#  Author : Paul RASCLE, EDF
#  Module : SALOME
#  $Header$

from salome_iapp import sg
from salome_kernel import *
import SALOMEDS

#--------------------------------------------------------------------------

def DumpComponent(Study, SO, offset):
    it = Study.NewChildIterator(SO)
    Builder = Study.NewBuilder()
    while it.More():
        CSO = it.Value()
        it.Next()
        anAttr = Builder.FindOrCreateAttribute(CSO, "AttributeName")
        AtName = anAttr._narrow(SALOMEDS.AttributeName)
        t_name = AtName.Value()
        if t_name[0] == 1:
            ofs = 1
            a = ""
            while ofs <= offset:
                a = a + "--"
                ofs = ofs +1
            MESSAGE( a + ">" + str(CSO.GetID()) + " " + str(t_name[1]) )
        t_RefSO = CSO.ReferencedObject()
        if t_RefSO[0] == 1:
            RefSO = t_RefSO[1]
            ofs = 1
            a = ""
            while ofs <= offset:
                a = a + "  "
                ofs = ofs +1
            MESSAGE( a + ">" + str(RefSO.GetID()) )
        DumpComponent(Study, CSO, offset+2)

    #--------------------------------------------------------------------------

def DumpStudy(Study):
    itcomp = Study.NewComponentIterator()
    while itcomp.More():
        SC = itcomp.Value()
        itcomp.Next()
        name = SC.ComponentDataType()
        MESSAGE( "-> ComponentDataType is " + name )
        DumpComponent(Study, SC, 1)
        

    #--------------------------------------------------------------------------

def IDToObject(id):
    myObj = None
    mySO = myStudy.FindObjectID(id);
    if mySO is not None:
        ok, anAttr = mySO.FindAttribute("AttributeIOR")
        if ok:
            AtIOR = anAttr._narrow(SALOMEDS.AttributeIOR)
            if AtIOR.Value() != "":
                myObj = orb.string_to_object(AtIOR.Value())
    return myObj

def ObjectToSObject(obj):
    mySO = None
    if obj is not None:
        ior =  orb.object_to_string(obj)
        if ior != "":
            mySO = myStudy.FindObjectIOR(ior)
    return mySO

def ObjectToID(obj):
    mySO = ObjectToSObject(obj)
    if mySO:
        return mySO.GetID()
    return ""

def IDToSObject(id):
    mySO = myStudy.FindObjectID(id);
    return mySO

    #--------------------------------------------------------------------------

salome_study_ID = -1

def getActiveStudy():
    global salome_study_ID
    print "getActiveStudy"
    if salome_study_ID == -1:
        if sg.hasDesktop():
            print "---in gui"
            salome_study_ID = sg.getActiveStudyId()
        else:
            print "---outside gui"
            salome_study_ID = createNewStudy()
            print"---", salome_study_ID
    return salome_study_ID
    
    #--------------------------------------------------------------------------

def createNewStudy():
    print "createNewStudy"
    i=1
    aStudyName = "noName"
    nameAlreadyInUse = 1
    listOfOpenStudies = myStudyManager.GetOpenStudies()
    print listOfOpenStudies
    while nameAlreadyInUse:
        aStudyName = "extStudy_%d"%i
        if aStudyName not in listOfOpenStudies:
            nameAlreadyInUse=0
        else:
            i = i+1
    theStudy = myStudyManager.NewStudy(aStudyName)
    theStudyId = theStudy._get_StudyId()
    print aStudyName, theStudyId
    return theStudyId
            
    #--------------------------------------------------------------------------

# get Study Manager reference
print "looking for studyManager ..."
obj = naming_service.Resolve('myStudyManager')
myStudyManager = obj._narrow(SALOMEDS.StudyManager)
print "studyManager found"

# get active study Id, ref and name
myStudyId = getActiveStudy()
print "myStudyId",myStudyId
myStudy = myStudyManager.GetStudyByID(myStudyId)
myStudyName = myStudy._get_Name()



