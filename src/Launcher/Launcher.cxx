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
#include "BatchLight_BatchManager_PBS.hxx"
#include "BatchLight_BatchManager_SLURM.hxx"
#include "BatchLight_Job.hxx"
#include "Launcher.hxx"
#include <iostream>
#include <sstream>
#include <sys/stat.h>

using namespace std;

//=============================================================================
/*! 
 *  Constructor
 *  \param orb
 *  Define a CORBA single thread policy for the server, which avoid to deal
 *  with non thread-safe usage like Change_Directory in SALOME naming service
 */
//=============================================================================

Launcher_cpp::Launcher_cpp()
{
  cerr << "Launcher_cpp constructor" << endl;
}

//=============================================================================
/*! 
 * destructor
 */
//=============================================================================

Launcher_cpp::~Launcher_cpp()
{
  cerr << "Launcher_cpp destructor" << endl;
  std::map < string, BatchLight::BatchManager * >::const_iterator it;
  for(it=_batchmap.begin();it!=_batchmap.end();it++)
    delete it->second;
}

//=============================================================================
/*! CORBA Method:
 *  Submit a batch job on a cluster and returns the JobId
 *  \param fileToExecute      : .py/.exe/.sh/... to execute on the batch cluster
 *  \param filesToExport      : to export on the batch cluster
 *  \param NumberOfProcessors : Number of processors needed on the batch cluster
 *  \param params             : Constraints for the choice of the batch cluster
 */
//=============================================================================
long Launcher_cpp::submitSalomeJob( const string fileToExecute ,
				    const vector<string>& filesToExport ,
				    const vector<string>& filesToImport ,
				    const BatchLight::batchParams& batch_params,
				    const machineParams& params) throw(LauncherException)
{
  cerr << "BEGIN OF Launcher_cpp::submitSalomeJob" << endl;
  long jobId;
  vector<string> aMachineList;

  // find a cluster matching the structure params
  vector<string> aCompoList ;
  try{
    aMachineList = _ResManager->GetFittingResources(params, aCompoList);
  }
  catch(const ResourcesException &ex){
    throw LauncherException(ex.msg.c_str());
  }
  if (aMachineList.size() == 0)
    throw LauncherException("No resources have been found with your parameters");

  ParserResourcesType p = _ResManager->GetResourcesList(aMachineList[0]);
  string clustername(p.Alias);
  cerr << "Choose cluster: " <<  clustername << endl;
  
  // search batch manager for that cluster in map or instanciate one
  map < string, BatchLight::BatchManager * >::const_iterator it = _batchmap.find(clustername);
  if(it == _batchmap.end())
    {
      _batchmap[clustername] = FactoryBatchManager(p);
      // TODO: Add a test for the cluster !
    }
    
  try{
    // create and submit job on cluster
    BatchLight::Job* job = new BatchLight::Job(fileToExecute, filesToExport, filesToImport, batch_params);
    bool res = job->check();
    if (!res) {
      delete job;
      throw LauncherException("Job parameters are bad (see informations above)");
    }

    // build salome coupling script for job
    buildSalomeCouplingScript(job,p);

    // submit job on cluster
    jobId = _batchmap[clustername]->submitJob(job);
  }
  catch(const BatchLight::BatchException &ex){
    throw LauncherException(ex.msg.c_str());
  }

  return jobId;
}

//=============================================================================
/*! CORBA Method:
 *  Query a batch job on a cluster and returns the status of job
 *  \param jobId              : identification of Salome job
 *  \param params             : Constraints for the choice of the batch cluster
 */
//=============================================================================
string Launcher_cpp::querySalomeJob( long jobId, 
				    const machineParams& params) throw(LauncherException)
{
  // find a cluster matching params structure
  vector<string> aCompoList ;
  vector<string> aMachineList = _ResManager->GetFittingResources( params , aCompoList ) ;
  ParserResourcesType p = _ResManager->GetResourcesList(aMachineList[0]);
  string clustername(p.Alias);
    
  // search batch manager for that cluster in map
  std::map < string, BatchLight::BatchManager * >::const_iterator it = _batchmap.find(clustername);
  if(it == _batchmap.end())
    throw LauncherException("no batchmanager for that cluster");
    
  return _batchmap[clustername]->queryJob(jobId);
}

//=============================================================================
/*! CORBA Method:
 *  Delete a batch job on a cluster 
 *  \param jobId              : identification of Salome job
 *  \param params             : Constraints for the choice of the batch cluster
 */
