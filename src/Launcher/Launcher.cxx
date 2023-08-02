// Copyright (C) 2007-2023  CEA, EDF, OPEN CASCADE
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

#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <time.h>
#include <memory>

#ifdef WITH_LIBBATCH
#include <libbatch/BatchManagerCatalog.hxx>
#include <libbatch/FactBatchManager.hxx>
#include <libbatch/BatchManager.hxx>
#endif

#include "Basics_Utils.hxx"
#include "Basics_DirUtils.hxx"
#include "SALOME_Launcher_Handler.hxx"
#include "Launcher.hxx"
#include "Launcher_Job_Command.hxx"
#include "Launcher_Job_YACSFile.hxx"
#include "Launcher_Job_PythonSALOME.hxx"
#include "Launcher_Job_CommandSALOME.hxx"
#include "Launcher_XML_Persistence.hxx"

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
  LAUNCHER_MESSAGE("Launcher_cpp constructor");
  _job_cpt = 0;
}

//=============================================================================
/*!
 * destructor
 */
//=============================================================================
Launcher_cpp::~Launcher_cpp()
{
  LAUNCHER_MESSAGE("Launcher_cpp destructor");
#ifdef WITH_LIBBATCH
  std::map<int, Launcher::Job *>::const_iterator it_job;
  for(it_job = _launcher_job_map.begin(); it_job != _launcher_job_map.end(); it_job++)
    delete it_job->second;
  std::map <int, Batch::BatchManager * >::const_iterator it1;
  for(it1=_batchmap.begin();it1!=_batchmap.end();it1++)
    delete it1->second;
#endif
}

#ifdef WITH_LIBBATCH

//=============================================================================
/*!
 * Add a job into the launcher - check resource and choose one
 */
//=============================================================================
void
Launcher_cpp::createJob(Launcher::Job * new_job)
{
  LAUNCHER_MESSAGE("Creating a new job");
  // Add job to the jobs map
  new_job->setNumber(_job_cpt);
  _job_cpt++;
  std::map<int, Launcher::Job *>::const_iterator it_job = _launcher_job_map.find(new_job->getNumber());
  if (it_job == _launcher_job_map.end())
  {
    _launcher_job_map[new_job->getNumber()] = new_job;
  }
  else
  {
    LAUNCHER_INFOS("A job has already the same id: " << new_job->getNumber());
    throw LauncherException("A job has already the same id - job is not created !");
  }
  LAUNCHER_MESSAGE("New Job created");
}

//=============================================================================
/*!
 * Add a job into the launcher - check resource and choose one
 */
//=============================================================================
int
Launcher_cpp::createJob(const JobParameters_cpp& job_parameters)
{
  std::string job_type = job_parameters.job_type;
  Launcher::Job * new_job; // It is Launcher_cpp that is going to destroy it

  if (job_type == Launcher::Job_Command::TYPE_NAME)
    new_job = new Launcher::Job_Command();
  else if (job_type == Launcher::Job_CommandSALOME::TYPE_NAME)
    new_job = new Launcher::Job_CommandSALOME();
  else if (job_type == Launcher::Job_YACSFile::TYPE_NAME)
    new_job = new Launcher::Job_YACSFile();
  else if (job_type == Launcher::Job_PythonSALOME::TYPE_NAME)
    new_job = new Launcher::Job_PythonSALOME();
  else
  {
    std::string message("Launcher_cpp::createJob: bad job type: ");
    message += job_type;
    throw LauncherException(message.c_str());
  }

  // Name
  new_job->setJobName(job_parameters.job_name);

  // Directories
  new_job->setWorkDirectory(job_parameters.work_directory);
  new_job->setLocalDirectory(job_parameters.local_directory);
  new_job->setResultDirectory(job_parameters.result_directory);

  // Parameters for COORM
  new_job->setLauncherFile(job_parameters.launcher_file);
  new_job->setLauncherArgs(job_parameters.launcher_args);

  // Job File
  new_job->setJobFile(job_parameters.job_file);
  new_job->setPreCommand(job_parameters.pre_command);

  // Files
  new_job->setEnvFile(job_parameters.env_file);
  for(const std::string param : job_parameters.in_files)
    new_job->add_in_file(param);
  for(const std::string param : job_parameters.out_files)
    new_job->add_out_file(param);

  new_job->setMaximumDuration(job_parameters.maximum_duration);
  new_job->setQueue(job_parameters.queue);
  new_job->setPartition(job_parameters.partition);
  new_job->setExclusive(job_parameters.exclusive);
  new_job->setMemPerCpu(job_parameters.mem_per_cpu);
  new_job->setWCKey(job_parameters.wckey);
  new_job->setExtraParams(job_parameters.extra_params);

  // Resources requirements
  new_job->setResourceRequiredParams(job_parameters.resource_required);

  // Adding specific parameters to the job
  for (const auto& it_specific : job_parameters.specific_parameters)
    new_job->addSpecificParameter(it_specific.first, it_specific.second);

  new_job->checkSpecificParameters();

  createJob(new_job);

  return new_job->getNumber();
}

