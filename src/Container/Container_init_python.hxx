//  SALOME Container : implementation of container and engine for Kernel
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
//  File   : Container_init_python.hxx
//  Author : Paul RASCLE, EDF
//  Module : KERNEL
//  $Header$

#ifndef _CONTAINER_INIT_PYTHON_HXX_
#define _CONTAINER_INIT_PYTHON_HXX_

#include <pthread.h>  // must be before Python.h !
#include <Python.h>

struct  KERNEL_PYTHON
{
  static PyThreadState *_gtstate;
  static PyObject *salome_shared_modules_module;

  static void init_python(int argc, char **argv);

};

#endif
