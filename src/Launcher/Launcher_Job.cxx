//  Copyright (C) 2009 CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Author: André RIBES - EDF R&D

#include "Launcher_Job.hxx"
#include "Launcher.hxx"

Launcher::Job::Job()
{
  _number = -1;
  _state = "CREATED";
  _launch_date = getLaunchDate();

  _env_file = "";
  _work_directory = "";
  _local_directory = "";
  _result_directory = "";
  _maximum_duration = "";
  _maximum_duration_in_second = -1;
  _machine_required_params.hostname = "";
  _machine_required_params.OS = "";
  _machine_required_params.nb_node = -1;
  _machine_required_params.nb_proc_per_node = -1;
  _machine_required_params.cpu_clock = -1;
  _machine_required_params.mem_mb = -1;
  _machine_required_params.parallelLib = "";
  _machine_required_params.nb_component_nodes = -1;
  _queue = "";

#ifdef WITH_LIBBATCH
  _batch_job = new Batch::Job();
#endif
}

Launcher::Job::~Job() 
{
  LAUNCHER_MESSAGE("Deleting job number: " << _number);
#ifdef WITH_LIBBATCH
  if (_batch_job_id.getReference() != "undefined")
    _batch_job_id.deleteJob();
  if (_batch_job)
    delete _batch_job;
#endif
}

void 
Launcher::Job::setState(const std::string & state)
{
  // State of a Job: CREATED, QUEUED, RUNNING, FINISHED, ERROR
  if (state != "CREATED" and
      state != "QUEUED" and
      state != "RUNNING" and
      state != "FINISHED" and
      state != "ERROR")
  {
    throw LauncherException("Bad state, this state does not exist: " + state);
  }
  _state = state;
}

std::string 
Launcher::Job::getState()
{
  return _state;
}

void 
Launcher::Job::setNumber(const int & number)
{
  if (_number != -1)
    std::cerr << "Launcher::Job::setNumber -- Job number was already defined, before: " << _number << " now: " << number << std::endl;
  _number = number;
}

int
Launcher::Job::getNumber()
{
  return _number;
}

void 
Launcher::Job::setMachineDefinition(const ParserResourcesType & machine_definition)
{
  // Check machine_definition
  if (machine_definition.UserName == "")
  {
    std::string mess = "Machine definition must define a user name !, machine name is: " + machine_definition.HostName;
    throw LauncherException(mess);
  }

  _machine_definition = machine_definition;
}

ParserResourcesType 
Launcher::Job::getMachineDefinition()
{
  return _machine_definition;
}

void 
Launcher::Job::setEnvFile(std::string & env_file)
{
  _env_file = env_file;
  if (_env_file != "")
    add_in_file(_env_file);
}

std::string
Launcher::Job::getEnvFile()
{
  return _env_file;
}

void 
Launcher::Job::setWorkDirectory(const std::string & work_directory)
{
  _work_directory = work_directory;
}

void 
Launcher::Job::setLocalDirectory(const std::string & local_directory)
{
  _local_directory = local_directory;
}

void 
Launcher::Job::setResultDirectory(const std::string & result_directory)
{
  _result_directory = result_directory;
}

void 
Launcher::Job::add_in_file(const std::string & file)
{
  std::list<std::string>::iterator it = std::find(_in_files.begin(), _in_files.end(), file);
  if (it == _in_files.end())
    _in_files.push_back(file);
  else
    std::cerr << "Launcher::Job::add_in_file -- Warning file was already entered in in_files: " << file << std::endl;
}

void 
Launcher::Job::add_out_file(const std::string & file)
{
  std::list<std::string>::iterator it = std::find(_out_files.begin(), _out_files.end(), file);
  if (it == _out_files.end())
    _out_files.push_back(file);
  else
    std::cerr << "Launcher::Job::add_out_file -- Warning file was already entered in out_files: " << file << std::endl;
}

void 
Launcher::Job::setMaximumDuration(const std::string & maximum_duration)
{
  checkMaximumDuration(maximum_duration);
  _maximum_duration_in_second = convertMaximumDuration(maximum_duration);
  _maximum_duration = maximum_duration;
}

void 
Launcher::Job::setMachineRequiredParams(const machineParams & machine_required_params)
{
  checkMachineRequiredParams(machine_required_params);
  _machine_required_params = machine_required_params;
}

void 
Launcher::Job::setQueue(const std::string & queue)
{
  _queue = queue;
}

std::string 
Launcher::Job::getWorkDirectory()
{
  return _work_directory;
}

std::string 
Launcher::Job::getLocalDirectory()
{
  return _local_directory;
}

std::string
Launcher::Job::getResultDirectory()
{
  return _result_directory;
}

const std::list<std::string> & 
Launcher::Job::get_in_files()
{
  return _in_files;
}

const std::list<std::string> & 
Launcher::Job::get_out_files()
{
  return _out_files;
}

std::string 
Launcher::Job::getMaximumDuration()
{
  return _maximum_duration;
}

machineParams 
Launcher::Job::getMachineRequiredParams()
{
  return _machine_required_params;
}

std::string 
Launcher::Job::getQueue()
{
  return _queue;
}

