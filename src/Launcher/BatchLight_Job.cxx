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
/*
 * Job.cxx : 
 *
 * Auteur : Bernard SECHER - CEA/DEN
 * Date   : Juillet 2007
 * Projet : SALOME
 *
 */

#include "BatchLight_Job.hxx"
#include <sstream>

using namespace std;
using namespace BatchLight;

Job::Job(const string fileToExecute, 
	 const vector<string>& filesToExport, 
	 const vector<string>& filesToImport, 
	 const batchParams& batch_params) : _fileToExecute(fileToExecute), 
  _filesToExport(filesToExport), 
  _filesToImport(filesToImport), 
  _batch_params(batch_params)
{
  _dirForTmpFiles = "/tmp/default_batch_tmp_directory";
  std::string _fileNameToExecute = "";
}

Job::~Job()
{
  cerr << "Job destructor" << endl;
}

void 
Job::addFileToImportList(std::string file_name) 
{
  _filesToImport.push_back(file_name);
}

const std::string
Job::getExpectedDuringTime() 
{
  std::string str(_batch_params.expected_during_time);
  return str;
}

const std::string 
Job::getMemory()
{
  std::string str(_batch_params.mem);
  return str;
}

bool
Job::check() {
  bool rtn = true;
  cerr << "Warning : batch_directory option is not currently implemented" << endl; 
  cerr << "Warning : currently these informations are only in the PBS batch manager" << endl; 
  cerr << "Job parameters are :"  <<endl;
  cerr << "Directory : $HOME/Batch/$date" << endl;

  // check expected_during_time (check the format)
  std::string edt_info;
  std::string edt_value = _batch_params.expected_during_time.c_str();
  if (edt_value != "") {
    std::string begin_edt_value = edt_value.substr(0, 2);
    std::string mid_edt_value = edt_value.substr(2, 1);
    std::string end_edt_value = edt_value.substr(3);
  
    long value;
    std::istringstream iss(begin_edt_value);
    if (!(iss >> value)) {
      edt_info = "Error on definition ! : " + edt_value;
      rtn = false;
    }
    else if (value < 0) {
      edt_info = "Error on definition time is negative ! : " + value;
      rtn = false;
    }
    std::istringstream iss_2(end_edt_value);
    if (!(iss_2 >> value)) {
      edt_info = "Error on definition ! : " + edt_value;
      rtn = false;
    }
    else if (value < 0) {
      edt_info = "Error on definition time is negative ! : " + value;
      rtn = false;
    }
    if (mid_edt_value != ":") {
      edt_info = "Error on definition ! :" + edt_value;
      rtn = false;
    }
  }
  else {
    edt_info = "No value given";
  }
  cerr << "Expected during time : " << edt_info << endl;

  // check memory (check the format)
  std::string mem_info;
  std::string mem_value = _batch_params.mem.c_str();
  if (mem_value != "") {
    std::string begin_mem_value = mem_value.substr(0, mem_value.length()-2);
    long re_mem_value;
    std::istringstream iss(begin_mem_value);
    if (!(iss >> re_mem_value)) {
      mem_info = "Error on definition ! : " + mem_value;
      rtn = false;
    }
    else if (re_mem_value <= 0) {
      mem_info = "Error on definition memory is negative ! : " + mem_value;
      rtn = false;
    }
    std::string end_mem_value = mem_value.substr(mem_value.length()-2);
    if (end_mem_value != "gb" and end_mem_value != "mb") {
      mem_info = "Error on definition, type is bad ! " + mem_value;
      rtn = false;
    }
  }
  else {
    mem_info = "No value given";
  }
  cerr << "Memory : " << mem_info << endl;

  // check nb_proc
  std::string nb_proc_info;
  ostringstream nb_proc_value;
  nb_proc_value << _batch_params.nb_proc;
  if(_batch_params.nb_proc <= 0) {
    nb_proc_info = "Bad value ! nb_proc = ";
    nb_proc_info += nb_proc_value.str();
    rtn = false;
  }
  else {
    nb_proc_info = nb_proc_value.str();
  }
  cerr << "Nb of processors : " << nb_proc_info << endl;

  return rtn;
}
