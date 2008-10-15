#include "Salome_file_i.hxx"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "HDFascii.hxx"

using namespace std;

void print_infos(Engines::file * infos) 
{
  cerr << "-------------------------------------------------------------------" << endl;
  cerr << "file_name = " << infos->file_name << endl;
  cerr << "path = " << infos->path << endl;
  cerr << "type = " << infos->type << endl;
  cerr << "source_file_name = " << infos->source_file_name << endl;
  cerr << "status = " << infos->status << endl;
}

void print_state(Engines::SfState * state) 
{
  cerr << "-------------------------------------------------------------------" << endl;
  cerr << "name = " << state->name << endl;
  cerr << "hdf5_file_name = " <<  state->hdf5_file_name << endl;
  cerr << "number_of_files = " <<  state->number_of_files << endl;
  cerr << "files_ok = " <<  state->files_ok << endl;
}


int main (int argc, char * argv[])
{
  system("rm toto cat test.hdf test2.hdf");

  Salome_file_i file;
  Salome_file_i file2;
  Salome_file_i file3;
  Salome_file_i file4;
  Salome_file_i file5;
  Engines::file * infos;
  Engines::SfState * state;
  Engines::files * all_infos;
  PortableServer::POA_var root_poa;
  PortableServer::POAManager_var pman;
  CORBA::Object_var obj;

  cerr << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cerr << "Test of setLocalFile()" << endl;
  file.setLocalFile("/tmp/toto");
  infos = file.getFileInfos("toto");
  print_infos(infos);


  cerr << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cerr << "Test of getFilesInfos()" << endl;
  all_infos = file.getFilesInfos();
  for (int i = 0; i < all_infos->length(); i++)
  {
    print_infos(&((*all_infos)[i]));
  }

  cerr << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cerr << "Test of getSalome_fileState()" << endl;
  state = file.getSalome_fileState();
  print_state(state);

  // We start CORBA ...
  CORBA::ORB_ptr orb = CORBA::ORB_init(argc , argv);
  obj = orb->resolve_initial_references("RootPOA");
  root_poa = PortableServer::POA::_narrow(obj);
  pman = root_poa->the_POAManager();
  pman->activate();

  file2.setLocalFile("/tmp/toto_distributed_source");
  Engines::Salome_file_ptr file2_ref = file2._this();

  cerr << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cerr << "Test of setDistributedFile()" << endl;
  file.setDistributedFile("/tmp/toto_distributed");
  file.connectDistributedFile("toto_distributed", file2_ref);
//  file.setDistributedSourceFile("toto_distributed", "toto_distributed_source");
  infos = file.getFileInfos("toto_distributed");
  print_infos(infos);

  // We create the files ... 
  std::ostringstream oss;
  oss << "/tmp/toto";
  std::ofstream f(oss.str().c_str());
  if (f) 
    f << "blablabla" << std::endl;
 
  std::ostringstream oss2;
  oss2 << "/tmp/toto_distributed_source";
  std::ofstream f2(oss2.str().c_str());
  if (f2) 
    f2 << "bliblibli" << std::endl;

  try 
  {
    file.recvFiles();
  }
  catch (SALOME::SALOME_Exception & e)
  {
    cerr << "Exception : " << e.details.text << endl;
  }

  cerr << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cerr << "Test of getFilesInfos()" << endl;
  all_infos = file.getFilesInfos();
  for (int i = 0; i < all_infos->length(); i++)
  {
    print_infos(&((*all_infos)[i]));
  }

  cerr << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cerr << "Test of getSalome_fileState()" << endl;
  state = file.getSalome_fileState();
  print_state(state);

  orb->destroy();

  file3.setLocalFile("/tmp/toto");
  file3.setLocalFile("/bin/cat");
  state = file3.getSalome_fileState();
  print_state(state);
  file3.save_all("test.hdf");
  file3.setLocalFile("/bin/tutu");
  file3.save("test2.hdf");

  file4.load("test.hdf");
  all_infos = file4.getFilesInfos();
  for (int i = 0; i < all_infos->length(); i++)
  {
    print_infos(&((*all_infos)[i]));
  }
  state = file4.getSalome_fileState();
  print_state(state);
  file5.load("test2.hdf");
  all_infos = file5.getFilesInfos();
  for (int i = 0; i < all_infos->length(); i++)
  {
    print_infos(&((*all_infos)[i]));
  }
  state = file5.getSalome_fileState();
  print_state(state);

  // Test of ConvertFromHDFToASCII
  // and ConvertFromASCIIToHDF
  cerr << "Test of ConvertFromASCIIToHDF" << endl;
  HDFascii::ConvertFromASCIIToHDF("/tmp/toto"); // RETURN NULL !
  cerr << "Test of ConvertFromHDFToASCII" << endl;
  cerr << HDFascii::ConvertFromHDFToASCII("test2.hdf", false) << endl;
  cerr << HDFascii::ConvertFromHDFToASCII("test2.hdf", true) << endl;

  cerr << "End of tests" << endl;
}
