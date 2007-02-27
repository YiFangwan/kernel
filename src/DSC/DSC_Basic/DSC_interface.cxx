
// André Ribes EDF R&D - 2006
// 

#include "DSC_interface.hxx"

Engines_DSC_interface::Engines_DSC_interface() {}

Engines_DSC_interface::~Engines_DSC_interface() {}

/* Ajoute le provides port "name" avec sa reference "ref".
 * Lève PortAlreadyDefined si le port est déjà défini.
 * Lève NilPort si la reference du port est nil.
 */
void
Engines_DSC_interface::add_provides_port(Ports::Port_ptr ref, 
				 const char* provides_port_name,
				 Ports::PortProperties_ptr port_prop) 
throw (Engines::DSC::PortAlreadyDefined,
       Engines::DSC::NilPort,
       Engines::DSC::BadProperty) 
{
  // Test des arguments de la méthode
  assert(provides_port_name);
  if (CORBA::is_nil(ref))
    throw Engines::DSC::NilPort();
  if (CORBA::is_nil(port_prop))
    throw Engines::DSC::BadProperty();

  my_ports_it = my_ports.find(provides_port_name);
  if (my_ports_it ==  my_ports.end()) {
    // Creation d'un nouveau port de type provides.
    port_t * new_port = new port_t();
    new_port->type = provides;
    new_port->connection_nbr = 0;
    new_port->provides_port_ref = Ports::Port::_duplicate(ref);
    new_port->port_prop = Ports::PortProperties::_duplicate(port_prop);

    // Enregistrement du port dans la map.
    my_ports[provides_port_name] = new_port;
  }
  else
    throw Engines::DSC::PortAlreadyDefined();
}

/* Ajoute le uses port "name" avec son repository_id Corba.
 * Lève PortAlreadyDefined si le port est déjà défini.
 */
void
Engines_DSC_interface::add_uses_port(const char* repository_id, 
			     const char* uses_port_name,
			     Ports::PortProperties_ptr port_prop) 
throw (Engines::DSC::PortAlreadyDefined,
       Engines::DSC::BadProperty) 
{
  // Test des arguments de la méthode
  // Note : Il est difficile de tester si la chaîne du 
  // repository_id est valide ...
  assert(repository_id);
  assert(uses_port_name);
  if (CORBA::is_nil(port_prop))
    throw Engines::DSC::BadProperty();

  my_ports_it = my_ports.find(uses_port_name);
  if (my_ports_it ==  my_ports.end()) {
    // Creation d'un nouveau port de type uses.
    port_t * new_port = new port_t();
    new_port->type = uses;
    new_port->connection_nbr = 0;
    new_port->uses_port_refs.length(0);
    new_port->repository_id = repository_id;
    new_port->port_prop = Ports::PortProperties::_duplicate(port_prop);

    // Enregistrement du port dans la map.
    my_ports[uses_port_name] = new_port;
  }
  else
    throw Engines::DSC::PortAlreadyDefined();
}

/* Fournit le port provides qui a le nom "provides_port_name" dans les my_ports.
 * Lève l'exception Engines::DSC::PortNotDefined s'il ne trouve
 * pas le port.
 * Le booleen permet de savoir s'il faut donner ou non le port s'il est connecté
 * ou non.
 */ 
Ports::Port_ptr
Engines_DSC_interface::get_provides_port(const char* provides_port_name,
				 const CORBA::Boolean connection_error) 
  throw (Engines::DSC::PortNotDefined,
	 Engines::DSC::PortNotConnected, 
	 Engines::DSC::BadPortType) 
{
  // Test des arguments de la méthode
  assert(provides_port_name);

  Ports::Port_ptr rtn_port = Ports::Port::_nil();
//   std::cout << "---- DSC_Interface : MARK 1 ---- Recherche de : " << provides_port_name << "----" << std::endl;
//   ports::iterator it;
//   std::cout << "----> ";
//   for(it=my_ports.begin();it!=my_ports.end();++it) 
//     std::cout << "|"<<(*it).first<<"|, ";
//   std::cout << std::endl;
  
  my_ports_it = my_ports.find(provides_port_name);
  if (my_ports_it ==  my_ports.end())
    throw Engines::DSC::PortNotDefined();
  if (my_ports[provides_port_name]->type != provides) {
    Engines::DSC::BadPortType BPT;
    BPT.expected = CORBA::string_dup("Expected a provides port");
    BPT.received = CORBA::string_dup((std::string("Received a uses/none port : ")+provides_port_name).c_str());
    throw BPT;
  }

  if (my_ports[provides_port_name]->connection_nbr == 0 && connection_error)
    throw Engines::DSC::PortNotConnected();

  rtn_port = Ports::Port::_duplicate(my_ports[provides_port_name]->provides_port_ref);
  return rtn_port;
}

