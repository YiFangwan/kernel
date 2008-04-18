// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef __LAUNCHER_HXX__
#define __LAUNCHER_HXX__

#include "BatchLight_BatchManager.hxx"
#include "ResourcesManager.hxx"

#include <string>

class LauncherException
{
public:
  const std::string msg;

  LauncherException(const std::string m) : msg(m) {}
};

class Launcher_cpp
{

public:
  Launcher_cpp();
  ~Launcher_cpp();

  long submitSalomeJob(const std::string fileToExecute ,
		       const std::vector<std::string>& filesToExport ,
		       const std::vector<std::string>& filesToImport ,
		       const BatchLight::batchParams& batch_params,
		       const machineParams& params) throw(LauncherException);

  std::string querySalomeJob( const long jobId, const machineParams& params) throw(LauncherException);
  void deleteSalomeJob( const long jobId, const machineParams& params) throw(LauncherException);
  void getResultSalomeJob( const std::string directory, const long jobId, const machineParams& params ) throw(LauncherException);

  void SetResourcesManager( ResourcesManager_cpp* rm ) { _ResManager = rm; }

protected:
  BatchLight::BatchManager *FactoryBatchManager( const ParserResourcesType& params ) throw(LauncherException);

  std::map <std::string,BatchLight::BatchManager*> _batchmap;
  ResourcesManager_cpp *_ResManager;
};

#endif
