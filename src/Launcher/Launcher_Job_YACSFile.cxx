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

#include "Launcher_Job_YACSFile.hxx"

#include <time.h>
#include <sys/stat.h>

Launcher::Job_YACSFile::Job_YACSFile(const std::string & yacs_file)
{
  _yacs_file =  yacs_file;
}

Launcher::Job_YACSFile::~Job_YACSFile() {}


void 
Launcher::Job_YACSFile::setYACSFile(const std::string & yacs_file)
{
  _yacs_file = yacs_file;
}

std::string 
Launcher::Job_YACSFile::getYACSFile()
{
  return _yacs_file;
}

void 
Launcher::Job_YACSFile::setMachineDefinition(const ParserResourcesType & machine_definition)
{
  // Check machine_definition
  if (machine_definition.AppliPath == "")
  {
    std::string mess = "Machine definition must define an application path !, machine name is: " + machine_definition.HostName;
    throw LauncherException(mess);
  }

  Launcher::Job::setMachineDefinition(machine_definition);
}

void
Launcher::Job_YACSFile::update_job()
{
#ifdef WITH_LIBBATCH
  Batch::Parametre params = common_job_params();

  // Adding New Files for this type of job
  // Copy YACS File
  // local file -> If file is not an absolute path, we apply _local_directory
  // remote file -> get only file name from _in_files
  std::string local_file;
  if (_yacs_file.substr(0, 1) == std::string("/"))
    local_file = _yacs_file;
  else
    local_file = _local_directory + "/" + _yacs_file;
  size_t found = _yacs_file.find_last_of("/");
  std::string remote_file = _work_directory + "/" + _yacs_file.substr(found+1);
  params[INFILE] += Batch::Couple(local_file, remote_file);

  // logs
  // local file -> If result_directory is not defined, we use HOME environnement
  std::string log_directory   = "logs";
  std::string log_local_file  = _result_directory + "/" + log_directory;
  std::string log_remote_file = _work_directory   + "/" + log_directory;
  params[OUTFILE] += Batch::Couple(log_local_file, log_remote_file);

  params[EXECUTABLE] = buildSalomeCouplingScript(params);

  // Add in files -> yacs_file and launch_script
  _batch_job->setParametre(params);
#endif
}

#ifdef WITH_LIBBATCH
std::string 
Launcher::Job_YACSFile::buildSalomeCouplingScript(Batch::Parametre params)
{
  // parameters
  std::string work_directory = params[WORKDIR].str();

  // File name
  std::string::size_type p1 = _yacs_file.find_last_of("/");
  std::string::size_type p2 = _yacs_file.find_last_of(".");
  std::string yacs_file_name = _yacs_file.substr(p1+1,p2-p1-1);
  
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
  
  std::string launch_date_port_file = launch_date;
  std::string launch_script = "/tmp/runSalome_" + yacs_file_name + "_" + launch_date + ".sh";
  std::ofstream launch_script_stream;
  launch_script_stream.open(launch_script.c_str(), std::ofstream::out);
   
  // Begin of script
  launch_script_stream << "#! /bin/sh -f" << std::endl;
  launch_script_stream << "cd " << work_directory << std::endl;
  launch_script_stream << "mkdir logs" << std::endl;
  launch_script_stream << "export SALOME_TMP_DIR=" << work_directory << "/logs" << std::endl;

  // -- Generates Catalog Resources
  // TODO

  // Launch SALOME with an appli
  launch_script_stream << _machine_definition.AppliPath << "/runAppli --terminal  --ns-port-log=" << launch_date_port_file <<  " > logs/salome.log 2>&1" << std::endl;
  launch_script_stream << "current=0\n"
		       << "stop=20\n" 
		       << "while ! test -f " << _machine_definition.AppliPath << "/" << launch_date_port_file << "\n"
		       << "do\n"
		       << "  sleep 2\n"
		       << "  let current=current+1\n"
		       << "  if [ \"$current\" -eq \"$stop\" ] ; then\n"
		       << "    echo Error Naming Service failed ! >&2\n"
		       << "    exit\n"
		       << "  fi\n"
		       << "done\n"
		       << "port=`cat " << _machine_definition.AppliPath << "/" << launch_date_port_file << "`\n";
  launch_script_stream << _machine_definition.AppliPath << "/runSession driver " << yacs_file_name << ".xml > logs/yacs.log 2>&1" << std::endl;
  launch_script_stream << _machine_definition.AppliPath << "/runSession killSalomeWithPort.py $port" << std::endl;

  // Return
  launch_script_stream.flush();
  launch_script_stream.close();
  chmod(launch_script.c_str(), 0x1ED);
  return launch_script;
}
#endif
