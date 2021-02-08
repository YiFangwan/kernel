// Copyright (C) 2007-2020  CEA/DEN, EDF R&D, OPEN CASCADE, CSGROUP
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//  File      : SALOMEDS_Tool.hxx
//  Created   : Mon Oct 21 16:24:50 2002
//  Author    : Sergey RUIN
//  Project   : SALOME
//  Module    : SALOMEDS
//
#ifndef __SALOMEDS_Tool_H__
#define __SALOMEDS_Tool_H__

#include <string>
#include <list>
#include <vector>
#include <stdlib.h>


// IDL headers
#include "SALOMEDSImpl_TMPFile.hxx"
#ifndef DISABLE_ORB
#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(SALOMEDS)
#endif

#ifdef WIN32
# if defined TOOLSDS_EXPORTS
#  define TOOLSDS_EXPORT __declspec( dllexport )
# else
#  define TOOLSDS_EXPORT __declspec( dllimport )
# endif
#else
# define TOOLSDS_EXPORT
#endif

#ifdef DISABLE_ORB
namespace SALOMEDSImpl_Tool {

 TOOLSDS_EXPORT class TMPFile: public SALOMEDSImpl_TMPFile
  {
  public:
    explicit TMPFile(long buffSize, unsigned char* streamBuff, bool isRelative):
      _myStreamBuff(streamBuff), _myBuffSize(buffSize), _myIsRelative(isRelative)
    {}
    inline long  length() const
    {return _myBuffSize;}

    inline unsigned char* NP_data() const
    {return _myStreamBuff;}

    inline bool IsRelative() const
    {return _myIsRelative;}
    TMPFile() = default;
    ~TMPFile() = default;

  private:
    unsigned char* _myStreamBuff;
    unsigned _myBuffSize;
    bool _myIsRelative;

    virtual size_t Size() override
    {return 0;}
    virtual TOctet& Get(size_t) override
    {return _myStreamBuff[0];}
  };
}
#endif

class TOOLSDS_EXPORT SALOMEDS_Tool
{
public:

  typedef std::vector<std::string> ListOfFiles;

  // Returns the unique temporary directory, that is defined in SALOME_TMP_DIR if this variable is set
  // otherwise return /tmp/something/ for Unix or c:\something\ for WIN32
  static std::string GetTmpDir();


  // Removes files which are in <theDirectory>, the files for deletion are listed in <theFiles>
  // if <IsDirDeleted> is true <theDirectory> is also deleted if it is empty
  static void RemoveTemporaryFiles(const std::string& theDirectory,
                                   const ListOfFiles& theFiles,
                                   const bool IsDirDeleted);

  // Returns the name by the path
  // for an example: if thePath = "/tmp/aaa/doc1.hdf" the function returns "doc1"
  static std::string GetNameFromPath(const std::string& thePath);

  // Returns the directory by the path
  // for an example: if thePath = "/tmp/aaa/doc1.hdf" the function returns "/tmp/aaa"
  static std::string GetDirFromPath(const std::string& thePath);

  #ifndef DISABLE_ORB
  // Converts files listed in <theFiles> which are in <theFromDirectory> into a byte sequence TMPFile
  static SALOMEDS::TMPFile* PutFilesToStream(const std::string& theFromDirectory,
                                             const ListOfFiles& theFiles,
                                             const int theNamesOnly = 0);

  // Converts files listed in <theFiles> which will be named as pointed in the <theFileNames> into a byte sequence TMPFile
  static SALOMEDS::TMPFile* PutFilesToStream(const ListOfFiles& theFiles,
                                             const ListOfFiles& theFileNames);
  // Converts a byte sequence <theStream> to files and places them in <theToDirectory>
  static ListOfFiles PutStreamToFiles(const SALOMEDS::TMPFile& theStream,
                                                 const std::string& theToDirectory,
                                                 const int theNamesOnly = 0);

  // Retrieves specified flaf from "AttributeFlags" attribute
  static bool GetFlag( const int             theFlag,
                       SALOMEDS::Study_var   theStudy,
                       SALOMEDS::SObject_var theObj );

  // Sets/Unsets specified flaf from "AttributeFlags" attribute
  static bool SetFlag( const int           theFlag,
                       SALOMEDS::Study_var theStudy,
                       const std::string&  theEntry,
                       const bool          theValue );

  // Get all children of object. If theObj is null all objects of study are returned
  static void GetAllChildren( SALOMEDS::Study_var               theStudy,
                              SALOMEDS::SObject_var             theObj,
                              std::list<SALOMEDS::SObject_var>& theList );
#else
  static SALOMEDSImpl_Tool::TMPFile* PutFilesToStreamImpl(const std::string& theFromDirectory,
                                             const ListOfFiles& theFiles,
                                             const int theNamesOnly = 0);

  static SALOMEDSImpl_Tool::TMPFile* PutFilesToStreamImpl(const ListOfFiles& theFiles,
                                             const ListOfFiles& theFileNames);

  static ListOfFiles PutStreamToFilesImpl(const SALOMEDSImpl_Tool::TMPFile& theStream,
                                                 const std::string& theToDirectory,
                                                 const int theNamesOnly = 0);
#endif
};
#endif




