//=============================================================================
void Launcher_cpp::deleteSalomeJob( const long jobId, 
				    const machineParams& params) throw(LauncherException)
{
  // find a cluster matching params structure
  vector<string> aCompoList ;
  vector<string> aMachineList = _ResManager->GetFittingResources( params , aCompoList ) ;
  ParserResourcesType p = _ResManager->GetResourcesList(aMachineList[0]);
  string clustername(p.Alias);
    
  // search batch manager for that cluster in map
  map < string, BatchLight::BatchManager * >::const_iterator it = _batchmap.find(clustername);
  if(it == _batchmap.end())
    throw LauncherException("no batchmanager for that cluster");
  
  _batchmap[clustername]->deleteJob(jobId);
}

//=============================================================================
/*! CORBA Method:
 *  Get result files of job on a cluster
 *  \param jobId              : identification of Salome job
 *  \param params             : Constraints for the choice of the batch cluster
 */
//=============================================================================
void Launcher_cpp::getResultSalomeJob( const string directory,
				       const long jobId, 
				       const machineParams& params) throw(LauncherException)
{
  vector<string> aCompoList ;
  vector<string> aMachineList = _ResManager->GetFittingResources( params , aCompoList ) ;
  ParserResourcesType p = _ResManager->GetResourcesList(aMachineList[0]);
  string clustername(p.Alias);
    
  // search batch manager for that cluster in map
  map < string, BatchLight::BatchManager * >::const_iterator it = _batchmap.find(clustername);
  if(it == _batchmap.end())
    throw LauncherException("no batchmanager for that cluster");
    
  _batchmap[clustername]->importOutputFiles( directory, jobId );
}

//=============================================================================
/*!
 *  Factory to instanciate the good batch manager for choosen cluster.
 */ 
//=============================================================================

BatchLight::BatchManager *Launcher_cpp::FactoryBatchManager( const ParserResourcesType& params ) throw(LauncherException)
{

  cerr << "Begin of Launcher_cpp::FactoryBatchManager" << endl;
  // Fill structure for batch manager
  BatchLight::clusterParams p;
  p.hostname = params.Alias;
  switch(params.Protocol){
  case rsh:
    p.protocol = "rsh";
    break;
  case ssh:
    p.protocol = "ssh";
    break;
  default:
    throw LauncherException("unknown protocol");
    break;
  }
  p.username = params.UserName;
  p.applipath = params.AppliPath;
  p.modulesList = params.ModulesList;
  p.nbnodes = params.DataForSort._nbOfNodes;
  p.nbprocpernode = params.DataForSort._nbOfProcPerNode;
  switch(params.mpi){
  case lam:
    p.mpiImpl = "lam";
    break;
  case mpich1:
    p.mpiImpl = "mpich1";
    break;
  case mpich2:
    p.mpiImpl = "mpich2";
    break;
  case openmpi:
    p.mpiImpl = "openmpi";
    break;
  case slurm:
    p.mpiImpl = "slurm";
    break;
  default:
    p.mpiImpl = "indif";
    break;
  }    

  cerr << "Instanciation of batch manager" << endl;
  switch( params.Batch ){
  case pbs:
    cerr << "Instantiation of PBS batch manager" << endl;
    return new BatchLight::BatchManager_PBS(p);
  case lsf:
    cerr << "Instantiation of SLURM batch manager" << endl;
    return new BatchLight::BatchManager_SLURM(p);
  default:
    cerr << "BATCH = " << params.Batch << endl;
    throw LauncherException("no batchmanager for that cluster");
  }
}

