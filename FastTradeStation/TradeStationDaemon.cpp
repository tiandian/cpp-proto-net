#include "LogManager.h"
#include "Configuration.h"
#include "ClientManager.h"

#include <iostream>
#include <string>
#include <unistd.h>

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <google/protobuf/stubs/common.h>

#define FST_VERSION "1.2"

using namespace std;

CLogManager	logger;
CConfiguration config;

int main(int argc, char* argv[])
{
	cout << "Fast Trade Station v" << FST_VERSION << " Daemon started up." << endl;

	if(!config.Load(argc, argv))
		return 1;

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	boost::asio::io_service io_service;
	// Register signal handlers so that the daemon may be shut down. 
	boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
	
	// Inform the io_service that we are about to become a daemon. The
	// io_service cleans up any internal resources, such as threads, that may
	// interfere with forking.
	io_service.notify_fork(boost::asio::io_service::fork_prepare);

	// Fork the process and have the parent exit. If the process was started
	// from a shell, this returns control to the user. Forking a new process is
	// also a prerequisite for the subsequent call to setsid().
	if (pid_t pid = fork())
	{
		if (pid > 0)
		{
			// We're in the parent process and need to exit.
			//
			// When the exit() function is used, the program terminates without
			// invoking local variables' destructors. Only global variables are
			// destroyed. As the io_service object is a local variable, this means
			// we do not have to call:
			//
			//   io_service.notify_fork(boost::asio::io_service::fork_parent);
			//
			// However, this line should be added before each call to exit() if
			// using a global io_service object. An additional call:
			//
			//   io_service.notify_fork(boost::asio::io_service::fork_prepare);
			//
			// should also precede the second fork().
			exit(0);
		}
		else
		{
			cout << "First fork failed." << endl;
			return 1;
		}
	}

	// Make the process a new session leader. This detaches it from the
	// terminal.
	setsid();

	// A process inherits its working directory from its parent. This could be
	// on a mounted filesystem, which means that the running daemon would
	// prevent this filesystem from being unmounted. Changing to the root
	// directory avoids this problem.
	chdir("/");

	// The file mode creation mask is also inherited from the parent process.
	// We don't want to restrict the permissions on files created by the
	// daemon, so the mask is cleared.
	umask(0);

	// A second fork ensures the process cannot acquire a controlling terminal.
	if (pid_t pid = fork())
	{
		if (pid > 0)
		{
			exit(0);
		}
		else
		{
			//cout << "Second fork failed" << endl;
			return 1;
		}
	}

	// Close the standard streams. This decouples the daemon from the terminal
	// that started it.
	close(0);
	close(1);
	close(2);

	// We don't want the daemon to have any standard input.
	if (open("/dev/null", O_RDONLY) < 0)
	{
		//cout << "Unable to open /dev/null" << endl;
		return 1;
	}

	// Inform the io_service that we have finished becoming a daemon. The
	// io_service uses this opportunity to create any internal file descriptors
	// that need to be private to the new process.
	io_service.notify_fork(boost::asio::io_service::fork_child);

	logger.Init(config.LogConfigPath());

	CClientManager clientManager;
	boost::shared_ptr<SessionManager> pSessionManager(SessionManager::Create());
	pSessionManager->RegisterHandler(&clientManager);
	const string& sAddr = config.GetAddr();
	const string& sPort = config.GetPort();
	pSessionManager->Listen(sAddr, sPort);
	logger.Info(boost::str(boost::format("Start listening at %s:%s") 
		% (sAddr.empty() ? "localhost" : sAddr.c_str()) % sPort.c_str()));

	signals.async_wait(
		boost::bind(&SessionManager::Close, pSessionManager.get()));
	
	// The io_service is now waiting for signal terminate
	io_service.run();
	
	logger.Info("Daemon stop normally.");

	return 0;
}