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
 * BatchManager.hxx : 
 *
 * Auteur : Bernard SECHER - CEA/DEN
 * Date   : Juillet 2007
 * Projet : SALOME
 *
 */

#ifndef _BL_BATCHMANAGER_H_
#define _BL_BATCHMANAGER_H_

#include <vector>
#include <string>
#include "Utils_SALOME_Exception.hxx"
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)

namespace BatchLight {

  class Job;

  struct batchParams{
    std::string hostname; // serveur ou tourne le BatchManager
    std::string protocol; // protocole d'acces au serveur: ssh ou rsh
    std::string username; // username d'acces au serveur
    std::string applipath; // path of apllication directory on server
    std::vector<std::string> modulesList; // list of Salome modules installed on server
  };

  class BatchManager
  {
  public:
    // Constructeur et destructeur
    BatchManager(const batchParams& p) throw(SALOME_Exception); // connexion a la machine host
    virtual ~BatchManager();

    // Methodes pour le controle des jobs : virtuelles pures
    virtual const int submitJob(BatchLight::Job & job) = 0; // soumet un job au gestionnaire
    virtual void deleteJob(const int & jobid) = 0; // retire un job du gestionnaire
    virtual int queryJob(const int & jobid) = 0; // renvoie l'etat du job

  protected:
    batchParams _params; 
    std::string _dirForTmpFiles; // repertoire temporaire sur le serveur
    std::string _TmpFileName;
    std::string _fileNameToExecute;

    void setDirForTmpFiles();
    void exportInFiles( const char *fileToExecute, const Engines::FilesToExportList filesToExportList ) throw(SALOME_Exception);
    virtual void buildSalomeCouplingScript( const char *fileToExecute ) throw(SALOME_Exception) = 0;
    virtual void buildSalomeBatchScript( const int nbproc ) throw(SALOME_Exception) = 0;
    virtual void buildSalomeSubmitBatchScript() throw(SALOME_Exception) = 0;
    void submit() throw(SALOME_Exception);

    std::string BuildTemporaryFileName() const;
    void RmTmpFile();

  private:

  };

}

#endif
