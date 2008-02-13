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

#include "BatchLight_BatchManager_PBS.hxx"
#include "utilities.h"
#include "BatchLight_Job.hxx"
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

using namespace std;

namespace BatchLight {

  // Constructeur
  BatchManager_PBS::BatchManager_PBS(const batchParams& p) throw(SALOME_Exception) : BatchManager(p)
  {
    // pbs batch system needs to know mpi implementation
    _mpiImpl = FactoryMpiImpl(_params.mpiImpl);
  }

  // Destructeur
  BatchManager_PBS::~BatchManager_PBS()
  {
    MESSAGE("BatchManager_PBS destructor "<<_params.hostname);
  }

  // Methode pour le controle des jobs : retire un job du gestionnaire
  void BatchManager_PBS::deleteJob(const int & jobid)
  {
    BEGIN_OF("BatchManager_PBS::deleteJob");
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
      throw SALOME_Exception("Unknown protocol");

    if (_params.username != ""){
      command += _params.username;
      command += "@";
    }

    command += _params.hostname;
    command += " \"qdel " ;
    command += oss.str();
    command += "\"";
    SCRUTE(command.c_str());
    status = system(command.c_str());
    if(status)
      throw SALOME_Exception("Error of connection on remote host");

    MESSAGE("jobId = " << jobid << "killed");
    END_OF("BatchManager_PBS::deleteJob");
  }
   
  // Methode pour le controle des jobs : renvoie l'etat du job
  string BatchManager_PBS::queryJob(const int & jobid)
  {
    BEGIN_OF("BatchManager_PBS::queryJob");
    // define name of log file
    string jstatus;
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
      throw SALOME_Exception("Unknown protocol");

    if (_params.username != ""){
      command += _params.username;
      command += "@";
    }

    command += _params.hostname;
    command += " \"qstat -f " ;
    //ostringstream oss2;
    //oss2 << jobid;
    //command += oss2.str();
    command += _pbs_job_name[jobid];
    command += "\" > ";
    command += logFile;
    SCRUTE(command.c_str());
    status = system(command.c_str());
    if(status && status != 153 && status != 256*153){
      MESSAGE("status="<<status);
      throw SALOME_Exception("Error of connection on remote host");
    }

    if(status == 153 || status == 256*153 )
      // If job is finished qstat command return 153 status
      jstatus = "D";
    else{
      // read status of job in log file
      char line[128];
      ifstream fp(logFile.c_str(),ios::in);
      
      string sline;
      int pos = string::npos;
      while( (pos == string::npos) && fp.getline(line,80,'\n') ){
	sline = string(line);
	pos = sline.find("job_state");
      };
      
      if(pos!=string::npos){
	istringstream iss(sline);
	iss >> jstatus;
	iss >> jstatus;
	iss >> jstatus;
      }
      else
	jstatus = "U";
    }

    MESSAGE("jobId = " << jobid << " " << jstatus);
    END_OF("BatchManager_PBS::queryJob");
    return jstatus;
  }

