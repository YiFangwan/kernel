using namespace std;
//=============================================================================
// File      : SALOME_PYQT_GUI.cxx
// Created   : mer jun  4 17:17:20 UTC 2003
// Author    : Paul RASCLE, EDF 
// Project   : SALOME
// Copyright : EDF 2003
// $Header$
//=============================================================================

#include "SALOME_PYQT_GUI.hxx"

#include "QAD_Desktop.h"

#include "QAD_MessageBox.h"
#include "SALOME_Selection.h"
#include "SALOME_InteractiveObject.hxx"
#include "SALOMEGUI_QtCatchCorbaException.hxx"
#include "utilities.h"
#include "PyInterp_PyQt.h"
//#include <cStringIO.h>

#include <sipSalomePyQtDeclSalomePyQt.h>
#include <sipqtQWorkspace.h>
#include <sipqtQPopupMenu.h>

#include <map>
#include <string>

static PyInterp_PyQt *interp = NULL;
static map<int,PyInterp_PyQt*> mapInterp;
static PyObject *module;
static string _moduleName;

//=============================================================================
/*!
 *  Calls module.setWorkSpace with PyQt QWorkspace object to use with
 *  interpreter.
 */
//=============================================================================

static void setWorkSpace()
{
  MESSAGE("setWorkSpace");
  PyLockWrapper aLock = interp->GetLockWrapper();

  PyObjWrapper pyws(sipMapCppToSelf( QAD_Application::getDesktop()->getMainFrame(),
				     sipClass_QWorkspace));
  PyObjWrapper res(PyObject_CallMethod(module,"setWorkSpace","O",pyws.get()));
  SCRUTE(pyws->ob_refcnt);
  if(!res){
    PyErr_Print();
    return ;
  }
  return;
}

//=============================================================================
/*!
 *  Initialises python interpreter (only one per study), imports python module
 *  from given module name, sets static reference to module,
 *  sets module workspace.
 */
//=============================================================================

static void initInterp(int StudyID)
{
  MESSAGE("initInterp");
  if(mapInterp.find(StudyID) != mapInterp.end()){
    MESSAGE ( " StudyID is found " << StudyID );
    interp = mapInterp[StudyID];
    return;
  }else{
    MESSAGE ( " StudyID is not found " << StudyID );
    interp=new PyInterp_PyQt();
    mapInterp[StudyID] = interp;
  }

  PyLockWrapper aLock = interp->GetLockWrapper();

  PyObjWrapper res(PyImport_ImportModule((char*)_moduleName.c_str()));
  if(!res){
    MESSAGE ( " Problem... " );
    PyErr_Print();
    return;
  }

  // PyQt import is OK
  setWorkSpace();
}

//=============================================================================
/*!
 *  Calls python module.OnGUIEvent(theCommandID)
 */
//=============================================================================

bool SALOME_PYQT_GUI::OnGUIEvent (int theCommandID,
				  QAD_Desktop* parent)
{
  MESSAGE("SALOME_PYQT_GUI::OnGUIEvent");
  PyLockWrapper aLock = interp->GetLockWrapper();

  PyObjWrapper res(PyObject_CallMethod(module,"OnGUIEvent","i",theCommandID));
  if(!res){
    PyErr_Print();
    return false;
  }
  return true;
}

//=============================================================================
/*!
 *  no call to python module.OnKeyPress()
 */
//=============================================================================

bool SALOME_PYQT_GUI::OnKeyPress (QKeyEvent* pe,
				  QAD_Desktop* parent,
				  QAD_StudyFrame* studyFrame)
{
  MESSAGE("SALOME_PYQT_GUI::OnKeyPress");
  return true;
}

//=============================================================================
/*!
 *  no call to python module.OnMousePress()
 */
//=============================================================================

bool SALOME_PYQT_GUI::OnMousePress (QMouseEvent* pe ,
				    QAD_Desktop* parent, 
				    QAD_StudyFrame* studyFrame)
{
  MESSAGE("SALOME_PYQT_GUI::OnMousePress");
  return false;
}

//=============================================================================
/*!
 *  no call to python module.OnMouseMove()
 */
//=============================================================================

bool SALOME_PYQT_GUI::OnMouseMove (QMouseEvent* pe ,
				   QAD_Desktop* parent, 
				   QAD_StudyFrame* studyFrame)
{
  // La ligne suivante est commentée sinon multiple traces ...
  // MESSAGE("SALOME_PYQT_GUI::OnMouseMouve");
  return true;
}

