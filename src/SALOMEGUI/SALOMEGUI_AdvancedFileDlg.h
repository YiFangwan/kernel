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
//  File   : SALOMEGUI_AdvancedFileDlg.h
//  Author : Michael Zorin
//  Module : SALOME
//  $Header: 

#ifndef SALOMEGUI_AdvancedFileDlg_H
#define SALOMEGUI_AdvancedFileDlg_H

#include "QAD_FileDlg.h"
#include <qcheckbox.h>

class SALOMEGUI_AdvancedFileDlg : public QAD_FileDlg
{
    Q_OBJECT

public:
    SALOMEGUI_AdvancedFileDlg( QWidget* parent, bool open, const QString& cbTitle, bool showQuickDir = true, bool modal = true );
    ~SALOMEGUI_AdvancedFileDlg();

public:    
    static bool IsChecked;
    static QString     getFileName( QWidget*           parent, 
				    const QString&     initial, 
				    const QStringList& filters, 
				    const QString&     caption,
				    const QString&     cbTitle,
				    bool               open,
				    bool               showQuickDir = true,
				    QAD_FileValidator* validator = 0);

private:
    QCheckBox* myCheckBox;    
    bool IsCBChecked();
    bool processPath( const QString& path );

};

#endif // SALOMEGUI_AdvancedFileDlg_H
