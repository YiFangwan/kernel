//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : QAD_PyEditor.cxx
//  Author : Nicolas REJNERI
//  Module : SALOME
//  $Header$

#include "QAD_PyEditor.h"
#include "QAD_PyInterp.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Config.h"
#include "QAD_Tools.h"
#include "QAD_MessageBox.h"
//#include "QAD_RightFrame.h"
using namespace std;

#include <qapplication.h>
#include <qmap.h>
#include <qclipboard.h>
#include <qthread.h>

// NRI : Temporary added
// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)
//NRI

#define SIZEPR 4
enum { IdCopy, IdPaste, IdClear, IdSelectAll };

class PythonThread : public QThread
{
public:
  PythonThread( PyInterp_base* interp, QAD_PyEditor* listener )
    : QThread(), myInterp( interp ), myListener( listener ), myCommand( 0 )
  {}

  virtual ~PythonThread() {}

  void exec( const char* command )
  {
    myCommand = (char*)command;
    start();
  }

protected:
  virtual void run()
  {
    if ( myInterp && myCommand && myListener ) {
      myListener->viewport()->setCursor( waitCursor );
      int ret = myInterp->run( myCommand );
      QThread::postEvent( myListener, 
			  new QCustomEvent( ret < 0 ? QAD_PyEditor::PYTHON_ERROR : ( ret ? QAD_PyEditor::PYTHON_INCOMPLETE : QAD_PyEditor::PYTHON_OK ) ) );
      myListener->viewport()->unsetCursor();
    }
  }

private:
  PyInterp_base* myInterp;
  QAD_PyEditor*  myListener;
  char*          myCommand;
};

/*!
    Constructor
*/
QAD_PyEditor::QAD_PyEditor(QAD_PyInterp* interp, 
			   QWidget *parent, const char *name)
  : QTextEdit(parent,name)
{
  QString fntSet = QAD_CONFIG->getSetting("Viewer:ConsoleFont");
  QFont myFont = QAD_Tools::stringToFont( fntSet );
//  QFont myFont("Courier",11);
  setFont(myFont);
  setTextFormat(QTextEdit::PlainText);
  _interp = interp;
  string banner = _interp->getbanner();
  setText(banner.c_str());
  _isInHistory = false;
  _currentPrompt = ">>> ";
  // put error messages of interpreter if they exist.
  _buf.truncate(0);
  setText(_interp->getverr().c_str());
  setText(_currentPrompt);
  setPalette( QAD_Application::getPalette(true) );
  setWordWrap(NoWrap);

  _thread = new PythonThread( interp, this );

  connect(this,SIGNAL(returnPressed()),this,SLOT(handleReturn()) );
}

/*!
    Destructor
*/
QAD_PyEditor::~QAD_PyEditor()
{
  if ( _thread->wait( 1000 ) )
    delete _thread;
}

/*!
    Called to insert a string s 
*/
void QAD_PyEditor::setText(QString s)
{
//   MESSAGE("setText");
  int para=paragraphs()-1;
  int col=paragraphLength(para);
  insertAt(s,para,col);
  int n = paragraphs()-1;  
  setCursorPosition( n, paragraphLength(n)); 
}

/*!
    Called when an handleReturn
*/
void QAD_PyEditor::handleReturn()
{
  int ret;
  int para=paragraphs()-2;

  // NRI : Temporary added
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  
  if ( aStudy->GetProperties()->IsLocked() ) {
    QApplication::restoreOverrideCursor();
    QAD_MessageBox::warn1 ( (QWidget*)QAD_Application::getDesktop(),
			    QObject::tr("WARNING"), 
			    QObject::tr("WRN_STUDY_LOCKED"),
			    QObject::tr("BUT_OK") );

    _currentPrompt = ">>> ";
    setText(_currentPrompt);
    
    return;
  }  
  // NRI

  _buf.append(text(para).remove(0,SIZEPR));
  _buf.truncate( _buf.length() - 1 );
  setReadOnly( true );
  _thread->exec(_buf.latin1());
}

