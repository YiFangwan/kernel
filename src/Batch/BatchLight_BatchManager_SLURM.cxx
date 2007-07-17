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
 * BatchManager.cxx : 
 *
 * Auteur : Bernard SECHER - CEA/DEN
 * Date   : Juillet 2007
 * Projet : SALOME
 *
 */

#include "BatchLight_BatchManager_SLURM.hxx"
#include "utilities.h"
#include "BatchLight_Job.hxx"
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

using namespace std;

namespace BatchLight {

  // Constructeur
  BatchManager_SLURM::BatchManager_SLURM(const batchParams& p) throw(SALOME_Exception) : BatchManager(p)
  {
  }

  // Destructeur
  BatchManager_SLURM::~BatchManager_SLURM()
  {
    // Nothing to do
  }

  // Methode pour le controle des jobs : soumet un job au gestionnaire
  const int BatchManager_SLURM::submitJob(Job & job)
  {
    BEGIN_OF("BatchManager_SLURM::submitJob");
    int id=0;

    setDirForTmpFiles();
    SCRUTE(_dirForTmpFiles);
    exportInFiles(job.getFileToExecute(),job.getFilesToExportList());
    buildSalomeCouplingScript(job.getFileToExecute());
    buildSalomeBatchScript(job.getNbProc());
    buildSalomeSubmitBatchScript();
    submit();
    END_OF("BatchManager_SLURM::submitJob");
    return id;
  }

  // Methode pour le controle des jobs : retire un job du gestionnaire
  void BatchManager_SLURM::deleteJob(const int & jobid)
  {
  }
   
  // Methode pour le controle des jobs : renvoie l'etat du job
  int BatchManager_SLURM::queryJob(const int & jobid)
  {
    int ji=0;
    return ji;
  }

  void BatchManager_SLURM::buildSalomeCouplingScript( const char *fileToExecute ) throw(SALOME_Exception)
  {
    BEGIN_OF("BatchManager_SLURM::buildSalomeCouplingScript");
    int status;
    int lenf = strlen( fileToExecute ) ;
    int i = lenf-1 ;
    while ( i >= 0 && fileToExecute[i] != '/' ) {
      i -= 1 ;
    }
    char * FileNameToExecute = new char[lenf-4-i] ;
    strncpy(FileNameToExecute , &fileToExecute[i+1] , lenf-4-i) ;
    _fileNameToExecute = string( FileNameToExecute ) ;
    delete FileNameToExecute ;
    SCRUTE(_fileNameToExecute) ;

    _TmpFileName = BuildTemporaryFileName();
    ofstream tempOutputFile;
    tempOutputFile.open(_TmpFileName.c_str(), ofstream::out );
    tempOutputFile << "#! /bin/sh -f" << endl ;
    tempOutputFile << "cd " ;
    tempOutputFile << _params.applipath << endl ;
    tempOutputFile << "export PYTHONPATH=~/" ;
    tempOutputFile << _dirForTmpFiles ;
    tempOutputFile << ":$PYTHONPATH" << endl ;
    tempOutputFile << "if test $SLURM_PROCID = 0; then" << endl ;
    tempOutputFile << "  ./runAppli --terminal --batch --modules=" ;
    for ( i = 0 ; i < _params.modulesList.size() ; i++ ) {
      tempOutputFile << _params.modulesList[i] ;
      if ( i != _params.modulesList.size()-1 )
	tempOutputFile << "," ;
    }
    tempOutputFile << " --standalone=registry,study,moduleCatalog --killall &" << endl ;
    tempOutputFile << "  for ((ip=1; ip < ${SLURM_NPROCS} ; ip++))" << endl;
    tempOutputFile << "  do" << endl ;
    tempOutputFile << "    arglist=\"$arglist YACS_Server_\"$ip" << endl ;
    tempOutputFile << "  done" << endl ;
    tempOutputFile << "  sleep 5" << endl ;
    tempOutputFile << "  ./runSession waitContainers.py $arglist" << endl ;
    tempOutputFile << "  ./runSession python ~/" << _dirForTmpFiles << "/" << _fileNameToExecute << ".py" << endl;
    tempOutputFile << "  ./runSession killCurrentPort" << endl;
    tempOutputFile << "else" << endl ;
    tempOutputFile << "  sleep 5" << endl ;
    tempOutputFile << "  ./runSession waitNS.py" << endl ;
    tempOutputFile << "  ./runSession SALOME_Container 'YACS_Server_'${SLURM_PROCID}" << endl ;
    tempOutputFile << "fi" << endl ;
    tempOutputFile.flush();
    tempOutputFile.close();
    chmod(_TmpFileName.c_str(), 0x1ED);
    SCRUTE(_TmpFileName.c_str()) ;

    string command;
    if( _params.protocol == "rsh" )
      command = "rcp ";
    else if( _params.protocol == "ssh" )
      command = "scp ";
    else
      throw SALOME_Exception("Unknown protocol");
    
    command += _TmpFileName;
    command += " ";
    if (_params.username != ""){
      command += _params.username;
      command += "@";
    }
    command += _params.hostname;
    command += ":";
    command += _dirForTmpFiles ;
    command += "/runSalome_" ;
    command += _fileNameToExecute ;
    command += "_Batch.sh" ;
    SCRUTE(command.c_str());
    status = system(command.c_str());
    if(status)
      throw SALOME_Exception("Error of connection on remote host");    
    RmTmpFile();
    
    END_OF("BatchManager_SLURM::buildSalomeCouplingScript");
  }

