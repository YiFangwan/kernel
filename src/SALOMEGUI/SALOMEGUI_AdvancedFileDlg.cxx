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
//  File   : SALOMEGUI_AdvancedFileDlg.cxx
//  Author : Michael Zorin
//  Module : SALOME
//  $Header: 

#include <qapplication.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstring.h>
#include "QAD_Config.h"
#include "SALOMEGUI_AdvancedFileDlg.h"

using namespace std;

bool SALOMEGUI_AdvancedFileDlg::IsChecked = false; 

/*!
Constructor
*/
SALOMEGUI_AdvancedFileDlg::SALOMEGUI_AdvancedFileDlg( QWidget* parent, bool open, const QString& cbTitle, bool showQuickDir, bool modal ) :
  QAD_FileDlg( parent, open, showQuickDir,  modal )
{ 
  myCheckBox = new QCheckBox( cbTitle, this );
  QLabel* label = new QLabel("", this);
  label->setMaximumWidth(0);
  QPushButton* pb = new QPushButton(this);               
  pb->setMaximumWidth(0);
  addWidgets( label, myCheckBox, pb );
  myCheckBox->setChecked(false);
}

/*!
  Destructor
*/
SALOMEGUI_AdvancedFileDlg::~SALOMEGUI_AdvancedFileDlg() 
{
}

/*!
  Processes selection : tries to set given path or filename as selection
*/
bool SALOMEGUI_AdvancedFileDlg::processPath( const QString& path )
{
  if ( !path.isNull() ) {
    QFileInfo fi( path );
    if ( fi.exists() ) {
      if ( fi.isFile() )
	setSelection( path );
      else if ( fi.isDir() )
	setDir( path );
      return true;
    }
    else {
      if ( QFileInfo( fi.dirPath() ).exists() ) {
	setDir( fi.dirPath() );
	return true;
      }
    }
  }
  return false;
}

/*!
  Returns the file name
*/
QString SALOMEGUI_AdvancedFileDlg::getFileName( QWidget*           parent, 
						const QString&     initial, 
						const QStringList& filters, 
						const QString&     caption,
						const QString&     cbTitle,
						bool               open,
						bool               showQuickDir,
						QAD_FileValidator* validator )
{            
  SALOMEGUI_AdvancedFileDlg* fd = new SALOMEGUI_AdvancedFileDlg( parent, open, cbTitle, showQuickDir, true );    
  if ( !caption.isEmpty() )
    fd->setCaption( caption );
  if ( !initial.isEmpty() ) { 
    fd->processPath( initial ); // VSR 24/03/03 check for existing of directory has been added to avoid QFileDialog's bug
  }
  fd->setFilters( filters );        
  if ( validator )
    fd->setValidator( validator );
  fd->exec();
  QString filename = fd->selectedFile();

  SALOMEGUI_AdvancedFileDlg::IsChecked = fd->IsCBChecked();
  
  delete fd;
  qApp->processEvents();
  
  return filename;
}

bool SALOMEGUI_AdvancedFileDlg::IsCBChecked() 
{
  return myCheckBox->isChecked();
}