/*
   Processes own popup menu
*/
void QAD_PyEditor::mousePressEvent (QMouseEvent * event)
{
  if ( event->button() == RightButton ) {
    QPopupMenu *popup = new QPopupMenu( this );
    QMap<int, int> idMap;

    int para1, col1, para2, col2;
    getSelection(&para1, &col1, &para2, &col2);
    bool allSelected = hasSelectedText() &&
      para1 == 0 && para2 == paragraphs()-1 && col1 == 0 && para2 == paragraphLength(para2);
    int id;
    id = popup->insertItem( tr( "EDIT_COPY_CMD" ) );
    idMap.insert(IdCopy, id);
    id = popup->insertItem( tr( "EDIT_PASTE_CMD" ) );
    idMap.insert(IdPaste, id);
    id = popup->insertItem( tr( "EDIT_CLEAR_CMD" ) );
    idMap.insert(IdClear, id);
    popup->insertSeparator();
    id = popup->insertItem( tr( "EDIT_SELECTALL_CMD" ) );
    idMap.insert(IdSelectAll, id);
    popup->setItemEnabled( idMap[ IdCopy ],  hasSelectedText() );
    popup->setItemEnabled( idMap[ IdPaste ],
			  !isReadOnly() && (bool)QApplication::clipboard()->text().length() );
    popup->setItemEnabled( idMap[ IdSelectAll ],
			  (bool)text().length() && !allSelected );
    
    int r = popup->exec( event->globalPos() );
    delete popup;
    
    if ( r == idMap[ IdCopy ] ) {
      copy();
    }
    else if ( r == idMap[ IdPaste ] ) {
      paste();
    }
    else if ( r == idMap[ IdClear ] ) {
      clear();
      string banner = _interp->getbanner();
      setText(banner.c_str());
      setText(_currentPrompt);
    }
    else if ( r == idMap[ IdSelectAll ] ) {
      selectAll();
    }
    return;
  }
  else {
    QTextEdit::mousePressEvent(event);
  }
}

/*!
    Called when a Mouse release event
*/
void QAD_PyEditor::mouseReleaseEvent ( QMouseEvent * e )
{
  //  MESSAGE("mouseReleaseEvent");
  int curPara, curCol; // for cursor position
  int endPara, endCol; // for last edited line
  getCursorPosition(&curPara, &curCol);
  endPara = paragraphs() -1;
  if (e->button() != MidButton)
    QTextEdit::mouseReleaseEvent(e);
  else if ((curPara == endPara) && (curCol >= SIZEPR))
    QTextEdit::mouseReleaseEvent(e);
}

/*!
    Called when a drop event (Drag & Drop)
*/
  void QAD_PyEditor::dropEvent (QDropEvent *e)
{
  MESSAGE("dropEvent : not handled");
}

/*!
   Checks, is the string a command line or not.
*/

bool QAD_PyEditor::isCommand( const QString& str) const
{
  if (str.find(_currentPrompt)==0)
    return true;
  return false;
}


