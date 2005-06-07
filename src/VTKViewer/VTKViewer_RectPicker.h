//  SALOME VTKViewer : build VTK viewer into Salome desktop
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
//  File   : VTKViewer_RectPicker.h
//  Author : Natalia KOPNOVA
//  Module : SALOME

#ifndef __VTKViewer_RectPicker_h
#define __VTKViewer_RectPicker_h

#include "VTKViewer_Definitions.h"
#include <vtkPicker.h>

class VTK_EXPORT VTKViewer_RectPicker : public vtkPicker
{
public:
  static VTKViewer_RectPicker *New();
  vtkTypeMacro(VTKViewer_RectPicker,vtkPicker);
  
  // Description:
  // Perform pick operation with selection rectangle provided. Normally the 
  // first two values for the selection top-left and right-bottom points are 
  // x-y pixel coordinate, and the third value is =0. 
  // Return non-zero if something was successfully picked.
  virtual int Pick(_VTK_FLOAT_ selectionX1, _VTK_FLOAT_ selectionY1, _VTK_FLOAT_ selectionZ1, 
		   _VTK_FLOAT_ selectionX2, _VTK_FLOAT_ selectionY2, _VTK_FLOAT_ selectionZ2,
                   vtkRenderer *renderer);  

  // Description: 
  // Perform pick operation with selection rectangle provided. Normally the first
  // two values for the selection top-left and right-bottom points are x-y pixel 
  // coordinate, and the third value is =0. 
  // Return non-zero if something was successfully picked.
  int Pick(_VTK_FLOAT_ selectionPt1[3], _VTK_FLOAT_ selectionPt2[3], vtkRenderer *ren)
    {return this->Pick(selectionPt1[0], selectionPt1[1], selectionPt1[2], 
		       selectionPt2[0], selectionPt2[1], selectionPt2[2],
		       ren);};

  // Description:
  // Bounding box intersection with hexahedron. The method returns a non-zero value 
  // if the bounding box is hit. Origin[4][4] starts the ray from corner points, 
  // dir[4][3] is the vector components of the ray in the x-y-z directions. 
  // (Notes: the intersection ray dir[4][3] is NOT normalized.)
  static char HitBBox(_VTK_FLOAT_ bounds[6], _VTK_FLOAT_ origin[4][4], _VTK_FLOAT_ dir[4][3]);

  // Description:
  // Position of point relative to hexahedron. The method returns a non-zero value 
  // if the point is inside. p1[4][4] is the corner points of top face, 
  // p2[4][4] is the corner points of bottom face. 
  static char PointInside(_VTK_FLOAT_ point[3], _VTK_FLOAT_ p1[4][4], _VTK_FLOAT_ p2[4][4], _VTK_FLOAT_ tol=0);

protected:
  VTKViewer_RectPicker();
  ~VTKViewer_RectPicker() {};

  virtual _VTK_FLOAT_ IntersectWithHex(_VTK_FLOAT_ p1[4][4], _VTK_FLOAT_ p2[4][4], _VTK_FLOAT_ tol, 
                          vtkAssemblyPath *path, vtkProp3D *p, 
                          vtkAbstractMapper3D *m);

private:
};

#endif


