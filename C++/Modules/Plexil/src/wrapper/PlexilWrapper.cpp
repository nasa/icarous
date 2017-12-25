#include "PlexilWrapper.h"

#include "ExecApplication.hh"
#include "AdapterConfiguration.hh"
#include "ExternalInterface.hh"
#include "InterfaceManager.hh"
#include "InterfaceSchema.hh"
#include "TimeAdapter.hh"
#include "AdapterFactory.hh"
#include "Debug.hh"
#include "Node.hh"
#include "lifecycle-utils.h"

#if HAVE_LUV_LISTENER
#include "LuvListener.hh"
#endif

#include <cstring>
#include <fstream>
#include <interfaces/CfsAdapter/CfsAdapter.hh>

#define ToCPP_Exec(a) (reinterpret_cast<PLEXIL::ExecApplication*>(a))
#define ToC_Exec(a)   (reinterpret_cast<plexilExec*>(a))

#define ToCPP_Adp(a) (reinterpret_cast<PLEXIL::InterfaceAdapter*>(a))
#define ToC_Adp(a) (reinterpret_cast<plexilInterfaceAdapter*>(a))


using namespace PLEXIL;

int plexil_init(int argc, char** argv,struct plexilExec** exec,struct plexilInterfaceAdapter **adp){


  std::string planName("error");
  std::string debugConfig("Debug.cfg");
  std::string interfaceConfig("interface-config.xml");
  std::string resourceFile("resource.data");
  std::vector<std::string> libraryNames;
  std::vector<std::string> libraryPath;
  std::string
    usage(
	            "Usage: universalExec -p <plan>\n\
                    [-l <library_file>]*         (no default)\n\
                    [-L <library_directory>]*    (default .)\n\
                    [-c <interface_config_file>] (default ./interface-config.xml)\n\
                    [-d <debug_config_file>]     (default ./Debug.cfg)\n\
                    [+d]                         (disable debug messages)\n");
  bool luvRequest = false;

  #if HAVE_LUV_LISTENER
  std::string luvHost = PLEXIL::LuvListener::LUV_DEFAULT_HOSTNAME();
  int luvPort = PLEXIL::LuvListener::LUV_DEFAULT_PORT();
  bool luvBlock = false;
  usage += "                    [-v [-h <luv_hostname>] [-n <luv_portnumber>] [-b] ]\n";
  #endif

  bool debugConfigSupplied = false;
  bool useDebugConfig = true;
  bool resourceFileSupplied = false;
  bool useResourceFile = true;

    //PLEXIL::setDebugOutputStream(std::cout);
    //PLEXIL::enableMatchingDebugMessages("CfsAdapter:executeCommand");

  // if not enough parameters, print usage
  if (argc < 2) {
    std::cout << usage << std::endl;
    return 1;
  }

  // parse out parameters
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-c") == 0) {
      if (argc == (++i)) {
	std::cerr << "Error: Missing argument to the " << argv[i - 1] << " option.\n"
		  << usage << std::endl;
	return 1;
      }
      interfaceConfig = std::string(argv[i]);
    }
    else if (strcmp(argv[i], "-d") == 0) {
      if (!useDebugConfig) {
	warn("Both -d and +d options specified.\n"
	     << usage);
	return 2;
      }
      else if (debugConfigSupplied) {
	warn("Multiple -d options specified.\n"
	     << usage);
	return 2;
      }
      else if (argc == (++i)) {
	std::cerr << "Error: Missing argument to the " << argv[i - 1] << " option.\n"
		  << usage << std::endl;
	return 2;
      }
      debugConfig = std::string(argv[i]);
      useDebugConfig = true;
      debugConfigSupplied = true;
    }
    else if (strcmp(argv[i], "+d") == 0) {
      if (debugConfigSupplied) {
	warn("Both -d and +d options specified.\n"
	     << usage);
	return 2;
      }
      debugConfig.clear();
      useDebugConfig = false;
    }
    else if (strcmp(argv[i], "-l") == 0) {
      if (argc == (++i)) {
	std::cerr << "Error: Missing argument to the " << argv[i - 1] << " option.\n"
		  << usage << std::endl;
	return 2;
      }
      libraryNames.push_back(argv[i]);
    }
    else if (strcmp(argv[i], "-L") == 0) {
      if (argc == (++i)) {
	std::cerr << "Error: Missing argument to the " << argv[i - 1] << " option.\n"
		  << usage << std::endl;
	return 2;
      }
      libraryPath.push_back(argv[i]);
    }
    else if (strcmp(argv[i], "-h") == 0) {
      if (!luvRequest) {
	// interpret as simple help request
	std::cout << usage << std::endl;
	return 0;
      }
      #if HAVE_LUV_LISTENER
      else if (argc == (++i)) {
	std::cerr << "Error: Missing argument to the " << argv[i - 1] << " option.\n"
		  << usage << std::endl;
	return 2;
      }
      luvHost = argv[i];
      #endif
    }
    else if (strcmp(argv[i], "-p") == 0) {
      if (argc == (++i)) {
	std::cerr << "Error: Missing argument to the " << argv[i - 1] << " option.\n"
		  << usage << std::endl;
	return 2;
      }
      planName = argv[i];
    }
    else if (strcmp(argv[i], "-r") == 0) {
      if (!useResourceFile) {
	warn("Both -r and +r options specified.\n"
	     << usage);
	return 2;
      }
      else if (resourceFileSupplied) {
	warn("Multiple -r options specified.\n"
	     << usage);
	return 2;
      }
      else if (argc == (++i)) {
	warn("Missing argument to the " << argv[i-1] << " option.\n"
	     << usage);
	return 2;
      }
      resourceFile = std::string(argv[i]);
      useResourceFile = true;
      resourceFileSupplied = true;
    }
    else if (strcmp(argv[i], "+r") == 0) {
      if (resourceFileSupplied) {
	warn("Both -r and +r options specified.\n"
	     << usage);
	return 2;
      }
      resourceFile.clear();
      useResourceFile = false;
    }
    #if HAVE_LUV_LISTENER
    else if (strcmp(argv[i], "-v") == 0)
      luvRequest = true;
    else if (strcmp(argv[i], "-n") == 0) {
      if (argc == (++i)) {
	std::cerr << "Error: Missing argument to the " << argv[i - 1] << " option.\n"
		  << usage << std::endl;
	return 2;
      }
      std::istringstream buffer(argv[i]);
      buffer >> luvPort;
    }
    else if (strcmp(argv[i], "-b") == 0)
      luvBlock = true;
    #endif
    else {
      std::cerr << "Error: Unknown option '" << argv[i] << "'.\n" << usage << std::endl;
      return 2;
    }
  }
  // basic initialization

  if (useDebugConfig) {
    std::ifstream dbgConfig(debugConfig.c_str());
    if (dbgConfig.good())
      readDebugConfigStream(dbgConfig);
  }

  // get interface configuration file, if provided
  pugi::xml_document configDoc;
  if (!interfaceConfig.empty()) {
    std::cout << "Reading interface configuration from " << interfaceConfig << std::endl;
    pugi::xml_parse_result parseResult = configDoc.load_file(interfaceConfig.c_str());
    if (parseResult.status != pugi::status_ok) {
      std::cout << "WARNING: unable to load interface configuration file "
		<< interfaceConfig
		<< ":\n "
		<< parseResult.description()
		<< "\nContinuing without interface configuration" << std::endl;
    }
    debugMsg("UniversalExec", " got configuration XML starting with " << configDoc.document_element().name());
  }

  // get Interfaces element
  pugi::xml_node configElt;
  if (configDoc.empty()) {
    // Construct default interface XML
    configElt = configDoc.append_child(PLEXIL::InterfaceSchema::INTERFACES_TAG());
    // Add a time adapter
    pugi::xml_node timeElt = configElt.append_child(PLEXIL::InterfaceSchema::ADAPTER_TAG());
    timeElt.append_attribute("AdapterType").set_value("OSNativeTime");
  }
  else {
    configElt = configDoc.child(PLEXIL::InterfaceSchema::INTERFACES_TAG());
    if (configElt.empty()) {
      std::cout << "ERROR: configuration XML lacks \"" << PLEXIL::InterfaceSchema::INTERFACES_TAG()
		<< "\" element; unable to initialize application"
		<< std::endl;
      return 1;
    }
  }

  #if HAVE_LUV_LISTENER
  // if a luv viewer is to be attached,
  // command line arguments must override config file
  if (luvRequest) {
    pugi::xml_node existing =
      configElt.find_child_by_attribute(InterfaceSchema::LISTENER_TAG(),
					InterfaceSchema::LISTENER_TYPE_ATTR(),
					"LuvListener");
    if (existing)
      configElt.remove_child(existing);

    pugi::xml_document* luvConfig =
      PLEXIL::LuvListener::constructConfigurationXml(luvBlock,
						     luvHost.c_str(),
						     luvPort);
    configElt.append_copy(luvConfig->document_element());
    delete luvConfig;
  }
  #endif

  // construct the application
  PLEXIL::ExecApplication* _app = new PLEXIL::ExecApplication();

  // initialize it
  std::cout << "Initializing application" << std::endl;
  if (useResourceFile) {
    g_interface->readResourceFile(resourceFile);
  }

  if (!_app->initialize(configElt)) {
    std::cout << "ERROR: unable to initialize application"
	      << std::endl;
    return 1;
  }

  // add library path
  if (!libraryPath.empty())
    _app->addLibraryPath(libraryPath);

  // start interfaces
  std::cout << "Starting interfaces" << std::endl;
  if (!_app->startInterfaces()) {
    std::cout << "ERROR: unable to start interfaces"
	      << std::endl;
    return 1;
  }

  // start the application
  std::cout << "Starting the exec" << std::endl;
  if (!_app->run()) {
    std::cout << "ERROR: Failed to start Exec" << std::endl;
    return 1;
  }

  // Below this point, must be careful to shut down gracefully
  bool error = false;

  // if specified on command line, load Plexil libraries
  for (std::vector<std::string>::const_iterator libraryName = libraryNames.begin();
       libraryName != libraryNames.end();
       ++libraryName) {
    std::cout << "Loading library node from file '" << *libraryName << "'" << std::endl;
    if (!_app->loadLibrary(*libraryName)) {
      std::cout << "ERROR: unable to load library " << *libraryName << std::endl;
      error = true;
    }
  }

  // load the plan
  if (!error && planName != "error") {
    pugi::xml_document plan;
    pugi::xml_parse_result parseResult = plan.load_file(planName.c_str());
    if (parseResult.status != pugi::status_ok) {
      std::cout << "Error parsing plan " << planName
		<< " (offset " << parseResult.offset << "): "
		<< parseResult.description() << std::endl;
      error = true;
    }
    else if (!_app->addPlan(&plan)) {
      std::cout << "Unable to load plan '" << planName << "', exiting" << std::endl;
      error = true;
    }
  }

  *exec = ToC_Exec(_app);
  *adp = ToC_Adp(g_configuration->getDefaultInterface());

  if(error){
    return 1;
  }
}

