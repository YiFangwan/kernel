//  SALOME Session : implementation of Session.idl
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : SALOME_Session_Server.cxx
//  Author : Paul RASCLE, EDF
//  Module : SALOME
//  $Header$

#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"
#include "SALOME_NamingService.hxx"
#include <iostream>
#include <unistd.h>
using namespace std;

#include <qthread.h> 
#include <qapplication.h>
#include <qlabel.h>

#include "SALOMEGUI_Application.h"
#include "QAD.h"
#include "QAD_MessageBox.h"
#include "QAD_Application.h"
#include "QAD_Settings.h"
#include "QAD_Config.h"
#include "QAD_Tools.h"
#include "QAD_ResourceMgr.h"


#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Session)
#include CORBA_SERVER_HEADER(SALOMEDS)

#include "utilities.h"

#include "SALOME_Session_i.hxx"

// ----------------------------------------------------------------------------

class SERVANT_Launcher: public QThread
{
  int _argc;
  char**_argv;
  CORBA::ORB_var _orb;
  PortableServer::POA_var _poa;
  QMutex* _GUIMutex;
public:
  SERVANT_Launcher(int argc, char ** argv, CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, QMutex *GUIMutex)
  {
    _argc=argc;
    _argv=argv;
    _orb=CORBA::ORB::_duplicate(orb);
    _poa=PortableServer::POA::_duplicate(poa);
    _GUIMutex=GUIMutex;
  };

  void run()
  {
    try
      {
	INFOS("SERVANT_Launcher thread started");
	SALOME_Session_i * mySALOME_Session = new SALOME_Session_i(_argc, _argv, _orb, _poa, _GUIMutex) ;
	PortableServer::ObjectId_var mySALOME_Sessionid = _poa->activate_object(mySALOME_Session) ;
	INFOS("poa->activate_object(mySALOME_Session)");
      
	CORBA::Object_var obj = mySALOME_Session->_this() ;
	CORBA::String_var sior(_orb->object_to_string(obj)) ;
      
	mySALOME_Session->NSregister();
      
	mySALOME_Session->_remove_ref() ;
      
	PortableServer::POAManager_var pman = _poa->the_POAManager() ;
	pman->activate() ;
	INFOS("pman->activate()");
      
	_orb->run() ;
	_orb->destroy() ;
      }
    catch (CORBA::SystemException&)
      {
	INFOS("Caught CORBA::SystemException.");
      }
    catch (CORBA::Exception&)
      {
	INFOS("Caught CORBA::Exception.");
      }
    catch (...)
      {
	INFOS("Caught unknown exception.");
      }  
  }
};

// ----------------------------------------------------------------------------

//! CORBA server for SALOME Session
/*!
 *  SALOME_Session Server launches a SALOME session servant.
 *  The servant registers to the Naming Service.
 *  See SALOME_Session.idl for interface specification.
 *
 *  Main services offered by the servant are:
 *   - launch GUI
 *   - stop Session (must be idle)
 *   - get session state
 */

