//=============================================================================
// File      : SALOME_PYQT_GUI.hxx
// Created   : mer jun  4 17:17:20 UTC 2003
// Author    : Paul RASCLE, EDF 
// Project   : SALOME
// Copyright : EDF 2003
// $Header$
//=============================================================================

#ifndef _SALOME_PYQT_GUI_HXX_
#define _SALOME_PYQT_GUI_HXX_

#include "QAD_Desktop.h"
#include "SALOMEGUI.h"

class SALOME_PYQT_GUI: public SALOMEGUI
{
  Q_OBJECT;
  
private:
  
public:
  
  SALOME_PYQT_GUI( const QString& name = "", QObject* parent = 0 );
  virtual ~SALOME_PYQT_GUI();

  virtual bool OnGUIEvent   (int theCommandID, QAD_Desktop* parent);
  virtual bool OnKeyPress   (QKeyEvent* pe, QAD_Desktop* parent,
			     QAD_StudyFrame* studyFrame);
  virtual bool OnMousePress (QMouseEvent* pe, QAD_Desktop* parent,
			     QAD_StudyFrame* studyFrame);
  virtual bool OnMouseMove  (QMouseEvent* pe, QAD_Desktop* parent,
			     QAD_StudyFrame* studyFrame);
  virtual bool SetSettings  (QAD_Desktop* parent, char* moduleName);
  virtual bool CustomPopup  (QAD_Desktop* parent, QPopupMenu* popup,
			     const QString & theContext,
			     const QString & theParent,
			     const QString & theObject);
  virtual void DefinePopup  (QString & theContext, QString & theParent,
			     QString & theObject) ;
  virtual bool ActiveStudyChanged (QAD_Desktop* parent);
  
protected:
  
};

#endif