//=============================================================================
/*!
 * Launch a job
 */
//=============================================================================
void
Launcher_cpp::launchJob(int job_id)
{
  LAUNCHER_MESSAGE("Launch a job");

  // Check if job exists
  Launcher::Job * job = findJob(job_id);

  // Check job state (cannot launch a job already launched...)
  if (job->getState() != "CREATED")
  {
    LAUNCHER_INFOS("Bad state of the job: " << job->getState());
    throw LauncherException("Bad state of the job: " + job->getState());
  }

  Batch::BatchManager * bm = getBatchManager(job);

  try {
    Batch::JobId batch_manager_job_id = bm->submitJob(*(job->getBatchJob()));
    job->setBatchManagerJobId(batch_manager_job_id);
    job->setState("QUEUED");
    job->setReference(batch_manager_job_id.getReference());
  }
  catch(const Batch::GenericException &ex)
  {
    LAUNCHER_INFOS("Job is not launched, exception in submitJob: " << ex.message);
    throw LauncherException(ex.message.c_str());
  }
  LAUNCHER_MESSAGE("Job launched");
}

//=============================================================================
/*!
 * Get job state
 */
//=============================================================================
std::string
Launcher_cpp::getJobState(int job_id)
{
  LAUNCHER_MESSAGE("Get job state");

  // Check if job exist
  Launcher::Job * job = findJob(job_id);

  std::string state;
  try
  {
    state = job->updateJobState();
  }
  catch(const Batch::GenericException &ex)
  {
    LAUNCHER_INFOS("getJobState failed, exception: " << ex.message);
    throw LauncherException(ex.message.c_str());
  }

  return state;
}

//=============================================================================
/*!
 * Get job assigned hostnames
 */
//=============================================================================
std::string
Launcher_cpp::getAssignedHostnames(int job_id)
{
  LAUNCHER_MESSAGE("Get job assigned hostnames");

  // Check if job exist
  Launcher::Job * job = findJob(job_id);
  std::string assigned_hostnames = job->getAssignedHostnames();

  return assigned_hostnames;
}

void
Launcher_cpp::exportInputFiles(int job_id)
{
  LAUNCHER_MESSAGE("Copy in files");
  // Check if job exists
  Launcher::Job * job = findJob(job_id);

  // Check job state (cannot launch a job already launched...)
  if (job->getState() != "CREATED")
  {
    LAUNCHER_INFOS("Bad state of the job: " << job->getState());
    throw LauncherException("Bad state of the job: " + job->getState());
  }

  Batch::BatchManager * bm = getBatchManager(job);

  try {
    bm->exportInputFiles(*(job->getBatchJob()));
  }
  catch(const Batch::GenericException &ex)
  {
    LAUNCHER_INFOS("Failed to copy in files: " << ex.message);
    throw LauncherException(ex.message.c_str());
  }
  LAUNCHER_MESSAGE("Files copied");
}