int main(int argc, char **argv)
{
  try
    {
      ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
      ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
      CORBA::ORB_var &orb = init( argc , argv ) ;

      long TIMESleep = 250000000;
      int NumberOfTries = 40;
      int a;
      timespec ts_req;
      ts_req.tv_nsec=TIMESleep;
      ts_req.tv_sec=0;
      timespec ts_rem;
      ts_rem.tv_nsec=0;
      ts_rem.tv_sec=0;
      CosNaming::NamingContext_var inc;
      PortableServer::POA_var poa;
      CORBA::Object_var theObj;
      CORBA::Object_var obj;
      CORBA::Object_var object;
      SALOME_NamingService *_NS = SINGLETON_<SALOME_NamingService>::Instance();
      _NS->init_orb(orb);

      int SESSION=0;
      const char * Env = getenv("USE_LOGGER"); 
      int EnvL =0;
      if ((Env!=NULL) && (strlen(Env)))
	EnvL=1;
      CosNaming::Name name;
      name.length(1);
      name[0].id=CORBA::string_dup("Logger");    
      PortableServer::POAManager_var pman; 
      for (int i = 1; i<=NumberOfTries; i++)
	{
	  if (i!=1) 
	    a=nanosleep(&ts_req,&ts_rem);
	  try
	    { 
	      obj = orb->resolve_initial_references("RootPOA");
	      if(!CORBA::is_nil(obj))
		poa = PortableServer::POA::_narrow(obj);
	      if(!CORBA::is_nil(poa))
		pman = poa->the_POAManager();
	      if(!CORBA::is_nil(orb)) 
		theObj = orb->resolve_initial_references("NameService");
	      if (!CORBA::is_nil(theObj))
		inc = CosNaming::NamingContext::_narrow(theObj);
	    }
	  catch( CORBA::COMM_FAILURE& )
	    {
	      MESSAGE( "Session Server: CORBA::COMM_FAILURE: Unable to contact the Naming Service" );
	    }
	  if(!CORBA::is_nil(inc))
	    {
	      MESSAGE( "Session Server: Naming Service was found" );
	      if(EnvL==1)
		{
		  for(int j=1; j<=NumberOfTries; j++)
		    {
		      if (j!=1) 
			a=nanosleep(&ts_req, &ts_rem);
		      try
			{
			  object = inc->resolve(name);
			}
		      catch(CosNaming::NamingContext::NotFound)
			{
			  MESSAGE( "Session Server: Logger Server wasn't found" );
			}
		      catch(...)
			{
			  MESSAGE( "Session Server: Unknown exception" ); 
			}
		      if (!CORBA::is_nil(object))
			{
			  MESSAGE( "Session Server: Loger Server was found" );
			  SESSION=1;
			  break;
			}
		    }
		}
	    }
	  if ((SESSION==1)||((EnvL==0)&&(!CORBA::is_nil(inc))))
	    break;
	}

      // CORBA Servant Launcher
      QMutex _GUIMutex ;

      SERVANT_Launcher * mySERVANT_Launcher = new SERVANT_Launcher(argc, argv, orb, poa, &_GUIMutex);
      mySERVANT_Launcher->start();
      
      // QApplication

      QApplication *_qappl = new QApplication(argc, argv );
      INFOS("creation QApplication");

      QAD_ASSERT ( QObject::connect(_qappl, SIGNAL(lastWindowClosed()), _qappl, SLOT(quit()) ) );
      SALOMEGUI_Application* _mw = new SALOMEGUI_Application ( "MDTV-Standard", "HDF", "hdf" );
      INFOS("creation SALOMEGUI_Application");
      
      SCRUTE(_NS);
      if ( !SALOMEGUI_Application::addToDesktop ( _mw, _NS ) )
	{
	  QAD_MessageBox::error1 ( 0,
				   QObject::tr("ERR_ERROR"), 
				   QObject::tr("ERR_APP_INITFAILED"),
				   QObject::tr("BUT_OK") );
	}
      else
	{
	  
	  QFileInfo prgInfo(argv[0]);
	  QDir prgDir(prgInfo.dirPath(true));
	  QAD_CONFIG->setPrgDir(prgDir);        // CWD is program directory
	  QAD_CONFIG->createConfigFile(false);  // Create config file
	                                        // ~/.tr(MEN_APPNAME)/tr(MEN_APPNAME).conf if there's none
	  QAD_CONFIG->readConfigFile();         // Read config file
	  
	  _qappl->setPalette( QAD_Application::getPalette() ); 

	  /* Run 'SALOMEGUI' application */
	  QAD_Application::run();

	  // T2.12 - catch exceptions thrown on attempts to modified a locked study
	  while (1) 
	    {
	      try 
		{
		  MESSAGE("run(): starting the main event loop");
		  int _ret = _qappl->exec();
		  break;
		}
	      catch(SALOMEDS::StudyBuilder::LockProtection&)
		{
		  MESSAGE("run(): An attempt to modify a locked study has not been handled by QAD_Operation");
		  QApplication::restoreOverrideCursor();
		  QAD_MessageBox::warn1 ( (QWidget*)QAD_Application::getDesktop(),
					  QObject::tr("WRN_WARNING"), 
					  QObject::tr("WRN_STUDY_LOCKED"),
					  QObject::tr("BUT_OK") );
		}
	      catch(...)
		{
		  MESSAGE("run(): An exception has been caught");
		  QApplication::restoreOverrideCursor();
		  QAD_MessageBox::error1 ( (QWidget*)QAD_Application::getDesktop(),
					   QObject::tr("ERR_ERROR"), 
					   QObject::tr("ERR_APP_EXCEPTION"),
					   QObject::tr("BUT_OK") );
		}
	    }

	  QString confMsg = "Settings create $HOME/." 
	    + QObject::tr("MEN_APPNAME") + "/" + QObject::tr("MEN_APPNAME") + ".conf";
	  MESSAGE (confMsg );
	  QAD_CONFIG->createConfigFile(true);
	}
    }
  catch (CORBA::SystemException&)
    {
      INFOS("Caught CORBA::SystemException.");
    }
  catch (CORBA::Exception&)
    {
      INFOS("Caught CORBA::Exception.");
    }
  catch (...)
    {
      INFOS("Caught unknown exception.");
    }
  return 0 ;
}
