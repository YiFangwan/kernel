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
#include "Batch_Date.hxx"
#include "Batch_FactBatchManager_eLSF.hxx"
#include "Batch_FactBatchManager_ePBS.hxx"
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
  std::map < string, Batch::BatchManager_eClient * >::const_iterator it1;
  for(it1=_batchmap.begin();it1!=_batchmap.end();it1++)
    delete it1->second;
  std::map < std::pair<std::string,long> , Batch::Job* >::const_iterator it2;
  for(it2=_jobmap.begin();it2!=_jobmap.end();it2++)
    delete it2->second;
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
				    const batchParams& batch_params,
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
  map < string, Batch::BatchManager_eClient * >::const_iterator it = _batchmap.find(clustername);
  if(it == _batchmap.end())
    {
      _batchmap[clustername] = FactoryBatchManager(p);
      // TODO: Add a test for the cluster !
    }
    
  try{
    // tmp directory on cluster to put files to execute
    string tmpdir = getTmpDirForBatchFiles();

    // create and submit job on cluster
    Batch::Parametre param;
    param[USER] = p.UserName;
    param[EXECUTABLE] = buildSalomeCouplingScript(fileToExecute,tmpdir,p);
    param[INFILE] = Batch::Couple( fileToExecute, getRemoteFile(tmpdir,fileToExecute) );
    for(int i=0;i<filesToExport.size();i++)
      param[INFILE] += Batch::Couple( filesToExport[i], getRemoteFile(tmpdir,filesToExport[i]) );
    if( filesToImport.size() > 0 ){
      param[OUTFILE] = Batch::Couple( "", filesToImport[0] );
      for(int i=1;i<filesToImport.size();i++)
	param[OUTFILE] += Batch::Couple( "", filesToImport[i] );
    }
    param[NBPROC] = batch_params.nb_proc;
    param[WORKDIR] = batch_params.batch_directory;
    param[TMPDIR] = tmpdir;
    param[MAXCPUTIME] = batch_params.expected_during_time;
    param[MAXRAMSIZE] = batch_params.mem;

    Batch::Environnement env;

    Batch::Job* job = new Batch::Job(param,env);

    // submit job on cluster
    Batch::JobId jid = _batchmap[clustername]->submitJob(*job);

    // get job id in long
    istringstream iss(jid.getReference());
    iss >> jobId;

    _jobmap[ pair<string,long>(clustername,jobId) ] = job;
  }
  catch(const Batch::EmulationException &ex){
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
string Launcher_cpp::querySalomeJob( long id, 
				     const machineParams& params) throw(LauncherException)
{
  // find a cluster matching params structure
  vector<string> aCompoList ;
  vector<string> aMachineList = _ResManager->GetFittingResources( params , aCompoList ) ;
  ParserResourcesType p = _ResManager->GetResourcesList(aMachineList[0]);
  string clustername(p.Alias);
    
  // search batch manager for that cluster in map
  std::map < string, Batch::BatchManager_eClient * >::const_iterator it = _batchmap.find(clustername);
  if(it == _batchmap.end())
    throw LauncherException("no batchmanager for that cluster");
    
  ostringstream oss;
  oss << id;
  Batch::JobId jobId( _batchmap[clustername], oss.str() );

  Batch::JobInfo jinfo = jobId.queryJob();
  Batch::Parametre par = jinfo.getParametre();
  return par[STATE];
}

//=============================================================================
/*! CORBA Method:
 *  Delete a batch job on a cluster 
 *  \param jobId              : identification of Salome job
 *  \param params             : Constraints for the choice of the batch cluster
 */
//=============================================================================
void Launcher_cpp::deleteSalomeJob( const long id, 
				    const machineParams& params) throw(LauncherException)
{
  // find a cluster matching params structure
  vector<string> aCompoList ;
  vector<string> aMachineList = _ResManager->GetFittingResources( params , aCompoList ) ;
  ParserResourcesType p = _ResManager->GetResourcesList(aMachineList[0]);
  string clustername(p.Alias);
    
  // search batch manager for that cluster in map
  map < string, Batch::BatchManager_eClient * >::const_iterator it = _batchmap.find(clustername);
  if(it == _batchmap.end())
    throw LauncherException("no batchmanager for that cluster");
  
  ostringstream oss;
  oss << id;
  Batch::JobId jobId( _batchmap[clustername], oss.str() );

  jobId.deleteJob();
}

//=============================================================================
/*! CORBA Method:
 *  Get result files of job on a cluster
 *  \param jobId              : identification of Salome job
 *  \param params             : Constraints for the choice of the batch cluster
 */
//=============================================================================
void Launcher_cpp::getResultSalomeJob( const string directory,
				       const long id, 
				       const machineParams& params) throw(LauncherException)
{
  vector<string> aCompoList ;
  vector<string> aMachineList = _ResManager->GetFittingResources( params , aCompoList ) ;
  ParserResourcesType p = _ResManager->GetResourcesList(aMachineList[0]);
  string clustername(p.Alias);
    
  // search batch manager for that cluster in map
  map < string, Batch::BatchManager_eClient * >::const_iterator it = _batchmap.find(clustername);
  if(it == _batchmap.end())
    throw LauncherException("no batchmanager for that cluster");
    
  Batch::Job* job = _jobmap[ pair<string,long>(clustername,id) ];

  _batchmap[clustername]->importOutputFiles( *job, directory );
}

//=============================================================================
/*!
 *  Factory to instanciate the good batch manager for choosen cluster.
 */ 
//=============================================================================

Batch::BatchManager_eClient *Launcher_cpp::FactoryBatchManager( const ParserResourcesType& params ) throw(LauncherException)
{

  std::string hostname, protocol, mpi;
  Batch::FactBatchManager_eClient* fact;

  hostname = params.Alias;
  switch(params.Protocol){
  case rsh:
    protocol = "rsh";
    break;
  case ssh:
    protocol = "ssh";
    break;
  default:
    throw LauncherException("unknown protocol");
    break;
  }
  switch(params.mpi){
  case lam:
    mpi = "lam";
    break;
  case mpich1:
    mpi = "mpich1";
    break;
  case mpich2:
    mpi = "mpich2";
    break;
  case openmpi:
    mpi = "openmpi";
    break;
  case slurm:
    mpi = "slurm";
    break;
  default:
    mpi = "indif";
    break;
  }    
  cerr << "Instanciation of batch manager" << endl;
  switch( params.Batch ){
  case pbs:
    cerr << "Instantiation of PBS batch manager" << endl;
    fact = new Batch::FactBatchManager_ePBS;
    break;
  case lsf:
    cerr << "Instantiation of LSF batch manager" << endl;
    fact = new Batch::FactBatchManager_eLSF;
    break;
  default:
    cerr << "BATCH = " << params.Batch << endl;
    throw LauncherException("no batchmanager for that cluster");
  }
  return (*fact)(hostname.c_str(),protocol.c_str(),mpi.c_str());
}

string Launcher_cpp::buildSalomeCouplingScript(const string fileToExecute, const string dirForTmpFiles, const ParserResourcesType& params)
{
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
  tempOutputFile << "  rm core.*" << endl;

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

  delete mpiImpl;

  return TmpFileName;
    
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

string Launcher_cpp::getTmpDirForBatchFiles()
{
  string ret;
  string thedate;

  // Adding date to the directory name
  Batch::Date date = Batch::Date(time(0));
  thedate = date.str();
  int lend = thedate.size() ;
  int i = 0 ;
  while ( i < lend ) {
    if ( thedate[i] == '/' || thedate[i] == '-' || thedate[i] == ':' ) {
      thedate[i] = '_' ;
    }
    i++ ;
  }

  ret = string("Batch/");
  ret += thedate;
  return ret;
}

string Launcher_cpp::getRemoteFile( std::string remoteDir, std::string localFile )
{
  string::size_type pos = localFile.find_last_of("/") + 1;
  int ln = localFile.length() - pos;
  string remoteFile = remoteDir + "/" + localFile.substr(pos,ln);
  return remoteFile;
}