//=============================================================================
/*!
 * Get Job result - the result directory could be changed
 */
//=============================================================================
void
Launcher_cpp::getJobResults(int job_id, std::string directory)
{
  LAUNCHER_MESSAGE("Get Job results");

  Launcher::Job * job = findJob(job_id);
  std::string resource_name = job->getResourceDefinition().name;
  try
  {
    if (directory != "")
      _batchmap[job_id]->importOutputFiles(*(job->getBatchJob()), directory);
    else
      _batchmap[job_id]->importOutputFiles(*(job->getBatchJob()), job->getResultDirectory());
  }
  catch(const Batch::GenericException &ex)
  {
    LAUNCHER_INFOS("getJobResult is maybe incomplete, exception: " << ex.message);
    throw LauncherException(ex.message.c_str());
  }
  LAUNCHER_MESSAGE("getJobResult ended");
}

//=============================================================================
/*!
 * Clear the remote working directory
 */
//=============================================================================
void
Launcher_cpp::clearJobWorkingDir(int job_id)
{
  LAUNCHER_MESSAGE("Clear the remote working directory");

  Launcher::Job * job = findJob(job_id);
  try
  {
    _batchmap[job_id]->clearWorkingDir(*(job->getBatchJob()));
  }
  catch(const Batch::GenericException &ex)
  {
    LAUNCHER_INFOS("getJobResult is maybe incomplete, exception: " << ex.message);
    throw LauncherException(ex.message.c_str());
  }
  LAUNCHER_MESSAGE("getJobResult ended");
}

//=============================================================================
/*!
 * Get Job dump state - the result directory could be changed
 */
//=============================================================================
bool
Launcher_cpp::getJobDumpState(int job_id, std::string directory)
{
  bool rtn;
  LAUNCHER_MESSAGE("Get Job dump state");

  Launcher::Job * job = findJob(job_id);
  std::string resource_name = job->getResourceDefinition().name;
  try
  {
    if (directory != "")
      rtn = _batchmap[job_id]->importDumpStateFile(*(job->getBatchJob()), directory);
    else
      rtn = _batchmap[job_id]->importDumpStateFile(*(job->getBatchJob()), job->getResultDirectory());
  }
  catch(const Batch::GenericException &ex)
  {
    LAUNCHER_INFOS("getJobResult is maybe incomplete, exception: " << ex.message);
    throw LauncherException(ex.message.c_str());
  }
  LAUNCHER_MESSAGE("getJobResult ended");
  return rtn;
}

//=============================================================================
/*!
 * Get one file from the working directory - the result directory can be changed
 */
//=============================================================================
bool
Launcher_cpp::getJobWorkFile(int job_id,
                             std::string work_file,
                             std::string directory)
{
  bool rtn;
  LAUNCHER_MESSAGE("Get working file " << work_file);

  Launcher::Job * job = findJob(job_id);
  std::string resource_name = job->getResourceDefinition().name;
  try
  {
    if (directory != "")
      rtn = _batchmap[job_id]->importWorkFile(*(job->getBatchJob()), work_file, directory);
    else
      rtn = _batchmap[job_id]->importWorkFile(*(job->getBatchJob()), work_file, job->getResultDirectory());
  }
  catch(const Batch::GenericException &ex)
  {
    LAUNCHER_INFOS("getJobWorkFile is maybe incomplete, exception: " << ex.message);
    throw LauncherException(ex.message.c_str());
  }
  LAUNCHER_MESSAGE("getJobWorkFile ended");
  return rtn;
}

//=============================================================================
/*!
 * Remove the job - into the Launcher and its batch manager
 */
//=============================================================================
void
Launcher_cpp::removeJob(int job_id)
{
  LAUNCHER_MESSAGE("Remove Job");

  // Check if job exist
  std::map<int, Launcher::Job *>::iterator it_job = _launcher_job_map.find(job_id);
  if (it_job == _launcher_job_map.end())
  {
    LAUNCHER_INFOS("Cannot find the job, is it created ? job number: " << job_id);
    throw LauncherException("Cannot find the job, is it created ?");
  }

  it_job->second->removeJob();
  delete it_job->second;
  _launcher_job_map.erase(it_job);
}