void plexil_run(struct plexilExec* exec){
    PLEXIL::ExecApplication *_app = ToCPP_Exec(exec);

    _app->notifyExec();
}

int plexil_destroy(struct plexilExec* exec){

  PLEXIL::ExecApplication *_app = ToCPP_Exec(exec);
  
  if (!_app->stop()) {
    std::cout << "ERROR: failed to stop Exec" << std::endl;
    return 1;
    }
  
  if (!_app->shutdown()) {
    std::cout << "ERROR: failed to shut down Exec" << std::endl;
    return 1;
  }


}

int plexil_getLookup(struct plexilInterfaceAdapter* adp,PlexilCommandMsg* msg){

    InterfaceAdapter* adapter = ToCPP_Adp(adp);

    CfsAdapter *cfsAdap = (CfsAdapter*) adapter;

    return cfsAdap->GetLookUpQueueMsg(msg);
}

int plexil_getCommand(struct plexilInterfaceAdapter* adp,PlexilCommandMsg* msg){

    InterfaceAdapter* adapter = ToCPP_Adp(adp);

    CfsAdapter *cfsAdap = (CfsAdapter*) adapter;

    int n = cfsAdap->GetCmdQueueMsg(msg);

    return n;
}

void plexil_return(struct plexilInterfaceAdapter* adp,PlexilCommandMsg* msg){
    InterfaceAdapter* adapter = ToCPP_Adp(adp);

    CfsAdapter *cfsAdap = (CfsAdapter*) adapter;

    cfsAdap->HandleReturnValue(msg);

    return;
}