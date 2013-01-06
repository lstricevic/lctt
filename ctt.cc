// lctt.cc - This is the source code of Light Corba Testing Tool based on the
//   	       examples from "The Basics" of the omniORB user guide.
//
//        On startup, the object reference is registered with the
//        COS naming service. The client uses the naming service to
//        locate this object.
//
//        The name which the object is bound to is as follows:
//              root  [context]
//               |
//              test  [context] kind [my_context]
//               |
//              Echo  [object]  kind [Object]
//

#include "echo.hh"

#ifdef HAVE_STD
#  include <iostream>
   using namespace std;
#else
#  include <iostream.h>
#endif


// This is the object implementation.
#include "timer.cc"
#include "options.cc"

static CORBA::Boolean      bindObjectToName(CORBA::ORB_ptr, CORBA::Object_ptr);
static CORBA::Object_ptr   getObjectReference(CORBA::ORB_ptr orb);
CORBA::Boolean transientHandler(void* pCookie, CORBA::ULong nRetries, const CORBA::TRANSIENT& ex);
CORBA::Boolean commFailureHandler(void* pCookie, CORBA::ULong nRetries, const CORBA::COMM_FAILURE& ex);

Options *gOpt = new Options();

class Echo_i : public POA_Echo
{
private:
	char *cp;
	inline void incCount(void){
		int i=gOpt->chunkSize-2;
		while(cp[i]=='9'){
			cp[i]='0';
			i--;
		}
		cp[i]++;

	}
public:
  inline Echo_i() {
	cp = new char[gOpt->chunkSize];
	memset(cp, '0', gOpt->chunkSize);
	cp[gOpt->chunkSize-1]='\0';
  }
  virtual ~Echo_i() {delete [] cp;}
  virtual char* echoString(const char* mesg);
};


char* Echo_i::echoString(const char* mesg)
{
	incCount();

  return CORBA::string_dup(cp);
}


//////////////////////////////////////////////////////////////////////

// This function acts as a client to the object.

static double hello(Echo_ptr e)
{

  CORBA::String_var src = (const char*) "Hello!";
  // String literals are (char*) rather than (const char*) on some
  // old compilers.  Thus it is essential to cast to (const char*)
  // here to ensure that the string is copied, so that the
  // CORBA::String_var does not attempt to 'delete' the string
  // literal.

  CORBA::String_var dest;
	Timer t1;
  for (int i=1;i<=gOpt->nChunks;i++)
	dest = e->echoString(src);

//  cout << "I said, \"" << (char*)src << "\"." << endl
//       << "The Echo object replied, \"" << (char*)dest <<"\"." << endl;
	return t1.getsec();
}