//=============================================================================
/*!
 * stop the job
 */
//=============================================================================
void
Launcher_cpp::stopJob(int job_id)
{
  LAUNCHER_MESSAGE("Stop Job");

  Launcher::Job * job = findJob(job_id);
  job->stopJob();
}

std::string
Launcher_cpp::dumpJob(int job_id)
{
  LAUNCHER_MESSAGE("dump Job");

  Launcher::Job * job = findJob(job_id);
  return Launcher::XML_Persistence::dumpJob(*job);
}

int
Launcher_cpp::restoreJob(const std::string& dumpedJob)
{
  LAUNCHER_MESSAGE("restore Job");
  Launcher::Job* new_job(nullptr);
  int jobId = -1;
  try
  {
    {
      new_job = Launcher::XML_Persistence::createJobFromString(dumpedJob);
    }
    if(new_job)
    {
      jobId = addJob(new_job);
    }
    else
      LAUNCHER_INFOS("Failed to create a new job.");
  }
  catch(const LauncherException &ex)
  {
    LAUNCHER_INFOS("Cannot load the job. Exception: " << ex.msg.c_str());
  }
  return jobId;
}

JobParameters_cpp
Launcher_cpp::getJobParameters(int job_id)
{
  Launcher::Job * job = findJob(job_id);
  JobParameters_cpp job_parameters;
  job_parameters.job_name         = job->getJobName();
  job_parameters.job_type         = job->getJobType();
  job_parameters.job_file         = job->getJobFile();
  job_parameters.env_file         = job->getEnvFile();
  job_parameters.work_directory   = job->getWorkDirectory();
  job_parameters.local_directory  = job->getLocalDirectory();
  job_parameters.result_directory = job->getResultDirectory();
  job_parameters.pre_command      = job->getPreCommand();

  // Parameters for COORM
  job_parameters.launcher_file = job->getLauncherFile();
  job_parameters.launcher_args = job->getLauncherArgs();

  job_parameters.in_files      = job->get_in_files();
  job_parameters.out_files     = job->get_out_files();

  job_parameters.maximum_duration = job->getMaximumDuration();
  job_parameters.queue            = job->getQueue();
  job_parameters.partition        = job->getPartition();
  job_parameters.exclusive        = job->getExclusive();
  job_parameters.mem_per_cpu      = job->getMemPerCpu();
  job_parameters.wckey            = job->getWCKey();
  job_parameters.extra_params     = job->getExtraParams();

  resourceParamsContainer resource_params = job->getResourceRequiredParams();
  job_parameters.resource_required.name             = resource_params.name;
  job_parameters.resource_required.hostname         = resource_params.hostname;
  job_parameters.resource_required.OS               = resource_params.OS;
  job_parameters.resource_required.nb_proc          = resource_params.nb_proc;
  job_parameters.resource_required.nb_node          = resource_params.nb_node;
  job_parameters.resource_required.nb_proc_per_node = resource_params.nb_proc_per_node;
  job_parameters.resource_required.cpu_clock        = resource_params.cpu_clock;
  job_parameters.resource_required.mem_mb           = resource_params.mem_mb;

  job_parameters.specific_parameters                = job->getSpecificParameters();

  return job_parameters;
}

//=============================================================================
/*!
 *  create a launcher job based on a file
 *  \param xmlExecuteFile     : to define the execution on the batch cluster
 */