  void BatchManager_PBS::buildSalomeCouplingScript(BatchLight::Job* job) throw(SALOME_Exception)
  {
    BEGIN_OF("BatchManager_PBS::buildSalomeCouplingScript");
    int status;
    const char *fileToExecute = job->getFileToExecute();
    const std::string dirForTmpFiles = job->getDirForTmpFiles();
    int idx = dirForTmpFiles.find("Batch/");
    std::string filelogtemp = dirForTmpFiles.substr(idx+6, dirForTmpFiles.length());

    string::size_type p1 = string(fileToExecute).find_last_of("/");
    string::size_type p2 = string(fileToExecute).find_last_of(".");
    std::string fileNameToExecute = string(fileToExecute).substr(p1+1,p2-p1-1);
    std::string TmpFileName = BuildTemporaryFileName();

    ofstream tempOutputFile;
    tempOutputFile.open(TmpFileName.c_str(), ofstream::out );
    tempOutputFile << "#! /bin/sh -f" << endl ;
    tempOutputFile << "cd " ;
    tempOutputFile << _params.applipath << endl ;
    tempOutputFile << "export PYTHONPATH=~/" ;
    tempOutputFile << dirForTmpFiles ;
    tempOutputFile << ":$PYTHONPATH" << endl ;
    tempOutputFile << "if test " ;
    tempOutputFile << _mpiImpl->rank() ;
    tempOutputFile << " = 0; then" << endl ;
    tempOutputFile << "  ./runAppli --terminal --modules=" ;
    for ( int i = 0 ; i < _params.modulesList.size() ; i++ ) {
      tempOutputFile << _params.modulesList[i] ;
      if ( i != _params.modulesList.size()-1 )
	tempOutputFile << "," ;
    }
    //tempOutputFile << " --standalone=registry,study,moduleCatalog --killall &" << endl ;
    tempOutputFile << " --standalone=registry,study,moduleCatalog --ns-port-log="
		   << filelogtemp 
		   << " &\n";
    tempOutputFile << "  for ((ip=1; ip < ";
    tempOutputFile << _mpiImpl->size();
    tempOutputFile << " ; ip++))" << endl;
    tempOutputFile << "  do" << endl ;
    tempOutputFile << "    arglist=\"$arglist YACS_Server_\"$ip" << endl ;
    tempOutputFile << "  done" << endl ;
    tempOutputFile << "  sleep 5" << endl ;
    tempOutputFile << "  ./runSession waitContainers.py $arglist" << endl ;
    tempOutputFile << "  ./runSession python ~/" << dirForTmpFiles << "/" << fileNameToExecute << ".py" << endl;

    //tempOutputFile << "  ./runSession killCurrentPort" << endl;
    tempOutputFile << "  if [ -f \"" << filelogtemp << "\" ]\n"
		   << "  then\n"
		   << "    port=`cat " << filelogtemp << "`\n"
		   << "    rm " << filelogtemp << "\n"
		   << "  fi\n"
		   << "  ./runSession killSalomeWithPort.py $port\n";
    
    tempOutputFile << "else" << endl ;
    tempOutputFile << "  sleep 5" << endl ;
    tempOutputFile << "  ./runSession waitNS.py" << endl ;
    tempOutputFile << "  ./runSession SALOME_Container 'YACS_Server_'";
    tempOutputFile << _mpiImpl->rank() << endl ;
    tempOutputFile << "fi" << endl ;
    tempOutputFile.flush();
    tempOutputFile.close();
    chmod(TmpFileName.c_str(), 0x1ED);
    SCRUTE(TmpFileName.c_str()) ;

    string command;
    if( _params.protocol == "rsh" )
      command = "rcp ";
    else if( _params.protocol == "ssh" )
      command = "scp ";
    else
      throw SALOME_Exception("Unknown protocol");
    
    command += TmpFileName;
    command += " ";
    if (_params.username != ""){
      command += _params.username;
      command += "@";
    }
    command += _params.hostname;
    command += ":";
    command += dirForTmpFiles ;
    command += "/runSalome_" ;
    command += fileNameToExecute ;
    command += "_Batch.sh" ;
    SCRUTE(fileNameToExecute) ;
    SCRUTE(command.c_str());
    status = system(command.c_str());
    if(status)
      throw SALOME_Exception("Error of connection on remote host");    
    RmTmpFile(TmpFileName);
    
    END_OF("BatchManager_PBS::buildSalomeCouplingScript");
  }