/*!
    Called when a keyPress event
*/
void QAD_PyEditor::keyPressEvent( QKeyEvent *e )
{
  int curLine, curCol; // for cursor position
  int endLine, endCol; // for last edited line
  getCursorPosition(&curLine, &curCol);
  endLine = paragraphs() -1;
  //MESSAGE("current position " << curLine << ", " << curCol);
  //MESSAGE("last line " << endLine);
  //MESSAGE(e->key());
  int aKey=e->key();
  int keyRange=0;
  if ((aKey >= Key_Space) && (aKey <= Key_ydiaeresis))
    keyRange = 0;
  else
    keyRange = aKey;

  bool ctrlPressed = ( (e->state() & ControlButton) == ControlButton );
  bool shftPressed = ( (e->state() & ShiftButton) ==  ShiftButton );

  switch (keyRange)
    {
    case 0 :
      {
	if (curLine <endLine || curCol < SIZEPR )
	  moveCursor(QTextEdit::MoveEnd, false);
	QTextEdit::keyPressEvent( e );
	break;
      }
    case Key_Return:
    case Key_Enter:
      {
	if (curLine <endLine)
	  moveCursor(QTextEdit::MoveEnd, false);
	else
	  moveCursor(QTextEdit::MoveLineEnd, false);
	QTextEdit::keyPressEvent( e );
	break;
      }
    case Key_Up:
      {
	// if Cntr+Key_Up event then move cursor up
	if (ctrlPressed) {
	    moveCursor(QTextEdit::MoveUp, false);
        }
	// if Shift+Key_Up event then move cursor up and select the text
	else if ( shftPressed && curLine > 0 ){
	    moveCursor(QTextEdit::MoveUp, true);
	}
	// scroll the commands stack up
	else { 
	   QString histLine = _currentPrompt;
	  if (! _isInHistory)
	    {
	      _isInHistory = true;
	      _currentCommand = text(endLine).remove(0,SIZEPR);
	      _currentCommand.truncate( _currentCommand.length() - 1 );
	      SCRUTE(_currentCommand);
	    }
	  QString previousCommand = _interp->getPrevious();
	  if (previousCommand.compare(BEGIN_HISTORY_PY) != 0)
	    {
	      removeParagraph(endLine);
	      histLine.append(previousCommand);
	      insertParagraph(histLine, -1);
	    }
	  moveCursor(QTextEdit::MoveEnd, false);
	}
	break;
      }
    case Key_Down:
      {
	// if Cntr+Key_Down event then move cursor down
	if (ctrlPressed) {
	  moveCursor(QTextEdit::MoveDown, false);
	}
	// if Shift+Key_Down event then move cursor down and select the text
	else if ( shftPressed && curLine < endLine ) {
	  moveCursor(QTextEdit::MoveDown, true);
	}
	// scroll the commands stack down
	else {
	QString histLine = _currentPrompt;
	  QString nextCommand = _interp->getNext();
	  if (nextCommand.compare(TOP_HISTORY_PY) != 0)
	    {
	      removeParagraph(endLine);
	      histLine.append(nextCommand);
	      insertParagraph(histLine, -1);
	    }
	  else
	    if (_isInHistory)
	      {
		_isInHistory = false;
		removeParagraph(endLine);
		histLine.append(_currentCommand);
		insertParagraph(histLine, -1);
	      }
	  moveCursor(QTextEdit::MoveEnd, false);
	}
	break;
      }
    case Key_Left:
      {
	if (!shftPressed && isCommand(text(curLine)) && curCol <= SIZEPR )
	  {
	    setCursorPosition((curLine -1), SIZEPR);
	    moveCursor(QTextEdit::MoveLineEnd, false);
	  }
	else QTextEdit::keyPressEvent( e );
	break;
      }
    case Key_Right:
      {
	if (!shftPressed && isCommand(text(curLine)) 
	    && curCol < SIZEPR) setCursorPosition(curLine, SIZEPR);
	QTextEdit::keyPressEvent( e );
	break;
      }
    case Key_Home: 
      {
	horizontalScrollBar()->setValue( horizontalScrollBar()->minValue() );
	if (isCommand(text(curLine))) {
	  setCursorPosition(curLine, SIZEPR);
	  if ( curCol > SIZEPR && shftPressed )
	    setSelection( curLine, SIZEPR, curLine, curCol );
	  else
	    selectAll( false );
	}
	else moveCursor(QTextEdit::MoveLineStart, shftPressed);
	break;
      }
    case Key_End:
      {
	moveCursor(QTextEdit::MoveLineEnd, shftPressed);
	break;
      }  
    case Key_Backspace :
      {
	if ((curLine == endLine) && (curCol > SIZEPR))
	  QTextEdit::keyPressEvent( e );
	break;
      }
    case Key_Delete :
      {
	if ((curLine == endLine) && (curCol > SIZEPR-1))
	  QTextEdit::keyPressEvent( e );
	break;
      }
    case Key_Insert :
      {
	if ( ctrlPressed )
	  copy();
	else if ( shftPressed ) {
	  moveCursor(QTextEdit::MoveEnd, false);
	  paste();
	}
	else
	  QTextEdit::keyPressEvent( e );
	break;
      }
    }
  if ( e->key() == Key_C && ( e->state() & ControlButton ) )
    {
      _buf.truncate(0);
      setText("\n");
      _currentPrompt = ">>> ";
      setText(_currentPrompt);
    }

  // NRI : DEBUG PAS TERRIBLE //
  if (( e->key() == Key_F3) || 
      ( e->key() == Key_F4) ||
      ( e->key() == Key_Return) ||
      ( e->key() == Key_Escape))
    QAD_Application::getDesktop()->onKeyPress( e );
  // NRI //
}

void QAD_PyEditor::customEvent(QCustomEvent *e)
{
  switch( e->type() ) {
  case PYTHON_OK:
  case PYTHON_ERROR:
    {
      _buf.truncate(0);
      setText(_interp->getvout().c_str());
      setText(_interp->getverr().c_str());
      _currentPrompt = ">>> ";
      setText(_currentPrompt);
      break;
    }
  case PYTHON_INCOMPLETE:
    {
      _buf.append("\n");
      _currentPrompt = "... ";
      setText(_currentPrompt);
      break;
    }
  default:
    QTextEdit::customEvent( e );
  }

  setReadOnly( false );
  _isInHistory = false;
}