//=============================================================================
long
Launcher_cpp::createJobWithFile(const std::string xmlExecuteFile,
                                const std::string clusterName)
{
  LAUNCHER_MESSAGE("Begin of Launcher_cpp::createJobWithFile");

  // Parsing xml file
  ParserLauncherType job_params = ParseXmlFile(xmlExecuteFile);

  // Creating a new job
  std::unique_ptr<Launcher::Job_Command> new_job(new Launcher::Job_Command);

  std::string cmdFile = Kernel_Utils::GetTmpFileName();
#ifndef WIN32
  cmdFile += ".sh";
#else
  cmdFile += ".bat";
#endif
  std::ofstream os;
  os.open(cmdFile.c_str(), std::ofstream::out );
  os << "#! /bin/sh" << std::endl;
  os << job_params.Command;
  os.close();

  new_job->setJobFile(cmdFile);
  new_job->setLocalDirectory(job_params.RefDirectory);
  new_job->setWorkDirectory(job_params.MachinesList[clusterName].WorkDirectory);
  new_job->setEnvFile(job_params.MachinesList[clusterName].EnvFile);

  for(size_t i=0; i < job_params.InputFile.size(); i++)
    new_job->add_in_file(job_params.InputFile[i]);
  for(size_t i=0; i < job_params.OutputFile.size();i++)
    new_job->add_out_file(job_params.OutputFile[i]);

  resourceParamsContainer p;
  p.hostname = clusterName;
  p.name = "";
  p.OS = "";
  p.nb_proc = job_params.NbOfProcesses;
  p.nb_node = 0;
  p.nb_proc_per_node = 0;
  p.cpu_clock = 0;
  p.mem_mb = 0;
  new_job->setResourceRequiredParams(p);

  createJob(new_job.get());
  return new_job->getNumber();
}

//=============================================================================
/*!
 *  Factory to instantiate the good batch manager for chosen cluster.
 */
//=============================================================================
Batch::BatchManager *
Launcher_cpp::FactoryBatchManager(ParserResourcesTypeJob& params)
{
  std::string mpi;
  Batch::CommunicationProtocolType protocol;
  Batch::FactBatchManager * fact;

  std::string hostname = params.hostname;

  switch(params.protocol)
  {
    case sh:
      protocol = Batch::SH;
      break;
    case rsh:
      protocol = Batch::RSH;
      break;
    case ssh:
      protocol = Batch::SSH;
      break;
    case rsync:
      protocol = Batch::RSYNC;
      break;
    default:
      throw LauncherException("Unknown protocol for this resource");
      break;
  }

  switch(params.mpiImpl)
  {
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
    case ompi:
      mpi = "ompi";
      break;
    case slurmmpi:
      mpi = "slurmmpi";
      break;
    case prun:
      mpi = "prun";
      break;
    default:
      mpi = "nompi";
  }

  const char * bmType;
  switch( params.batch )
  {
    case pbs:
      bmType = "PBS";
      break;
    case lsf:
      bmType = "LSF";
      break;
    case sge:
      bmType = "SGE";
      break;
    case ccc:
      bmType = "CCC";
      break;
    case slurm:
      bmType = "SLURM";
      break;
    case none:
      bmType = "LOCAL";
      break;
    case ll:
      bmType = "LL";
      break;
    case vishnu:
      bmType = "VISHNU";
      break;
    case oar:
      bmType = "OAR";
      break;
    case coorm:
      bmType = "COORM";
      break;
    default:
      LAUNCHER_MESSAGE("Bad batch description of the resource: Batch = " << params.batch);
      throw LauncherException("No batchmanager for that cluster - Bad batch description of the resource");
  }
  Batch::BatchManagerCatalog & cata = Batch::BatchManagerCatalog::getInstance();
  fact = dynamic_cast<Batch::FactBatchManager*>(cata(bmType));
  if (fact == NULL) {
    LAUNCHER_MESSAGE("Cannot find batch manager factory for " << bmType << ". Check your version of libBatch.");
    throw LauncherException("Cannot find batch manager factory");
  }
  LAUNCHER_MESSAGE("Instantiation of batch manager of type: " << bmType);
  Batch::BatchManager * batch_client = (*fact)(hostname.c_str(), params.username.c_str(),
                                               protocol, mpi.c_str());
  return batch_client;
}

