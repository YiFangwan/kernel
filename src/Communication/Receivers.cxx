#include "poa.h"
#include "utilities.h"
using namespace std;

#define TAILLE_SPLIT 100000
#define TIMEOUT 20

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
CorbaNCNoCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::CorbaNCNoCopyReceiver(CorbaSender mySender):_mySender(mySender){
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
CorbaNCNoCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::~CorbaNCNoCopyReceiver(){
  _mySender->release();
  CORBA::release(_mySender);
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
void *CorbaNCNoCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::getDistValue(long &size)
{
  TSeqCorba seq=_mySender->send();
  size=seq->length();
  return seq->get_buffer(1);
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
void *CorbaNCNoCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::getValue(long &size)
{
  return Receiver::getValue(size,_mySender);
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
CorbaNCWithCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::CorbaNCWithCopyReceiver(CorbaSender mySender):_mySender(mySender){
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
CorbaNCWithCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::~CorbaNCWithCopyReceiver(){
  _mySender->release();
  CORBA::release(_mySender);
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
void *CorbaNCWithCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::getDistValue(long &size){
  size=_mySender->getSize();
  long n;
  T *ret=new T[size];
  T *iter=ret;
  for(long i=0;i<size;i+=TAILLE_SPLIT)
    {
      if(size-i>TAILLE_SPLIT)
	n=TAILLE_SPLIT;
      else
	n=size-i;
      TSeqCorba seq=_mySender->sendPart(i,n);
      T *seqd=(T *)seq->get_buffer(0);
      for(long j=0;j<n;j++)
	*iter++=*seqd++;
    }
  return ret;
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
void *CorbaNCWithCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::getValue(long &size)
{
  return Receiver::getValue(size,_mySender);
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
CorbaWCNoCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::CorbaWCNoCopyReceiver(CorbaSender mySender):_mySender(mySender){
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
CorbaWCNoCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::~CorbaWCNoCopyReceiver(){
  _mySender->release();
  CORBA::release(_mySender);
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
void *CorbaWCNoCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::getDistValue(long &size){
  size=_mySender->getSize();
  long n;
  T *ret=new T[size];
  T *iter=ret;
  for(long i=0;i<size;i+=TAILLE_SPLIT)
    {
      if(size-i>TAILLE_SPLIT)
	n=TAILLE_SPLIT;
      else
	n=size-i;
      TSeqCorba seq=_mySender->sendPart(i,n);
      TCorba *seqd=seq->get_buffer(0);
      for(long j=0;j<n;j++)
	*iter++=*seqd++;
    }
  return ret;
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
void *CorbaWCNoCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::getValue(long &size)
{
  return Receiver::getValue(size,_mySender);
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
CorbaWCWithCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::CorbaWCWithCopyReceiver(CorbaSender mySender):_mySender(mySender){
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
CorbaWCWithCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::~CorbaWCWithCopyReceiver(){
  _mySender->release();
  CORBA::release(_mySender);
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
void *CorbaWCWithCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::getDistValue(long &size){
  size=_mySender->getSize();
  long n;
  T *ret=new T[size];
  T *iter=ret;
  for(long i=0;i<size;i+=TAILLE_SPLIT)
    {
      if(size-i>TAILLE_SPLIT)
	n=TAILLE_SPLIT;
      else
	n=size-i;
      TSeqCorba seq=_mySender->sendPart(i,n);
      TCorba *seqd=seq->get_buffer(0);
      for(long j=0;j<n;j++)
      *iter++=*seqd++;
    }
  return ret;
}

template<class T,class TCorba,class TSeqCorba,class CorbaSender>
void *CorbaWCWithCopyReceiver<T,TCorba,TSeqCorba,CorbaSender>::getValue(long &size)
{
  return Receiver::getValue(size,_mySender);
}

#ifdef HAVE_MPI2

template<class T,MPI_Datatype T2>
MPIReceiver<T,T2>::MPIReceiver(SALOME::MPISender_ptr mySender):_mySender(mySender){
}

template<class T,MPI_Datatype T2>
MPIReceiver<T,T2>::~MPIReceiver(){
  _mySender->release();
  CORBA::release(_mySender);
}

template<class T,MPI_Datatype T2>
void *MPIReceiver<T,T2>::getDistValue(long &size){
  int i=0;
  int myproc;
  int sproc;
  MPI_Status status;
  MPI_Comm com; 
  char   port_name_clt [MPI_MAX_PORT_NAME];
  float telps, tuser, tsys, tcpu;
  T *_v;
  long _n;

  
  CORBA::Any a; 
  MPI_Comm_rank(MPI_COMM_WORLD, &myproc);
  SALOME::MPISender::param_var p =_mySender->getParam();
  _mySender->send();
  sproc = p->myproc;
  MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
  while ( i != TIMEOUT  && MPI_Lookup_name((char*)p->service,MPI_INFO_NULL,port_name_clt) != MPI_SUCCESS) { 
    i++;
  }       
  MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_ARE_FATAL);
  if ( i == TIMEOUT  ) { 
    MPI_Finalize();
    exit(-1);
  }
  else{
    //       Connect to service, get the inter-communicator server
    //      Attention MPI_Comm_connect est un appel collectif :
    // 	- Si lancement mpirun -c n -----> uniquement     MPI_COMM_SELF fonctionne
    // 	- Si lancement client_server&client_server ----> MPI_COMM_WORLD fonctionne
    
    //      TIMEOUT is inefficient since MPI_Comm_Connect doesn't return if we asked for
    //        a service that has been unpublished !
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
    i = 0;
    while ( i != TIMEOUT  &&  MPI_Comm_connect(port_name_clt, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &com)!=MPI_SUCCESS ) { 
      i++; 
    } 
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_ARE_FATAL);
    if ( i == TIMEOUT ) {
      MPI_Finalize(); 
      exit(-1);
    }
  }
  MPI_Recv( &_n, 1, MPI_LONG, sproc,p->tag1,com,&status);
  _v = new T[_n];
  MPI_Recv( _v, _n, T2, sproc,p->tag2,com,&status);
  _mySender->close(p);
  MPI_Comm_disconnect( &com );  
  size=_n;
  return _v;
}

template<class T,MPI_Datatype T2>
void *MPIReceiver<T,T2>::getValue(long &size)
{
  return Receiver::getValue(size,_mySender);
}

#endif

#ifdef HAVE_SOCKET
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <rpc/xdr.h>

template<class T,int (*myFunc)(XDR*,T*)>
SocketReceiver<T,myFunc>::SocketReceiver(SALOME::SocketSender_ptr mySender) : _mySender(mySender)
{
  _clientSockfd = -1;
  _senderDestruc=true;
}

template<class T,int (*myFunc)(XDR*,T*)>
SocketReceiver<T,myFunc>::~SocketReceiver()
{
  if(_senderDestruc)
    {
      _mySender->release();
      CORBA::release(_mySender);
    }
}

template<class T,int (*myFunc)(XDR*,T*)>
void *SocketReceiver<T,myFunc>::getValue(long &size)
{
  return Receiver::getValue(size,_mySender);
}

template<class T,int (*myFunc)(XDR*,T*)>
void* SocketReceiver<T,myFunc>::getDistValue(long &size)
{
  int n=0, m;
  T *v;
  XDR xp; /* pointeur sur le decodeur XDR */

  try{
    initCom();

    SALOME::SocketSender::param_var p = _mySender->getParam();

    size = p->lend - p->lstart + 1;
    v = new T[size];

    connectCom(p->internet_address, p->myport);
  
    _mySender->send();

    xdrmem_create(&xp,(char*)v,size*sizeof(T),XDR_DECODE );
    while( n < size*sizeof(T) ){
      m = read(_clientSockfd, (char*)v+n, size*sizeof(T)-n);
      if( m < 0 ){
	closeCom();
	delete [] v;
	SALOME::ExceptionStruct es;
	es.type = SALOME::COMM;
	es.text = "error read Socket exception";
	throw SALOME::SALOME_Exception(es);
      }
      n += m;
    }
    xdr_vector( &xp, (char*)v, size, sizeof(T), (xdrproc_t)myFunc);
    xdr_destroy( &xp );
    
    _mySender->endOfCom();
    closeCom();
  }
  catch(SALOME::SALOME_Exception &ex){
    if( ex.details.type == SALOME::COMM )
      {
	_senderDestruc=false;
	cout << ex.details.text << endl;
	throw MultiCommException("Unknown sender protocol");
      }
    else
      throw ex;
  }
 
  return v;
}

template<class T,int (*myFunc)(XDR*,T*)>
void SocketReceiver<T,myFunc>::initCom()
{
  try{
    _mySender->initCom();

    /* Ouverture de la socket */
    _clientSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_clientSockfd < 0) {
      closeCom();
      SALOME::ExceptionStruct es;
      es.type = SALOME::COMM;
      es.text = "error Socket exception";
      throw SALOME::SALOME_Exception(es);
    }
  }
  catch(SALOME::SALOME_Exception &ex){
    if( ex.details.type == SALOME::COMM )
      {
	_senderDestruc=false;
	cout << ex.details.text << endl;
	throw MultiCommException("Unknown sender protocol");
      }
    else
      throw ex;
  }

}

template<class T,int (*myFunc)(XDR*,T*)>
void SocketReceiver<T,myFunc>::connectCom(const char *dest_address, int port)
{
  struct sockaddr_in serv_addr;
  struct hostent * server;
  SALOME::ExceptionStruct es;

  try{
    /* reception of the host structure on the remote process */
    server = gethostbyname(dest_address);
    if( server == NULL ) {
      closeCom();
      es.type = SALOME::COMM;
      es.text = "error unknown host Socket exception";
      _senderDestruc=false;
      throw SALOME::SALOME_Exception(es);
    }

    /* Initialisation of the socket structure */
    bzero((char*)&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bcopy((char *)server->h_addr, 
	  (char *)&serv_addr.sin_addr.s_addr,
	  server->h_length);
    serv_addr.sin_port = htons(port);
    
    if( connect(_clientSockfd, (struct sockaddr *) & serv_addr, sizeof(struct sockaddr)) < 0 ){
      closeCom();
      es.type = SALOME::COMM;
      es.text = "error connect Socket exception";
      _senderDestruc=false;
      throw SALOME::SALOME_Exception(es);
    }

    _mySender->acceptCom();

  }
  catch(SALOME::SALOME_Exception &ex){
    if( ex.details.type == SALOME::COMM )
      {
	_senderDestruc=false;
	cout << ex.details.text << endl;
	throw MultiCommException("Unknown sender protocol");
      }
    else
      throw ex;
  }

}


template<class T,int (*myFunc)(XDR*,T*)>
void SocketReceiver<T,myFunc>::closeCom()
{
  _mySender->closeCom();
  if( _clientSockfd >= 0 ){
    close(_clientSockfd);
    _clientSockfd = -1;
  }
 
}

#endif