void 
Launcher::Job::checkMaximumDuration(const std::string & maximum_duration)
{
  std::string result("");
  std::string edt_value = maximum_duration;
  if (edt_value != "") {
    std::string begin_edt_value = edt_value.substr(0, 2);
    std::string mid_edt_value = edt_value.substr(2, 1);
    std::string end_edt_value = edt_value.substr(3);
  
    long value;
    std::istringstream iss(begin_edt_value);
    if (!(iss >> value)) {
      result = "[Launcher::Job::checkExpectedDuration] Error on definition ! : " + edt_value;
    }
    else if (value < 0) {
      result = "[Launcher::Job::checkExpectedDuration] Error on definition time is negative ! : " + value;
    }
    std::istringstream iss_2(end_edt_value);
    if (!(iss_2 >> value)) {
      result = "[Launcher::Job::checkExpectedDuration] Error on definition ! : " + edt_value;
    }
    else if (value < 0) {
      result = "[Launcher::Job::checkExpectedDuration] Error on definition time is negative ! : " + value;
    }
    if (mid_edt_value != ":") {
      result = "[Launcher::Job::checkExpectedDuration] Error on definition ! :" + edt_value;
    }
  }
  if (result != "")
    throw LauncherException(result);
}

void 
Launcher::Job::checkMachineRequiredParams(const machineParams & machine_required_params)
{
  // nb_node has be to > 0
  if (machine_required_params.nb_node <= 0)
  {
    std::string message("[Launcher::Job::checkMachineRequiredParams] node number is not >0 ! ");
    throw LauncherException(message);
  }
}

long 
Launcher::Job::convertMaximumDuration(const std::string & edt)
{
  long hh, mm, ret;

  if( edt.size() == 0 )
    return -1;

  std::string::size_type pos = edt.find(":");
  std::string h = edt.substr(0,pos);
  std::string m = edt.substr(pos+1,edt.size()-pos+1);
  std::istringstream issh(h);
  issh >> hh;
  std::istringstream issm(m);
  issm >> mm;
  ret = hh*60 + mm;
  ret = ret * 60;

  return ret;
}

std::string 
Launcher::Job::getLaunchDate()
{
  time_t rawtime;
  time(&rawtime);
  std::string launch_date = ctime(&rawtime);
  int i = 0 ;
  for (;i < launch_date.size(); i++) 
    if (launch_date[i] == '/' or 
	launch_date[i] == '-' or 
	launch_date[i] == ':' or
	launch_date[i] == ' ') 
      launch_date[i] = '_';
  launch_date.erase(--launch_date.end()); // Last caracter is a \n

  return launch_date;
}

std::string
Launcher::Job::updateJobState()
{
#ifdef WITH_LIBBATCH
  if (_batch_job_id.getReference() != "undefined")
  {
    // A batch manager has been affected to the job
    Batch::JobInfo job_info = _batch_job_id.queryJob();
    Batch::Parametre par = job_info.getParametre();

    LAUNCHER_MESSAGE("State received is: " << par[STATE].str());

    // Patch until new LIBBATCH version
    // eSSH Client and ePBS Client
    if (par[STATE].str() == "Running" or par[STATE].str() == "E" or par[STATE].str() == "R")
      _state = "RUNNING";
    else if (par[STATE].str() == "Stopped")
      _state = "PAUSED";
    else if (par[STATE].str() == "Done" or par[STATE].str() == "U")
      _state = "FINISHED";
    else if (par[STATE].str() == "Dead")
      _state = "ERROR";
    else if (par[STATE].str() == "Q")
      _state = "QUEUED";
  }
#endif
  return _state;
}

#ifdef WITH_LIBBATCH
Batch::Job * 
Launcher::Job::getBatchJob()
{
  update_job();
  return _batch_job;
}

Batch::Parametre
Launcher::Job::common_job_params()
{
  Batch::Parametre params;

  params[USER] = _machine_definition.UserName;
  params[NBPROC] = _machine_required_params.nb_node;

  // Memory
  if (_machine_required_params.mem_mb > 0)
  {
    // Memory is in kilobytes
    params[MAXRAMSIZE] = _machine_required_params.mem_mb * 1024;
  }

  // We define a default directory based on user time
  if (_work_directory == "")
  {
    std::string thedate;
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
    _work_directory = std::string("Batch/");
    _work_directory += thedate;
  }
  params[WORKDIR] = _work_directory;
  params[TMPDIR] = _work_directory; // To Compatibility -- remove ??? TODO

  // If result_directory is not defined, we use HOME environnement
  if (_result_directory == "")
    _result_directory = getenv("HOME");

  // _in_files
  for(std::list<std::string>::iterator it = _in_files.begin(); it != _in_files.end(); it++)
  {
    std::string file = *it;

    // local file -> If file is not an absolute path, we apply _local_directory
    std::string local_file;
    if (file.substr(0, 1) == std::string("/"))
      local_file = file;
    else
      local_file = _local_directory + "/" + file;
    
    // remote file -> get only file name from _in_files
    size_t found = file.find_last_of("/");
    std::string remote_file = _work_directory + "/" + file.substr(found+1);

    params[INFILE] += Batch::Couple(local_file, remote_file);
  }
   
  // _out_files
  for(std::list<std::string>::iterator it = _out_files.begin(); it != _out_files.end(); it++)
  {
    std::string file = *it;

    // local file 
    size_t found = file.find_last_of("/");
    std::string local_file = _result_directory +  "/" + file.substr(found+1);

    // remote file -> If file is not an absolute path, we apply _work_directory
    std::string remote_file;
    if (file.substr(0, 1) == std::string("/"))
      remote_file = file;
    else
      remote_file = _work_directory + "/" + file;

    params[OUTFILE] += Batch::Couple(local_file, remote_file);
  }

  // Time
  if (_maximum_duration_in_second != -1)
    params[MAXWALLTIME] = _maximum_duration_in_second;

  // Queue
  if (_queue != "")
    params[QUEUE] = _queue;

  return params;
}

void 
Launcher::Job::setBatchManagerJobId(Batch::JobId batch_manager_job_id)
{
  _batch_job_id = batch_manager_job_id;
}

Batch::JobId 
Launcher::Job::getBatchManagerJobId()
{
  return _batch_job_id;
}
#endif