void Launcher_cpp::buildSalomeCouplingScript(BatchLight::Job* job, const ParserResourcesType& params)
{
  const string fileToExecute = job->getFileToExecute();
  const std::string dirForTmpFiles = job->getDirForTmpFiles();
  int idx = dirForTmpFiles.find("Batch/");
  std::string filelogtemp = dirForTmpFiles.substr(idx+6, dirForTmpFiles.length());

  string::size_type p1 = fileToExecute.find_last_of("/");
  string::size_type p2 = fileToExecute.find_last_of(".");
  std::string fileNameToExecute = fileToExecute.substr(p1+1,p2-p1-1);
  std::string TmpFileName = "/tmp/runSalome_" + fileNameToExecute + ".sh";

  MpiImpl* mpiImpl = FactoryMpiImpl(params.mpi);

  ofstream tempOutputFile;
  tempOutputFile.open(TmpFileName.c_str(), ofstream::out );

  // Begin
  tempOutputFile << "#! /bin/sh -f" << endl ;
  tempOutputFile << "cd " ;
  tempOutputFile << params.AppliPath << endl ;
  tempOutputFile << "export SALOME_BATCH=1\n";
  tempOutputFile << "export PYTHONPATH=~/" ;
  tempOutputFile << dirForTmpFiles ;
  tempOutputFile << ":$PYTHONPATH" << endl ;

  // Test node rank
  tempOutputFile << "if test " ;
  tempOutputFile << mpiImpl->rank() ;
  tempOutputFile << " = 0; then" << endl ;

  // -----------------------------------------------
  // Code for rank 0 : launch runAppli and a container
  // RunAppli
  tempOutputFile << "  ./runAppli --terminal --modules=" ;
  for ( int i = 0 ; i < params.ModulesList.size() ; i++ ) {
    tempOutputFile << params.ModulesList[i] ;
    if ( i != params.ModulesList.size()-1 )
      tempOutputFile << "," ;
  }
  tempOutputFile << " --standalone=registry,study,moduleCatalog --ns-port-log="
		 << filelogtemp 
		 << " &\n";

  // Wait NamingService
  tempOutputFile << "  current=0\n"
		 << "  stop=20\n" 
		 << "  while ! test -f " << filelogtemp << "\n"
		 << "  do\n"
		 << "    sleep 2\n"
		 << "    let current=current+1\n"
		 << "    if [ \"$current\" -eq \"$stop\" ] ; then\n"
		 << "      echo Error Naming Service failed ! >&2"
		 << "      exit\n"
		 << "    fi\n"
		 << "  done\n"
		 << "  port=`cat " << filelogtemp << "`\n";
    
  // Wait other containers
  tempOutputFile << "  for ((ip=1; ip < ";
  tempOutputFile << mpiImpl->size();
  tempOutputFile << " ; ip++))" << endl;
  tempOutputFile << "  do" << endl ;
  tempOutputFile << "    arglist=\"$arglist YACS_Server_\"$ip" << endl ;
  tempOutputFile << "  done" << endl ;
  tempOutputFile << "  sleep 5" << endl ;
  tempOutputFile << "  ./runSession waitContainers.py $arglist" << endl ;
  
  // Launch user script
  tempOutputFile << "  ./runSession python ~/" << dirForTmpFiles << "/" << fileNameToExecute << ".py" << endl;

  // Stop application
  tempOutputFile << "  rm " << filelogtemp << "\n"
		 << "  ./runSession killCurrentPort" << endl;
  // waiting standard killing improvement by P. Rascle
  tempOutputFile << "  killall notifd" << endl;
  tempOutputFile << "  killall omniNames" << endl;

  // -------------------------------------
  // Other nodes launch a container
  tempOutputFile << "else" << endl ;

  // Wait NamingService
  tempOutputFile << "  current=0\n"
		 << "  stop=20\n" 
		 << "  while ! test -f " << filelogtemp << "\n"
		 << "  do\n"
		 << "    sleep 2\n"
		 << "    let current=current+1\n"
		 << "    if [ \"$current\" -eq \"$stop\" ] ; then\n"
		 << "      echo Error Naming Service failed ! >&2"
		 << "      exit\n"
		 << "    fi\n"
		 << "  done\n"
		 << "  port=`cat " << filelogtemp << "`\n";

  // Launching container
  tempOutputFile << "  ./runSession SALOME_Container YACS_Server_";
  tempOutputFile << mpiImpl->rank()
		 << " > ~/" << dirForTmpFiles << "/YACS_Server_" 
		 << mpiImpl->rank() << "_container_log." << filelogtemp
		 << " 2>&1\n";
  tempOutputFile << "fi" << endl ;
  tempOutputFile.flush();
  tempOutputFile.close();
  chmod(TmpFileName.c_str(), 0x1ED);
  cerr << TmpFileName.c_str() << endl;

  job->addFileToExportList(fileToExecute);
  job->setFileToExecute(TmpFileName);

  delete mpiImpl;
    
}

MpiImpl *Launcher_cpp::FactoryMpiImpl(MpiImplType mpi) throw(LauncherException)
{
  switch(mpi){
  case lam:
    return new MpiImpl_LAM();
  case mpich1:
    return new MpiImpl_MPICH1();
  case mpich2:
    return new MpiImpl_MPICH2();
  case openmpi:
    return new MpiImpl_OPENMPI();
  case slurm:
    return new MpiImpl_SLURM();
  case indif:
    throw LauncherException("you must specify a mpi implementation in CatalogResources.xml file");
  default:
    ostringstream oss;
    oss << mpi << " : not yet implemented";
    throw LauncherException(oss.str().c_str());
  }

}
