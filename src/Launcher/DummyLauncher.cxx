//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// This implementation is an empty shell compiled only when libBatch is absent.
// It will just throw exceptions when it is called.

#include "Launcher.hxx"

using namespace std;

Launcher_cpp::Launcher_cpp()
{
}

Launcher_cpp::~Launcher_cpp()
{
}

long Launcher_cpp::submitJob(const string xmlExecuteFile,
                             const string clusterName) throw (LauncherException)
{
  throw LauncherException("Method Launcher_cpp::submitJob is not available "
                          "(libBatch was not found at compilation time).");
}

long Launcher_cpp::submitSalomeJob(const string fileToExecute,
                                   const vector<string>& filesToExport,
                                   const vector<string>& filesToImport,
                                   const batchParams& batch_params,
                                   const machineParams& params) throw (LauncherException)
{
  throw LauncherException("Method Launcher_cpp::submitSalomeJob is not available "
                          "(libBatch was not found at compilation time).");
}

string Launcher_cpp::queryJob(long id,
                              const machineParams& params) throw (LauncherException)
{
  throw LauncherException("Method Launcher_cpp::queryJob is not available "
                          "(libBatch was not found at compilation time).");
}

string Launcher_cpp::queryJob(long id,
                              const string clusterName) throw (LauncherException)
{
  throw LauncherException("Method Launcher_cpp::queryJob is not available "
                          "(libBatch was not found at compilation time).");
}

void Launcher_cpp::deleteJob(const long id,
                             const machineParams& params) throw (LauncherException)
{
  throw LauncherException("Method Launcher_cpp::deleteJob is not available "
                          "(libBatch was not found at compilation time).");
}

void Launcher_cpp::deleteJob(long id,
                             const string clusterName) throw (LauncherException)
{
  throw LauncherException("Method Launcher_cpp::deleteJob is not available "
                          "(libBatch was not found at compilation time).");
}

void Launcher_cpp::getResultsJob(const string directory,
                                 const long id,
                                 const machineParams& params) throw (LauncherException)
{
  throw LauncherException("Method Launcher_cpp::getResultsJob is not available "
                          "(libBatch was not found at compilation time).");
}

void Launcher_cpp::getResultsJob(const string directory,
                                 long id,
                                 const string clusterName) throw (LauncherException)
{
  throw LauncherException("Method Launcher_cpp::getResultsJob is not available "
                          "(libBatch was not found at compilation time).");
}