  void BatchManager_SLURM::buildSalomeBatchScript( const int nbproc ) throw(SALOME_Exception)
  {
    BEGIN_OF("BatchManager_SLURM::buildSalomeBatchScript");
    int status;
    _TmpFileName = BuildTemporaryFileName();
    ofstream tempOutputFile;
    tempOutputFile.open(_TmpFileName.c_str(), ofstream::out );

    tempOutputFile << "#! /bin/sh -f" << endl ;
    tempOutputFile << "#BSUB -n  " ;
    tempOutputFile << nbproc << endl ;
    tempOutputFile << "#BSUB -o runSalome.log%J" << endl ;
    tempOutputFile << "mpirun -srun ~/" ;
    tempOutputFile << _dirForTmpFiles ;
    tempOutputFile << "/runSalome_" ;
    tempOutputFile << _fileNameToExecute ;
    tempOutputFile << "_Batch.sh" << endl ;
    tempOutputFile.flush();
    tempOutputFile.close();
    chmod(_TmpFileName.c_str(), 0x1ED);
    SCRUTE(_TmpFileName.c_str()) ;

    string command;
    if( _params.protocol == "rsh" )
      command = "rcp ";
    else if( _params.protocol == "ssh" )
      command = "scp ";
    else
      throw SALOME_Exception("Unknown protocol");
    command += _TmpFileName;
    command += " ";
    if (_params.username != ""){
      command += _params.username;
      command += "@";
    }
    command += _params.hostname;
    command += ":";
    command += _dirForTmpFiles ;
    command += "/" ;
    command += _fileNameToExecute ;
    command += "_Batch.sh" ;
    SCRUTE(command.c_str());
    status = system(command.c_str());
    if(status)
      throw SALOME_Exception("Error of connection on remote host");    

    RmTmpFile();
    END_OF("BatchManager_SLURM::buildSalomeBatchScript");
    
  }

  void BatchManager_SLURM::buildSalomeSubmitBatchScript() throw(SALOME_Exception)
  {

    BEGIN_OF("BatchManager_SLURM::buildSalomeSubmitBatchScript");
    _TmpFileName = BuildTemporaryFileName();
    int status;
    ofstream tempOutputFile;
    tempOutputFile.open(_TmpFileName.c_str(), ofstream::out );

    tempOutputFile << "#! /bin/sh -f" << endl ;
    tempOutputFile << "bsub < ~/" ;
    tempOutputFile << _dirForTmpFiles ;
    tempOutputFile << "/" ;
    tempOutputFile << _fileNameToExecute ;
    tempOutputFile << "_Batch.sh &" << endl ;
    tempOutputFile.flush();
    tempOutputFile.close();
    chmod(_TmpFileName.c_str(), 0x1ED);
    SCRUTE(_TmpFileName.c_str()) ;

    string command;
    if( _params.protocol == "rsh" )
      command = "rcp ";
    else if( _params.protocol == "ssh" )
      command = "scp ";
    else
      throw SALOME_Exception("Unknown protocol");
    command += _TmpFileName;
    command += " ";
    if (_params.username != ""){
      command += _params.username;
      command += "@";
    }
    command += _params.hostname;
    command += ":";
    command += _dirForTmpFiles ;
    command += "/" ;
    command += _fileNameToExecute ;
    command += "_bsub.sh" ;
    SCRUTE(command.c_str());
    status = system(command.c_str());
    if(status)
      throw SALOME_Exception("Error of connection on remote host");    

    RmTmpFile();
    END_OF("BatchManager_SLURM::buildSalomeSubmitBatchScript");
    
  }

}
