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
Launcher::Job_YACSFile::update_job()
{
#ifdef WITH_LIBBATCH
  Batch::Parametre params = common_job_params();

  params[EXECUTABLE] = buildSalomeCouplingScript();
  _batch_job->setParametre(params);
#endif
}

std::string 
Launcher::Job_YACSFile::buildSalomeCouplingScript()
{
}
