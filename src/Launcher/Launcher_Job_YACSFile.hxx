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

#ifndef _LAUNCHER_JOB_YACSFILE_HXX_
#define _LAUNCHER_JOB_YACSFILE_HXX_

#include "Launcher_Job.hxx"
#include "Launcher.hxx"

#ifdef WITH_LIBBATCH
#include <Batch/Batch_Job.hxx>
#endif

namespace Launcher
{
  class Job_YACSFile : virtual public Launcher::Job
  {
    public:
      Job_YACSFile(const std::string & yacs_file);
      virtual ~Job_YACSFile();

      // Specific parameters
      void setYACSFile(const std::string & yacs_file);
      std::string getYACSFile();

      virtual void setMachineDefinition(const ParserResourcesType & machine_definition);

      virtual void update_job();

#ifdef WITH_LIBBATCH
    protected:
      std::string buildSalomeCouplingScript(Batch::Parametre params);
#endif

    private:
      std::string _yacs_file;
  };
}

#endif