/* Fournit le port uses qui a le nom "uses_port_name" s'il existe et s'il est 
 * connecté.
 * Dans le cas contraire lève les exceptions :
 * PortNotDefined, PortNotConnected.
 */
Engines::DSC::uses_port * 
Engines_DSC_interface::get_uses_port(const char* uses_port_name) 
  throw (Engines::DSC::PortNotDefined,
	 Engines::DSC::PortNotConnected,
	 Engines::DSC::BadPortType) 
{
  // Test des arguments de la méthode
  assert(uses_port_name);

  Engines::DSC::uses_port * rtn_port = NULL;  
  // On commence par tester si le port existe
  my_ports_it = my_ports.find(uses_port_name);
  if (my_ports_it == my_ports.end())
    throw Engines::DSC::PortNotDefined();
  if (my_ports[uses_port_name]->type != uses){
    Engines::DSC::BadPortType BPT;
    BPT.expected = CORBA::string_dup("Expected a uses port");
    BPT.received = CORBA::string_dup((std::string("Received a provides/none port : ")+uses_port_name).c_str());
    throw BPT;
  }

  // On regarde maintenant si le port est connecté
  if (my_ports[uses_port_name]->connection_nbr > 0) {
    rtn_port = new Engines::DSC::uses_port(my_ports[uses_port_name]->uses_port_refs);
  }
  else
    throw Engines::DSC::PortNotConnected();
  
  return rtn_port;
}


/* 
 * Cette méthode prévient par le biais de l'attribut connection_nbr
 * que le port provides est connecté à un uses port de plus.
 * Notons que pour le moment le provides_port n'a pas de référence sur le composant
 * qui détient le uses port. Le modèle actuel considère que c'est au 
 * "framework" ou a l'application de gérer les connexions et les déconnexions.
 * Il n'y a donc pas de callback entre deux ports connectés.
 *
 */
void
Engines_DSC_interface::connect_provides_port(const char* provides_port_name)
    throw (Engines::DSC::PortNotDefined)
{
  assert(provides_port_name);

  // Le port uses existe t'il ?
  my_ports_it = my_ports.find(provides_port_name);
  if (my_ports_it ==  my_ports.end())
    throw Engines::DSC::PortNotDefined();
  if (my_ports[provides_port_name]->type != provides)
    throw Engines::DSC::PortNotDefined();


  // Augmentation du nombre de connexions
  my_ports[provides_port_name]->connection_nbr += 1;
  // On prévient le code utilisateur
  provides_port_changed(provides_port_name, 
			my_ports[provides_port_name]->connection_nbr,
			Engines::DSC::AddingConnection
		       );
}

/* Cette méthode permet d'ajouter une connexion à un port uses.
 * Elle appelle ensuite une méthode abstraite que le composant doit 
 * implémenter afin d'être averti lorsque les connexions changent.
 * En effet, l'utilisateur doit ensuite prendre le nouveau uses_port
 * fournit dans le callback.
 *
 */
void
Engines_DSC_interface::connect_uses_port(const char* uses_port_name,
					 Ports::Port_ptr provides_port_ref) 
  throw (Engines::DSC::PortNotDefined,
	 Engines::DSC::BadPortType,
	 Engines::DSC::NilPort)
{
  assert(uses_port_name);

  // Le port uses existe t'il ?
  my_ports_it = my_ports.find(uses_port_name);
  if (my_ports_it ==  my_ports.end())
    throw Engines::DSC::PortNotDefined();
  if (my_ports[uses_port_name]->type != uses) {
    Engines::DSC::BadPortType BPT;
    BPT.expected = CORBA::string_dup("Expected a uses port");
    BPT.received = CORBA::string_dup((std::string("Received a provides/none port : ")+uses_port_name).c_str());
    throw BPT;
  }

  // La reference est-elle nulle ?
  if (CORBA::is_nil(provides_port_ref))
    throw Engines::DSC::NilPort();

  // Le type est-il correct ?
  const char * repository_id = my_ports[uses_port_name]->repository_id.c_str();
  if (provides_port_ref->_is_a(repository_id)) 
  {
    // Ajout dans la sequence
    CORBA::ULong lgth = my_ports[uses_port_name]->uses_port_refs.length();
    my_ports[uses_port_name]->
      uses_port_refs.length(lgth + 1);
    my_ports[uses_port_name]->uses_port_refs[lgth] = 
      Ports::Port::_duplicate(provides_port_ref);

    // Augmentation du nombre de connexions
    my_ports[uses_port_name]->connection_nbr += 1;
    
    // Appel du callback pour prévenir le uses port a été modifié
    uses_port_changed(uses_port_name,
		      new Engines::DSC::uses_port(my_ports[uses_port_name]->uses_port_refs),
		      Engines::DSC::AddingConnection);
  }
  else {
    Engines::DSC::BadPortType BPT;
    BPT.expected = CORBA::string_dup("Expected ...");
    BPT.received = CORBA::string_dup((std::string("Received an incorrect repository id type ")+
				      repository_id).c_str());
    throw BPT;
  }

}

