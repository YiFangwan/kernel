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
#include "BatchLight_Job.hxx"
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

using namespace std;

namespace BatchLight {

  // Constructeur
  BatchManager_SLURM::BatchManager_SLURM(const clusterParams& p) throw(BatchException) : BatchManager(p)
  {
    _mpiImpl = FactoryMpiImpl(_params.mpiImpl);
  }

  // Destructeur
  BatchManager_SLURM::~BatchManager_SLURM()
  {
    cerr << "BatchManager_SLURM destructor "<<_params.hostname << endl;
  }

  // Methode pour le controle des jobs : retire un job du gestionnaire
  void BatchManager_SLURM::deleteJob(const int & jobid)
  {
    string command;
    int status;
    ostringstream oss;
    oss << jobid;

    // define command to submit batch
    if( _params.protocol == "rsh" )
      command = "rsh ";
    else if( _params.protocol == "ssh" )
      command = "ssh ";
    else
      throw BatchException("Unknown protocol");

    if (_params.username != ""){
      command += _params.username;
      command += "@";
    }

    command += _params.hostname;
    command += " \"bkill " ;
    command += oss.str();
    command += "\"";
    cerr << command.c_str() << endl;
    status = system(command.c_str());
    if(status)
      throw BatchException("Error of connection on remote host");

    cerr << "jobId = " << jobid << "killed" << endl;
  }
   
  // Methode pour le controle des jobs : renvoie l'etat du job
  string BatchManager_SLURM::queryJob(const int & jobid)
  {
    // define name of log file
    string logFile="/tmp/logs/";
    logFile += getenv("USER");
    logFile += "/batchSalome_";

    srand ( time(NULL) );
    int ir = rand();
    ostringstream oss;
    oss << ir;
    logFile += oss.str();
    logFile += ".log";

    string command;
    int status;

    // define command to submit batch
    if( _params.protocol == "rsh" )
      command = "rsh ";
    else if( _params.protocol == "ssh" )
      command = "ssh ";
    else
      throw BatchException("Unknown protocol");

    if (_params.username != ""){
      command += _params.username;
      command += "@";
    }

    command += _params.hostname;
    command += " \"bjobs " ;
    ostringstream oss2;
    oss2 << jobid;
    command += oss2.str();
    command += "\" > ";
    command += logFile;
    cerr << command.c_str() << endl;
    status = system(command.c_str());
    if(status)
      throw BatchException("Error of connection on remote host");

    // read staus of job in log file
    char line[128];
    ifstream fp(logFile.c_str(),ios::in);
    fp.getline(line,80,'\n');
    
    string sjobid, username, jstatus;
    fp >> sjobid;
    fp >> username;
    fp >> jstatus;

    cerr << "jobId = " << jobid << " " << jstatus << endl;
    return jstatus;
  }

  void BatchManager_SLURM::buildBatchScript(BatchLight::Job* job) throw(BatchException)
  {
    int status;
    const int nbproc = job->getNbProc();
    std::string edt = job->getExpectedDuringTime();
    std::string mem = job->getMemory();
    const std::string dirForTmpFiles = job->getDirForTmpFiles();
    const string fileToExecute = job->getFileToExecute();
    string::size_type p1 = fileToExecute.find_last_of("/");
    string::size_type p2 = fileToExecute.find_last_of(".");
    std::string rootNameToExecute = fileToExecute.substr(p1+1,p2-p1-1);
    std::string fileNameToExecute = "~/" + dirForTmpFiles + "/" + string(basename(fileToExecute.c_str()));

    int nbmaxproc = _params.nbnodes * _params.nbprocpernode;
    if( nbproc > nbmaxproc ){
      cerr << nbproc << " processors asked on a cluster of " << nbmaxproc << " processors" << endl;
      throw BatchException("Too much processors asked for that cluster");
    }

    int nbnodes;
    if( nbproc < _params.nbnodes )
      nbnodes = nbproc;
    else
      nbnodes = _params.nbnodes;

    std::string TmpFileName = BuildTemporaryFileName();
    ofstream tempOutputFile;
    tempOutputFile.open(TmpFileName.c_str(), ofstream::out );

    tempOutputFile << "#! /bin/sh -f" << endl ;
    tempOutputFile << "#BSUB -n " << nbproc << endl ;
    tempOutputFile << "#BSUB -o " << dirForTmpFiles << "/runSalome.log%J" << endl ;
    tempOutputFile << _mpiImpl->boot("",nbproc);
    tempOutputFile << _mpiImpl->run("",nbproc,fileNameToExecute);
    tempOutputFile << _mpiImpl->halt();
    tempOutputFile.flush();
    tempOutputFile.close();
    chmod(TmpFileName.c_str(), 0x1ED);
    cerr << TmpFileName.c_str() << endl;

    string command;
    if( _params.protocol == "rsh" )
      command = "rcp ";
    else if( _params.protocol == "ssh" )
      command = "scp ";
    else
      throw BatchException("Unknown protocol");
    command += TmpFileName;
    command += " ";
    if (_params.username != ""){
      command += _params.username;
      command += "@";
    }
    command += _params.hostname;
    command += ":";
    command += dirForTmpFiles ;
    command += "/" ;
    command += rootNameToExecute ;
    command += "_Batch.sh" ;
     cerr << command.c_str() << endl;
    status = system(command.c_str());
    if(status)
      throw BatchException("Error of connection on remote host");    

    RmTmpFile(TmpFileName);
    
  }

  int BatchManager_SLURM::submit(BatchLight::Job* job) throw(BatchException)
  {
    const std::string dirForTmpFiles = job->getDirForTmpFiles();
    const string fileToExecute = job->getFileToExecute();
    string::size_type p1 = fileToExecute.find_last_of("/");
    string::size_type p2 = fileToExecute.find_last_of(".");
    std::string fileNameToExecute = fileToExecute.substr(p1+1,p2-p1-1);

    // define name of log file
    string logFile="/tmp/logs/";
    logFile += getenv("USER");
    logFile += "/batchSalome_";

    srand ( time(NULL) );
    int ir = rand();
    ostringstream oss;
    oss << ir;
    logFile += oss.str();
    logFile += ".log";

    string command;
    int status;

    // define command to submit batch
    if( _params.protocol == "rsh" )
      command = "rsh ";
    else if( _params.protocol == "ssh" )
      command = "ssh ";
    else
      throw BatchException("Unknown protocol");

    if (_params.username != ""){
      command += _params.username;
      command += "@";
    }

    command += _params.hostname;
    command += " \"bsub < " ;
    command += dirForTmpFiles ;
    command += "/" ;
    command += fileNameToExecute ;
    command += "_Batch.sh\" > ";
    command += logFile;
    cerr << command.c_str() << endl;
    status = system(command.c_str());
    if(status)
      throw BatchException("Error of connection on remote host");

    // read id of submitted job in log file
    char line[128];
    FILE *fp = fopen(logFile.c_str(),"r");
    fgets( line, 128, fp);
    fclose(fp);
    
    string sline(line);
    int p10 = sline.find("<");
    int p20 = sline.find(">");
    string strjob = sline.substr(p10+1,p20-p10-1);

    int id;
    istringstream iss(strjob);
    iss >> id;

    return id;
  }

}