//----------------------------------------------------------
// Without LIBBATCH - Launcher_cpp do nothing...
//----------------------------------------------------------
#else

void
Launcher_cpp::createJob(Launcher::Job * new_job)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot create a job !!!");
  throw LauncherException("Method Launcher_cpp::createJob is not available "
                          "(libBatch was not present at compilation time)");
}

int
Launcher_cpp::createJob(const JobParameters_cpp& job_parameters)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot create a job !!!");
  throw LauncherException("Method Launcher_cpp::createJob is not available "
                          "(libBatch was not present at compilation time)");
}

void
Launcher_cpp::launchJob(int job_id)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot launch a job !!!");
  throw LauncherException("Method Launcher_cpp::launchJob is not available "
                          "(libBatch was not present at compilation time)");
}

std::string
Launcher_cpp::getJobState(int job_id)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot get job state!!!");
  throw LauncherException("Method Launcher_cpp::getJobState is not available "
                          "(libBatch was not present at compilation time)");
}

std::string
Launcher_cpp::getAssignedHostnames(int job_id)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot get job assigned hostnames!!!");
  throw LauncherException("Method Launcher_cpp::getAssignedHostnames is not available "
                          "(libBatch was not present at compilation time)");
}

void
Launcher_cpp::exportInputFiles(int job_id)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot copy the files!!!");
  throw LauncherException("Method Launcher_cpp::exportInputFiles is not available "
                          "(libBatch was not present at compilation time)");
}

void
Launcher_cpp::getJobResults(int job_id, std::string directory)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot get job results!!!");
  throw LauncherException("Method Launcher_cpp::getJobResults is not available "
                          "(libBatch was not present at compilation time)");
}

void
Launcher_cpp::clearJobWorkingDir(int job_id)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot clear directory!!!");
  throw LauncherException("Method Launcher_cpp::clearJobWorkingDir is not available "
                          "(libBatch was not present at compilation time)");
}

bool
Launcher_cpp::getJobDumpState(int job_id, std::string directory)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot get job dump state!!!");
  throw LauncherException("Method Launcher_cpp::getJobDumpState is not available "
                          "(libBatch was not present at compilation time)");
}

bool
Launcher_cpp::getJobWorkFile(int job_id, std::string work_file, std::string directory)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot get job dump state!!!");
  throw LauncherException("Method Launcher_cpp::getJobWorkFile is not available "
                          "(libBatch was not present at compilation time)");
}

void
Launcher_cpp::removeJob(int job_id)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot remove job!!!");
  throw LauncherException("Method Launcher_cpp::removeJob is not available "
                          "(libBatch was not present at compilation time)");
}

void
Launcher_cpp::stopJob(int job_id)
{
  throw LauncherException("Method Launcher_cpp::stopJob is not available "
                          "(libBatch was not present at compilation time)");
}

std::string
Launcher_cpp::dumpJob(int job_id)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot dump job!!!");
  throw LauncherException("Method Launcher_cpp::dumpJob is not available "
                          "(libBatch was not present at compilation time)");
  return "";
}

int
Launcher_cpp::restoreJob(const std::string& dumpedJob)
{
  LAUNCHER_INFOS("Launcher compiled without LIBBATCH - cannot restore job!!!");
  throw LauncherException("Method Launcher_cpp::restoreJob is not available "
                          "(libBatch was not present at compilation time)");
  return 0;
}

long
Launcher_cpp::createJobWithFile( const std::string xmlExecuteFile, std::string clusterName)
{
  throw LauncherException("Method Launcher_cpp::createJobWithFile is not available "
                          "(libBatch was not present at compilation time)");
  return 0;
}

JobParameters_cpp
Launcher_cpp::getJobParameters(int job_id)
{
  throw LauncherException("Method Launcher_cpp::getJobParamaters is not available "
                          "(libBatch was not present at compilation time)");
}

#endif

