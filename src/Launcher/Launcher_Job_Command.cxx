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

#include "Launcher_Job_Command.hxx"

Launcher::Job_Command::Job_Command(const std::string & command)
{
  _command = command;
  _env_file = "";
}

Launcher::Job_Command::~Job_Command() {}

void 
Launcher::Job_Command::setCommand(const std::string & command)
{
  _command = command;
}

std::string 
Launcher::Job_Command::getCommand()
{
  return _command;
}

void 
Launcher::Job_Command::setEnvFile(std::string & env_file)
{
  _env_file = env_file;
}

std::string
Launcher::Job_Command::getEnvFile()
{
  return _env_file;
}

void
Launcher::Job_Command::update_job()
{
#ifdef WITH_LIBBATCH
  Batch::Parametre params = common_job_params();

  // Files
  // local file -> If file is not an absolute path, we apply _local_directory
  // remote file -> get only file name from _in_files

  // Copy command file
  std::string local_file;
  if (_command.substr(0, 1) == std::string("/"))
    local_file = _command;
  else
    local_file = _local_directory + "/" + _command;
  size_t found = _command.find_last_of("/");
  std::string remote_file = _work_directory + "/" + _command.substr(found+1);
  params[INFILE] += Batch::Couple(local_file, remote_file);

  // Copy env file
  if (_env_file != "")
  {
    if (_env_file.substr(0, 1) == std::string("/"))
      local_file = _env_file;
    else
      local_file = _local_directory + "/" + _env_file;
    found = _env_file.find_last_of("/");
    remote_file = _work_directory + "/" + _env_file.substr(found+1);
    params[INFILE] += Batch::Couple(local_file, remote_file);
  }

  params[EXECUTABLE] = buildCommandScript(params, _launch_date);
  _batch_job->setParametre(params);
#endif
}

#ifdef WITH_LIBBATCH
std::string 
Launcher::Job_Command::buildCommandScript(Batch::Parametre params, std::string launch_date)
{
  // parameters
  std::string work_directory = params[WORKDIR].str();

  // File name
  std::string::size_type p1 = _command.find_last_of("/");
  std::string::size_type p2 = _command.find_last_of(".");
  std::string command_name = _command.substr(p1+1,p2-p1-1);
  std::string command_file_name = _command.substr(p1+1);
  
  std::string launch_date_port_file = launch_date;
  std::string launch_script = "/tmp/runCommand_" + command_name + "_" + launch_date + ".sh";
  std::ofstream launch_script_stream;
  launch_script_stream.open(launch_script.c_str(), std::ofstream::out);
   
  // Script
  launch_script_stream << "#! /bin/sh -f" << std::endl;
  launch_script_stream << "cd " << work_directory << std::endl;
  if (_env_file != "")
  {
    std::string::size_type last = _env_file.find_last_of("/");
    launch_script_stream << "source ./" << _env_file.substr(last+1) << std::endl;
  }
  launch_script_stream << "./" << command_file_name << " > " << work_directory <<"/logs/command_" << launch_date << ".log 2>&1" << std::endl;

  // Return
  launch_script_stream.flush();
  launch_script_stream.close();
  chmod(launch_script.c_str(), 0x1ED);
  chmod(_command.c_str(), 0x1ED);
  return launch_script;
}
#endif