//=============================================================================
/*!
 *  Calls initInterp to initialise python interpreter (only one per study) and
 *  to import python module. Calls module.setSettings() 
 */
//=============================================================================

bool SALOME_PYQT_GUI::SetSettings (QAD_Desktop* parent, char* moduleName)
{
  MESSAGE("SALOME_PYQT_GUI::SetSettings");
  int StudyID = QAD_Application::getDesktop()->getActiveStudy()->getStudyId();
  SCRUTE ( StudyID );
  _moduleName = moduleName + string("GUI");
  SCRUTE(_moduleName);
  initInterp(StudyID);
  
  PyLockWrapper aLock = interp->GetLockWrapper();

  PyObjWrapper res(PyObject_CallMethod(module,"setSettings",""));
  if(!res){
    PyErr_Print();
    return false;
  }
  return true;
}

//=============================================================================
/*!
 * Calls module.customPopup with popup menu to custom, and string values of 
 * context, parent and selected object (strings defined by DefinePopup, which
 * is called before).
 */
//=============================================================================

bool SALOME_PYQT_GUI::CustomPopup ( QAD_Desktop* parent,
				    QPopupMenu* popup,
				    const QString & theContext,
				    const QString & theParent,
				    const QString & theObject )
{
  MESSAGE("SALOME_PYQT_GUI::CustomPopup");

  MESSAGE ( " theContext : " << theContext.latin1() );
  MESSAGE ( " theParent : " << theParent.latin1() );
  MESSAGE ( " theObject : " << theObject.latin1() );

  PyLockWrapper aLock = interp->GetLockWrapper();

  PyObjWrapper pypop(sipMapCppToSelf( popup, sipClass_QPopupMenu));

  PyObjWrapper res(PyObject_CallMethod(module,"customPopup",
				       "Osss",pypop.get(),
				       theContext.latin1(), 
				       theObject.latin1(), 
				       theParent.latin1()));
  if(!res){
    PyErr_Print();
    return false;
  }
  return true;
}


//=============================================================================
/*!
 * Calls module.definePopup to modify the strings that define context parent
 * and selected object. Called before CustomPopup.
 */
//=============================================================================

void SALOME_PYQT_GUI::DefinePopup( QString & theContext,
				   QString & theParent,
				   QString & theObject )
{
  MESSAGE("SALOME_PYQT_GUI::DefinePopup");
  theContext = "";
  theObject = "";
  theParent = "";
  
  PyLockWrapper aLock = interp->GetLockWrapper();

  PyObjWrapper res(PyObject_CallMethod(module,"definePopup","sss",
				       theContext.latin1(), 
				       theObject.latin1(), 
				       theParent.latin1()));
  if(!res){
    PyErr_Print();
    return;
  }
  char *co, *ob, *pa;
  int parseOk = PyArg_ParseTuple(res, "sss", &co, &ob, &pa);

  MESSAGE ("parseOk " << parseOk);
  MESSAGE (" --- " << co << " " << ob << " " << pa);

  theContext = co;
  theObject = ob;
  theParent = pa;

  MESSAGE ( " theContext : " << theContext.latin1() );
  MESSAGE ( " theParent : " << theParent.latin1() );
  MESSAGE ( " theObject : " << theObject.latin1() );
}

//=============================================================================
/*!
 * Initialize new interpreter (if not exists) with new study ID.
 * Calls module.activeStudyChanged with new study ID. Called twice.
 */
//=============================================================================

bool SALOME_PYQT_GUI::ActiveStudyChanged( QAD_Desktop* parent )
{
  MESSAGE("SALOME_PYQT_GUI::ActiveStudyChanged");
  
  int StudyID = parent->getActiveApp()->getActiveStudy()->getStudyId();
  initInterp(StudyID);
  
  PyLockWrapper aLock = interp->GetLockWrapper();

  PyObjWrapper res(PyObject_CallMethod(module,"activeStudyChanged","i", StudyID ));
  if(!res){
    PyErr_Print();
    return false;
  }
  return true;
}


//=============================================================================
/*!
 *  
 */
//=============================================================================


static SALOME_PYQT_GUI aGUI("");
extern "C"
{
  Standard_EXPORT SALOMEGUI* GetComponentGUI() {
    return &aGUI;
  }
}