ParserLauncherType
Launcher_cpp::ParseXmlFile(std::string xmlExecuteFile)
{
  ParserLauncherType job_params;
  SALOME_Launcher_Handler * handler = new SALOME_Launcher_Handler(job_params);

  const char* aFilePath = xmlExecuteFile.c_str();
  FILE* aFile = fopen(aFilePath, "r");
  if (aFile != NULL)
  {
    xmlDocPtr aDoc = xmlReadFile(aFilePath, NULL, 0);
    if (aDoc != NULL)
      handler->ProcessXmlDocument(aDoc);
    else
    {
      std::string message = "ResourcesManager_cpp: could not parse file: " + xmlExecuteFile;
      LAUNCHER_MESSAGE(message);
      delete handler;
      throw LauncherException(message);
    }
    // Free the document
    xmlFreeDoc(aDoc);
    fclose(aFile);
  }
  else
  {
    std::string message = "ResourcesManager_cpp: file is not readable: " + xmlExecuteFile;
    LAUNCHER_MESSAGE(message);
    delete handler;
    throw LauncherException(message);
  }

  // Return
  delete handler;
  return job_params;
}

std::map<int, Launcher::Job *>
Launcher_cpp::getJobs()
{
  return _launcher_job_map;
}

#ifdef WITH_LIBBATCH
Batch::BatchManager*
Launcher_cpp::getBatchManager(Launcher::Job * job)
{
  if(!_ResManager)
    throw LauncherException("Resource manager is not set.");

  Batch::BatchManager* result = nullptr;
  int job_id = job->getNumber();

  // Select a resource for the job
  std::vector<std::string> ResourceList;
  resourceParamsContainer params = job->getResourceRequiredParams();
  // Consider only resources that can launch batch jobs

  try
  {
    ResourceList = _ResManager->GetFittingResourcesContainer(params);
  }
  catch(const ResourcesException &ex)
  {
    throw LauncherException(ex.msg.c_str());
  }
  if (ResourceList.size() == 0)
  {
    LAUNCHER_INFOS("No adequate resource found for the job, number " << job->getNumber());
    job->setState("ERROR");
    throw LauncherException("No resource found the job");
  }

  // Configure the job with the resource selected - the first of the list
  ParserResourcesTypeJob resource_definition = _ResManager->GetResourceDefinitionJob(ResourceList[0]);

  // Set resource definition to the job
  // The job will check if the definitions needed
  try
  {
    job->setResourceDefinition(resource_definition);
  }
  catch(const LauncherException &ex)
  {
    LAUNCHER_INFOS("Error in the definition of the resource, mess: " << ex.msg);
    job->setState("ERROR");
    throw ex;
  }

  // Step 2: We can now add a Factory if the resource is correctly define
  std::map<int, Batch::BatchManager *>::const_iterator it = _batchmap.find(job_id);
  if(it == _batchmap.end())
  {
    try
    {
      // Warning cannot write on one line like this, because map object is constructed before
      // the method is called...
      //_batchmap[job_id] = FactoryBatchManager(resource_definition);
      result = FactoryBatchManager(resource_definition);
      _batchmap[job_id] = result;
    }
    catch(const LauncherException &ex)
    {
      LAUNCHER_INFOS("Error during creation of the batch manager of the job, mess: " << ex.msg);
      throw ex;
    }
    catch(const Batch::GenericException &ex)
    {
      LAUNCHER_INFOS("Error during creation of the batch manager of the job, mess: " << ex.message);
      throw LauncherException(ex.message);
    }
  }
  else
    result = it->second;
  return result;
}
#endif