//////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	cout<<"CORBA Testing Tool v0.0.4"<<endl;
  gOpt->getCL(argc, argv);

  try {
    // Initialise the ORB.
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

    // Is this server?
		if(gOpt->server){
			cout<<"server"<<endl;
  
      // Obtain a reference to the root POA.
      CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
      PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
  
      // We allocate the object on the heap.  Since this is a reference
      // counted object, it will be deleted by the POA when it is no
      // longer needed.
      Echo_i* myecho = new Echo_i();
  
      // Activate the object.  This tells the POA that this object is
      // ready to accept requests.
      PortableServer::ObjectId_var myechoid = poa->activate_object(myecho);
  
      // Obtain a reference to the object, and register it in
      // the naming service.
      obj = myecho->_this();
  
      CORBA::String_var x;
      x = orb->object_to_string(obj);
      cout << x << endl;
  
      if( !bindObjectToName(orb, obj) )
        return 1;
  
      // Decrement the reference count of the object implementation, so
      // that it will be properly cleaned up when the POA has determined
      // that it is no longer needed.
      myecho->_remove_ref();
  
      // Obtain a POAManager, and tell the POA to start accepting
      // requests on its objects.
      PortableServer::POAManager_var pman = poa->the_POAManager();
      pman->activate();
      orb->run();
		}
		else {
    // If it is not a server, it is a client
			cout<<"client"<<endl;

			omniORB::setClientCallTimeout(gOpt->callTimeout);
      omniORB::installTransientExceptionHandler(NULL, transientHandler);
      cout << "Transient exception handler installed." << endl;
      omniORB::installCommFailureExceptionHandler(NULL, commFailureHandler);
      cout << "CommFailure exception handler installed." << endl;


      CORBA::Object_var obj = getObjectReference(orb);
  
      Echo_var echoref = Echo::_narrow(obj);
  		if( CORBA::is_nil(echoref) ) {
    		cerr << "The object reference is nil!\n" << endl;
				return 1;
		  }
  
			double totalTime=0;
      for (CORBA::ULong count=1; count <= gOpt->testCount; count++){
        double testTime = hello(echoref);
				totalTime+=testTime;
				cout<<"Test "<< count <<" time: "<<testTime<<"s"<<endl;
			}
			cout<<"--------------------------------------"<<endl;
			cout<<"Total test time: "<< totalTime <<"s (Average "<<totalTime/gOpt->testCount<<"s per test)"<<endl;
  
      orb->destroy();
		cout<<"client end"<<endl;
		}
  }
  catch(CORBA::TRANSIENT&) {
    cerr << "Caught system exception TRANSIENT -- unable to contact the "
         << "server." << endl;
  }
  catch(CORBA::SystemException& ex) {
    cerr << "Caught CORBA::" << ex._name() << endl;
  }
  catch(CORBA::Exception& ex) {
    cerr << "Caught CORBA::Exception: " << ex._name() << endl;
  }
  catch(omniORB::fatalException& fe) {
    cerr << "Caught omniORB::fatalException:" << endl;
    cerr << "  file: " << fe.file() << endl;
    cerr << "  line: " << fe.line() << endl;
    cerr << "  mesg: " << fe.errmsg() << endl;
  }
  return 0;
}

//////////////////////////////////////////////////////////////////////

static CORBA::Boolean
bindObjectToName(CORBA::ORB_ptr orb, CORBA::Object_ptr objref)
{
  CosNaming::NamingContext_var rootContext;

  try {
    // Obtain a reference to the root context of the Name service:
    CORBA::Object_var obj;
    obj = orb->resolve_initial_references("NameService");

    // Narrow the reference returned.
    rootContext = CosNaming::NamingContext::_narrow(obj);
    if( CORBA::is_nil(rootContext) ) {
      cerr << "Failed to narrow the root naming context." << endl;
      return 0;
    }
  }
  catch (CORBA::NO_RESOURCES&) {
    cerr << "Caught NO_RESOURCES exception. You must configure omniORB "
	 << "with the location" << endl
	 << "of the naming service." << endl;
    return 0;
  }
  catch (CORBA::ORB::InvalidName&) {
    // This should not happen!
    cerr << "Service required is invalid [does not exist]." << endl;
    return 0;
  }

  try {
    // Bind a context called "test" to the root context:

    CosNaming::Name contextName;
    contextName.length(1);
    contextName[0].id   = (const char*) "test";       // string copied
    contextName[0].kind = (const char*) "my_context"; // string copied
    // Note on kind: The kind field is used to indicate the type
    // of the object. This is to avoid conventions such as that used
    // by files (name.type -- e.g. test.ps = postscript etc.)

    CosNaming::NamingContext_var testContext;
    try {
      // Bind the context to root.
      testContext = rootContext->bind_new_context(contextName);
    }
    catch(CosNaming::NamingContext::AlreadyBound& ex) {
      // If the context already exists, this exception will be raised.
      // In this case, just resolve the name and assign testContext
      // to the object returned:
      CORBA::Object_var obj;
      obj = rootContext->resolve(contextName);
      testContext = CosNaming::NamingContext::_narrow(obj);
      if( CORBA::is_nil(testContext) ) {
        cerr << "Failed to narrow naming context." << endl;
        return 0;
      }
    }

    // Bind objref with name Echo to the testContext:
    CosNaming::Name objectName;
    objectName.length(1);
    objectName[0].id   = (const char*) "Echo";   // string copied
    objectName[0].kind = (const char*) "Object"; // string copied

    try {
      testContext->bind(objectName, objref);
    }
    catch(CosNaming::NamingContext::AlreadyBound& ex) {
      testContext->rebind(objectName, objref);
    }
    // Note: Using rebind() will overwrite any Object previously bound
    //       to /test/Echo with obj.
    //       Alternatively, bind() can be used, which will raise a
    //       CosNaming::NamingContext::AlreadyBound exception if the name
    //       supplied is already bound to an object.

    // Amendment: When using OrbixNames, it is necessary to first try bind
    // and then rebind, as rebind on it's own will throw a NotFoundexception if
    // the Name has not already been bound. [This is incorrect behaviour -
    // it should just bind].
  }
  catch(CORBA::TRANSIENT& ex) {
    cerr << "Caught system exception TRANSIENT -- unable to contact the "
         << "naming service." << endl
	 << "Make sure the naming server is running and that omniORB is "
	 << "configured correctly." << endl;

    return 0;
  }
  catch(CORBA::SystemException& ex) {
    cerr << "Caught a CORBA::" << ex._name()
	 << " while using the naming service." << endl;
    return 0;
  }
  return 1;
}

