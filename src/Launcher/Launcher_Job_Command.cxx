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

  std::string command = "";
  if (_env_file != "")
    command = "source " + _env_file + "\n";
  command += _command;
  params[EXECUTABLE] = command;
  _batch_job->setParametre(params);
#endif
}
