// Copyright (C) 2019  CEA/DEN, EDF R&D
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

%module pylauncher

%{
#include "SALOME_ResourcesCatalog_Parser.hxx"
#include "Launcher_Job.hxx"
%}

class ParserResourcesType
{
public:
  ParserResourcesType();
  ~ParserResourcesType();
  std::string getAccessProtocolTypeStr() const;
  std::string getResourceTypeStr() const;
  std::string getBatchTypeStr() const;
  std::string getMpiImplTypeStr() const;
  std::string getClusterInternalProtocolStr() const;
  std::string getCanLaunchBatchJobsStr() const;
  std::string getCanRunContainersStr() const;

  void setAccessProtocolTypeStr(const std::string & protocolTypeStr);
  void setResourceTypeStr(const std::string & resourceTypeStr);
  void setBatchTypeStr(const std::string & batchTypeStr);
  void setMpiImplTypeStr(const std::string & mpiImplTypeStr);
  void setClusterInternalProtocolStr(const std::string & internalProtocolTypeStr);
  void setCanLaunchBatchJobsStr(const std::string & canLaunchBatchJobsStr);
  void setCanRunContainersStr(const std::string & canRunContainersStr);
};

namespace Launcher
{
  class Job
  {
    public:
      Job();
      virtual ~Job();
      virtual void update_job() = 0;
  };
}
