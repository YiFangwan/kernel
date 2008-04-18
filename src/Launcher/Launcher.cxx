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
  default:
    p.mpiImpl = "indif";
    break;
  }    

  cerr << "Instanciation of batch manager" << endl;
  switch( params.Batch ){
  case pbs:
    cerr << "Instantiation of PBS batch manager" << endl;
    return new BatchLight::BatchManager_PBS(p);
  case slurm:
    cerr << "Instantiation of SLURM batch manager" << endl;
    return new BatchLight::BatchManager_SLURM(p);
  default:
    cerr << "BATCH = " << params.Batch << endl;
    throw LauncherException("no batchmanager for that cluster");
  }
}