void
Launcher_cpp::addJobDirectlyToMap(Launcher::Job * new_job)
{
  // Step 0: Calculated job_id
  new_job->setNumber(_job_cpt);
  _job_cpt++;

#ifdef WITH_LIBBATCH
  // Step 1: check if resource is already in the map
  Batch::BatchManager * bm = getBatchManager(new_job);

  // Step 2: add the job to the batch manager
  try
  {
    Batch::JobId batch_manager_job_id = bm->addJob(*(new_job->getBatchJob()),
                                                   new_job->getReference());
    new_job->setBatchManagerJobId(batch_manager_job_id);
  }
  catch(const Batch::GenericException &ex)
  {
    LAUNCHER_INFOS("Job cannot be added, exception in addJob: " << ex.message);
    throw LauncherException(ex.message.c_str());
  }

  // Step 3: add job to launcher map
  std::map<int, Launcher::Job *>::const_iterator it_job = _launcher_job_map.find(new_job->getNumber());
  if (it_job == _launcher_job_map.end())
  {
    _launcher_job_map[new_job->getNumber()] = new_job;
  }
  else
  {
    LAUNCHER_INFOS("A job as already the same id: " << new_job->getNumber());
    throw LauncherException("A job as already the same id - job is not created !");
  }
  LAUNCHER_MESSAGE("New job added");
#endif
}

int
Launcher_cpp::addJob(Launcher::Job * new_job)
{
  string job_state = new_job->getState();
  int jobId = -1;
  if (job_state == "CREATED")
  {
    // In this case, we ignore run_part information
    createJob(new_job);
    jobId = new_job->getNumber();
  }
  else if (job_state == "QUEUED"     ||
            job_state == "RUNNING"    ||
            job_state == "IN_PROCESS" ||
            job_state == "PAUSED")
  {
    addJobDirectlyToMap(new_job);
    jobId = new_job->getNumber();

    // We check that the BatchManager could resume the job
#ifdef WITH_LIBBATCH
    if (new_job->getBatchManagerJobId().getReference() != new_job->getReference())
    {
      LAUNCHER_INFOS("BatchManager type cannot resume a job - job state is set to ERROR");
      new_job->setState("ERROR");
    }
#endif
  }
  else if (job_state == "FINISHED" ||
            job_state == "FAILED"   ||
            job_state == "ERROR")
  {
    // We add run_part information
    addJobDirectlyToMap(new_job);
    jobId = new_job->getNumber();
  }
  else
  {
    LAUNCHER_INFOS("A bad job is found, state unknown " << job_state);
    jobId = -1;
  }
  return jobId;
}

list<int>
Launcher_cpp::loadJobs(const char* jobs_file)
{
  list<int> new_jobs_id_list;

  // Load the jobs from XML file
  list<Launcher::Job *> jobs_list = Launcher::XML_Persistence::loadJobs(jobs_file);

  // Create each job in the launcher
  list<Launcher::Job *>::const_iterator it_job;
  for (it_job = jobs_list.begin(); it_job != jobs_list.end(); it_job++)
  {
    Launcher::Job* new_job(*it_job);
    int jobId = -1;
    try
    {
      jobId = addJob(new_job);
      if(jobId >= 0)
        new_jobs_id_list.push_back(jobId);
    }
    catch(const LauncherException &ex)
    {
      LAUNCHER_INFOS("Cannot load the job. Exception: " << ex.msg.c_str());
    }
  }

  return new_jobs_id_list;
}

void
Launcher_cpp::saveJobs(const char* jobs_file)
{
  // Create a sorted list from the internal job map
  list<const Launcher::Job *> jobs_list;
  {
    for (int i=0; i<_job_cpt; i++)
    {
      map<int, Launcher::Job *>::const_iterator it_job = _launcher_job_map.find(i);
      if (it_job != _launcher_job_map.end())
        jobs_list.push_back(it_job->second);
    }
  }
  // Save the jobs in XML file
  Launcher::XML_Persistence::saveJobs(jobs_file, jobs_list);
}

Launcher::Job *
Launcher_cpp::findJob(int job_id)
{
  std::map<int, Launcher::Job *>::const_iterator it_job = _launcher_job_map.find(job_id);
  if (it_job == _launcher_job_map.end())
  {
    LAUNCHER_INFOS("Cannot find the job, is it created ? job number: " << job_id);
    throw LauncherException("Cannot find the job, is it created ?");
  }
  Launcher::Job * job = it_job->second;
  return job;
}