  void BatchManager_PBS::buildSalomeBatchScript(BatchLight::Job* job) throw(SALOME_Exception)
  {
    BEGIN_OF("BatchManager_PBS::buildSalomeBatchScript");
    int status;
    const int nbproc = job->getNbProc();
    const std::string dirForTmpFiles = job->getDirForTmpFiles();
    const char *fileToExecute = job->getFileToExecute();
    string::size_type p1 = string(fileToExecute).find_last_of("/");
    string::size_type p2 = string(fileToExecute).find_last_of(".");
    std::string fileNameToExecute = string(fileToExecute).substr(p1+1,p2-p1-1);

    int nbmaxproc = _params.nbnodes * _params.nbprocpernode;
    if( nbproc > nbmaxproc ){
      MESSAGE(nbproc << " processors asked on a cluster of " << nbmaxproc << " processors");
      throw SALOME_Exception("Too much processors asked for that cluster");
    }

    int nbnodes;
    if( nbproc < _params.nbnodes )
      nbnodes = nbproc;
    else
      nbnodes = _params.nbnodes;

    std::string TmpFileName = BuildTemporaryFileName();
    ofstream tempOutputFile;
    tempOutputFile.open(TmpFileName.c_str(), ofstream::out );

    ostringstream filenameToExecute;
    filenameToExecute << " ~/" << dirForTmpFiles << "/runSalome_" << fileNameToExecute << "_Batch.sh";

    tempOutputFile << "#! /bin/sh -f" << endl ;
    tempOutputFile << "#PBS -l nodes=" << nbnodes << endl ;
    tempOutputFile << "#PBS -o /$PBS_O_HOME/" << dirForTmpFiles << "/runSalome.output.log" << endl ;
    tempOutputFile << "#PBS -e /$PBS_O_HOME/" << dirForTmpFiles << "/runSalome.error.log" << endl ;
    tempOutputFile << _mpiImpl->boot("${PBS_NODEFILE}",nbnodes);
    tempOutputFile << _mpiImpl->run("${PBS_NODEFILE}",nbproc,filenameToExecute.str());
    tempOutputFile << _mpiImpl->halt();
    tempOutputFile.flush();
    tempOutputFile.close();
    chmod(TmpFileName.c_str(), 0x1ED);
    SCRUTE(TmpFileName.c_str()) ;

    string command;
    if( _params.protocol == "rsh" )
      command = "rcp ";
    else if( _params.protocol == "ssh" )
      command = "scp ";
    else
      throw SALOME_Exception("Unknown protocol");
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
    command += fileNameToExecute ;
    command += "_Batch.sh" ;
    SCRUTE(command.c_str());
    status = system(command.c_str());
    if(status)
      throw SALOME_Exception("Error of connection on remote host");    
  
    // Adding log files into import list files
    ostringstream file_name_output;
    file_name_output << "~/" << dirForTmpFiles << "/" << "runSalome.output.log";
    ostringstream file_name_error;
    file_name_error << "~/" << dirForTmpFiles << "/" << "runSalome.error.log";
    job->addFileToImportList(file_name_output.str());
    job->addFileToImportList(file_name_error.str());
    RmTmpFile(TmpFileName);
    END_OF("BatchManager_PBS::buildSalomeBatchScript");
  }

  int BatchManager_PBS::submit(BatchLight::Job* job) throw(SALOME_Exception)
  {
    BEGIN_OF("BatchManager_PBS::submit");
    const std::string dirForTmpFiles = job->getDirForTmpFiles();
    const char *fileToExecute = job->getFileToExecute();
    string::size_type p1 = string(fileToExecute).find_last_of("/");
    string::size_type p2 = string(fileToExecute).find_last_of(".");
    std::string fileNameToExecute = string(fileToExecute).substr(p1+1,p2-p1-1);

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
      throw SALOME_Exception("Unknown protocol");

    if (_params.username != ""){
      command += _params.username;
      command += "@";
    }

    command += _params.hostname;
    command += " \"qsub " ;
    command += dirForTmpFiles ;
    command += "/" ;
    command += fileNameToExecute ;
    command += "_Batch.sh\" > ";
    command += logFile;
    SCRUTE(command.c_str());
    status = system(command.c_str());
    if(status)
      throw SALOME_Exception("Error of connection on remote host");

    // read id of submitted job in log file
    char line[128];
    FILE *fp = fopen(logFile.c_str(),"r");
    fgets( line, 128, fp);
    fclose(fp);
    
    string sline(line);
    int pos = sline.find(".");
    string strjob;
    if(pos == string::npos)
      strjob = sline;
    else
      strjob = sline.substr(0,pos);

    int id;
    istringstream iss(strjob);
    iss >> id;

    // Ajout dans la map
    _pbs_job_name[id] = sline;
    END_OF("BatchManager_PBS::submit");
    return id;
  }

}
