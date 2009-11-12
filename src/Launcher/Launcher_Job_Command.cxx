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

  // log
  std::string log_file        = "command.log";
  std::string log_local_file  = _result_directory + "/" + log_file;
  std::string log_remote_file = _work_directory   + "/" + log_file;
  params[OUTFILE] += Batch::Couple(log_local_file, log_remote_file);

  params[EXECUTABLE] = buildCommandScript(params);
  _batch_job->setParametre(params);
#endif
}

#ifdef WITH_LIBBATCH
std::string 
Launcher::Job_Command::buildCommandScript(Batch::Parametre params)
{
  // parameters
  std::string work_directory = params[WORKDIR].str();

  // File name
  std::string::size_type p1 = _command.find_last_of("/");
  std::string::size_type p2 = _command.find_last_of(".");
  std::string command_name = _command.substr(p1+1,p2-p1-1);
  
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
  std::string launch_script = "/tmp/runCommand_" + command_name + "_" + launch_date + ".sh";
  std::ofstream launch_script_stream;
  launch_script_stream.open(launch_script.c_str(), std::ofstream::out);
   
  // Script
  launch_script_stream << "#! /bin/sh -f" << std::endl;
  launch_script_stream << "cd " << work_directory << std::endl;
  if (_env_file != "")
     launch_script_stream << "source " << _env_file << std::endl;
  launch_script_stream << _command << " > command.log 2>&1" << std::endl;

  // Return
  launch_script_stream.flush();
  launch_script_stream.close();
  chmod(launch_script.c_str(), 0x1ED);
  chmod(_command.c_str(), 0x1ED);
  return launch_script;
}
#endif
