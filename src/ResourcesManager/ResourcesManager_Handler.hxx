//  SALOME ResourcesManager_Handler
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
//  File   : ResourcesManager_Handler.hxx
//  Author : Jean Rahuel
//  Module : SALOME
//$Header: 

#ifndef RESSOURCESMANAGER_HANDLER_HXX
#define RESSOURCESMANAGER_HANDLER_HXX

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER( ResourcesManager )
#include CORBA_SERVER_HEADER( ContainersManager )

#include "utilities.h"
#include "ResourcesManager_Parser.hxx"
#include "ResourcesComputer_Impl.hxx"
#include <qxml.h>
#include <string>
#include <vector>

class ResourcesManager_Handler : public QXmlDefaultHandler {

  private :

    CORBA::ORB_ptr             _Orb ;
    PortableServer::POA_ptr    _Poa ;

//    Resources::ListOfComputers_var _ComputersList ;
    vector< Computer_Impl * >      _ComputersVector ;
    Resources::ComputerParameters  _ComputerParameters ;
    Resources::ComputerEnvironment _ComputerEnvironment ;

    QString errorProt ;

    string content ;

    void ResetParameters() ;

    Containers::System OsEnumValue( const string anOsName ) ;

  public :

    ResourcesManager_Handler( CORBA::ORB_ptr orb ,
		              PortableServer::POA_ptr poa ) ;

    virtual ~ResourcesManager_Handler() ;

//    virtual Resources::ListOfComputers * ListOfComputers() ;
    vector< Computer_Impl * > ComputersVector() ;

    virtual bool startDocument() ;

    virtual bool startElement( const QString& , const QString& ,
			       const QString& qName , const QXmlAttributes& atts ) ;

    virtual bool endElement( const QString& , const QString& ,
  			     const QString& qName) ;

    virtual bool characters( const QString & chars) ;

    virtual bool endDocument() ;

    virtual QString errorProtocol() ;

    virtual bool fatalError( const QXmlParseException& exception ) ;

} ;

#endif
