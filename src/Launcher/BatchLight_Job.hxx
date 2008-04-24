// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
/*
 * Job.hxx : 
 *
 * Auteur : Bernard SECHER - CEA/DEN
 * Date   : Juillet 2007
 * Projet : SALOME
 *
 */

#ifndef _BL_JOB_H_
#define _BL_JOB_H_

#include <iostream>
#include "BatchLight_BatchManager.hxx"

namespace BatchLight {

  class Job
  {
  public:
    // Constructeurs et destructeur
    Job(const std::string fileToExecute, 
	const std::vector<std::string>& filesToExport, 
	const std::vector<std::string>& filesToImport, 
	const batchParams& batch_params);
    virtual ~Job();

    const std::string getFileToExecute() const { return _fileToExecute; }
    void setFileToExecute(const std::string fileToExecute) { _fileToExecute=fileToExecute; }
    const std::vector<std::string> getFilesToExportList() const { return _filesToExport; }
    const std::vector<std::string> getFilesToImportList() const { return _filesToImport; }
    void addFileToExportList(std::string file_name);
    void addFileToImportList(std::string file_name);
    const long getNbProc() const { return _batch_params.nb_proc; }
    const std::string getExpectedDuringTime();
    const std::string getMemory();

    const std::string getDirForTmpFiles() const { return _dirForTmpFiles;}
    void setDirForTmpFiles(std::string dirForTmpFiles) { _dirForTmpFiles = dirForTmpFiles;
      std::cerr << _dirForTmpFiles << std::endl;}
    bool check();							
  protected:
    std::string _fileToExecute;
    std::vector<std::string> _filesToExport;
    std::vector<std::string> _filesToImport;
    batchParams _batch_params;
    std::string _dirForTmpFiles; // Tmp directory on the server
  private:

  };

}

#endif