/* Cette méthode teste si le uses port "name" est connecté.
 * Léve PortNotDefined si le port n'existe pas.
 */
CORBA::Boolean
Engines_DSC_interface::is_connected(const char* port_name) 
  throw (Engines::DSC::PortNotDefined) 
{
  assert(port_name);

  CORBA::Boolean rtn = false;

  // Le port existe t-il ?
  my_ports_it = my_ports.find(port_name);
  if (my_ports_it ==  my_ports.end())
    throw Engines::DSC::PortNotDefined();

  // Le port est-il connecté ?
  if (my_ports[port_name]->connection_nbr > 0)
    rtn = true;

  return rtn;
}

void
Engines_DSC_interface::disconnect_provides_port(const char* provides_port_name,
					const Engines::DSC::Message message)
throw (Engines::DSC::PortNotDefined,
       Engines::DSC::PortNotConnected)
{
  assert(provides_port_name);

  // Le port existe t-il ?
  my_ports_it = my_ports.find(provides_port_name);
  if (my_ports_it ==  my_ports.end())
    throw Engines::DSC::PortNotDefined();
  if (my_ports[provides_port_name]->type != provides)
    throw Engines::DSC::PortNotDefined();

  // Le port est-il connecté ?
  if (my_ports[provides_port_name]->connection_nbr > 0) 
  {
    my_ports[provides_port_name]->connection_nbr -= 1;
    provides_port_changed(provides_port_name,
			  my_ports[provides_port_name]->connection_nbr,
			  message);
  }
  else
    throw Engines::DSC::PortNotConnected();
}

void
Engines_DSC_interface::disconnect_uses_port(const char* uses_port_name,
				    Ports::Port_ptr provides_port_ref,
				    const Engines::DSC::Message message)
throw (Engines::DSC::PortNotDefined,
       Engines::DSC::PortNotConnected,
       Engines::DSC::BadPortReference) 
{
  assert(uses_port_name);

  // Le port existe t-il ?
  my_ports_it = my_ports.find(uses_port_name);
  if (my_ports_it ==  my_ports.end())
    throw Engines::DSC::PortNotDefined();
  if (my_ports[uses_port_name]->type != uses)
    throw Engines::DSC::PortNotDefined();

  // Le port est-il connecté ?
  if (my_ports[uses_port_name]->connection_nbr > 0) {
    // On cherche le port dans la sequence representant le
    // uses port.
    if (CORBA::is_nil(provides_port_ref))
      throw Engines::DSC::BadPortReference();

    CORBA::Long port_index = -1;
    CORBA::ULong seq_length = my_ports[uses_port_name]->uses_port_refs.length(); 
    for(int i = 0; i < seq_length; i++)
    {
      if (my_ports[uses_port_name]->uses_port_refs[i]->_is_equivalent(provides_port_ref))
      {
	port_index = i;
	break;
      }
    }

    if (port_index == -1)
      throw Engines::DSC::BadPortReference();

    my_ports[uses_port_name]->connection_nbr -= 1;
    Engines::DSC::uses_port * new_uses_port = 
      new Engines::DSC::uses_port();
    new_uses_port->length(seq_length - 1);

    int index_ancien = 0;
    int index_nouveau = 0;
    for(;index_ancien < seq_length;) {
      if (index_ancien == port_index) 
      {
	// Rien a faire !
	// On ne change pas le index du nouveau tableau
	index_ancien += 1;
      }
      else 
      {
	(*new_uses_port)[index_nouveau] = my_ports[uses_port_name]->uses_port_refs[index_ancien];
	index_ancien += 1;
	index_nouveau += 1;
      }
    }

    // On remplace la sequence ...
    my_ports[uses_port_name]->uses_port_refs = *new_uses_port;

    // Rq c'est à l'utilisateur de détruire 
    // la sequence s'il n'en veut pas !!!
    uses_port_changed(uses_port_name,
		      new_uses_port,
		      message);
  }
  else
    throw Engines::DSC::PortNotConnected();
}

Ports::PortProperties_ptr
Engines_DSC_interface::get_port_properties(const char* port_name) 
  throw (Engines::DSC::PortNotDefined) 
{
  assert(port_name);

  Ports::PortProperties_ptr rtn_properties = Ports::PortProperties::_nil();

  // Le port existe t-il ?
  my_ports_it = my_ports.find(port_name);
  if (my_ports_it ==  my_ports.end())
    throw Engines::DSC::PortNotDefined();

  rtn_properties = Ports::PortProperties::_duplicate(my_ports[port_name]->port_prop);
  return rtn_properties;
}
