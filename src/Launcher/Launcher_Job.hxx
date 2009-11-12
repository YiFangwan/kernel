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

#ifndef _LAUNCHER_JOB_HXX_
#define _LAUNCHER_JOB_HXX_

#include <SALOMEconfig.h>
#include "ResourcesManager.hxx"

#include <stdlib.h>

#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <exception>

#ifdef WITH_LIBBATCH
#include <Batch/Batch_Job.hxx>
#include <Batch/Batch_Date.hxx>
#include <Batch/Batch_JobId.hxx>
#endif

namespace Launcher
{
  class Job
  {
    public:
      Job();
      virtual ~Job();

      // Launcher managing parameters
      // State of a Job: CREATED, IN_PROCESS, QUEUED, RUNNING, PAUSED, FINISHED, ERROR
      void setState(const std::string & state);
      std::string getState();

      void setNumber(const int & number);
      int getNumber();

      virtual void setMachineDefinition(const ParserResourcesType & machine_definition);
      ParserResourcesType getMachineDefinition();
      
      // Common parameters
      void setWorkDirectory(const std::string & work_directory);
      void setLocalDirectory(const std::string & local_directory);
      void setResultDirectory(const std::string & result_directory);
      void add_in_file(const std::string & file);
      void add_out_file(const std::string & file);
      void setMaximumDuringTime(const std::string & maximum_during_time);
      void setMachineRequiredParams(const machineParams & machine_required_params);

      std::string getWorkDirectory();
      std::string getLocalDirectory();
      std::string getResultDirectory();
      const std::list<std::string> & get_in_files();
      const std::list<std::string> & get_out_files();
      std::string getMaximumDuringTime();
      machineParams getMachineRequiredParams();
      
      std::string updateJobState();

      // Checks
      void checkMaximumDuringTime(const std::string & maximum_during_time);
      void checkMachineRequiredParams(const machineParams & machine_required_params);

      // Helps
      long convertMaximumDuringTime(const std::string & maximum_during_time);

      // Abstract class
      virtual void update_job() = 0;

    protected:
      int _number;

      std::string _state;

      ParserResourcesType _machine_definition;

      std::string _work_directory;
      std::string _local_directory;
      std::string _result_directory;
      std::list<std::string> _in_files;
      std::list<std::string> _out_files;
      std::string _maximum_during_time;
      long _maximum_during_time_in_second;
      machineParams _machine_required_params;

#ifdef WITH_LIBBATCH
    // Connection with LIBBATCH
    public:      
      Batch::Job * getBatchJob();
      Batch::Parametre common_job_params();
      void setBatchManagerJobId(Batch::JobId batch_manager_job_id);
      Batch::JobId getBatchManagerJobId();

    protected:
      Batch::Job * _batch_job;
      Batch::JobId _batch_job_id;
#endif
  };
}

#endif