//////////////////////////////////////////////////////////////////////

static CORBA::Object_ptr
getObjectReference(CORBA::ORB_ptr orb)
{
  CosNaming::NamingContext_var rootContext;
  
  try {
    // Obtain a reference to the root context of the Name service:
    CORBA::Object_var obj;
    obj = orb->resolve_initial_references("NameService");

    // Narrow the reference returned.
    rootContext = CosNaming::NamingContext::_narrow(obj);
    if( CORBA::is_nil(rootContext) ) {
      cerr << "Failed to narrow the root naming context." << endl;
      return CORBA::Object::_nil();
    }
  }
  catch (CORBA::NO_RESOURCES&) {
    cerr << "Caught NO_RESOURCES exception. You must configure omniORB "
	 << "with the location" << endl
	 << "of the naming service." << endl;
    return 0;
  }
  catch(CORBA::ORB::InvalidName& ex) {
    // This should not happen!
    cerr << "Service required is invalid [does not exist]." << endl;
    return CORBA::Object::_nil();
  }

  // Create a name object, containing the name test/context:
  CosNaming::Name name;
  name.length(2);

  name[0].id   = (const char*) "test";       // string copied
  name[0].kind = (const char*) "my_context"; // string copied
  name[1].id   = (const char*) "Echo";
  name[1].kind = (const char*) "Object";
  // Note on kind: The kind field is used to indicate the type
  // of the object. This is to avoid conventions such as that used
  // by files (name.type -- e.g. test.ps = postscript etc.)

  try {
    // Resolve the name to an object reference.
    return rootContext->resolve(name);
  }
  catch(CosNaming::NamingContext::NotFound& ex) {
    // This exception is thrown if any of the components of the
    // path [contexts or the object] aren't found:
    cerr << "Context not found." << endl;
  }
  catch(CORBA::TRANSIENT& ex) {
    cerr << "Caught system exception TRANSIENT -- unable to contact the "
         << "naming service." << endl
	 << "Make sure the naming server is running and that omniORB is "
	 << "configured correctly." << endl;

  }
  catch(CORBA::SystemException& ex) {
    cerr << "Caught a CORBA::" << ex._name()
	 << " while using the naming service." << endl;
    return 0;
  }

  return CORBA::Object::_nil();
}

CORBA::Boolean transientHandler(void* pCookie, CORBA::ULong nRetries, const CORBA::TRANSIENT& ex)
{
	cerr << endl << "TRANSIENT handler called. Retries = " << nRetries << endl << endl;
	return ((nRetries < gOpt->nRetries) ? 1 : 0 );
}

CORBA::Boolean commFailureHandler(void* pCookie, CORBA::ULong nRetries, const CORBA::COMM_FAILURE& ex)
{
	cerr << endl << "COMM_FAILURE handler called. Retries = " << nRetries << endl << endl;
	return ((nRetries < gOpt->nRetries) ? 1 : 0 );
}


// vim: ts=2 sw=2 

