
#include "Service.h"
#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>

#ifdef WIN32

#include "UTF16.h"
#include <Windows.h>

namespace
{
	void WINAPI service_thread_main(DWORD argc, LPTSTR* argv);
	BOOL WINAPI control_handler(DWORD ctrl_type);
	VOID WINAPI service_ctrl(DWORD ctrl_code);

	BOOL report_status(DWORD current_state, DWORD win32_exitcode, DWORD wait_hint);

	int run_debug(std::vector<std::string> args);
	int run_service();
	int run_install();
	int run_uninstall();
	void print_help();

	std::function<std::unique_ptr<Service>(std::vector<std::string> args)> service_start;
	std::unique_ptr<Service> instance;
	std::string service_name;
	std::mutex mutex;
	std::condition_variable event_condition;
	bool stop_flag = false;
	bool reload_flag = false;

	bool debug_mode;
	int check_point;
	SERVICE_STATUS_HANDLE handle_service_status;
	SERVICE_STATUS service_status;

	int run_debug(std::vector<std::string> args)
	{
		debug_mode = true;
		SetConsoleCtrlHandler(&control_handler, TRUE);

		try
		{
			instance = service_start(std::move(args));
			while (true)
			{
				std::unique_lock<std::mutex> lock(mutex);
				event_condition.wait(lock, [&]() { return stop_flag || reload_flag; });

				if (stop_flag)
					break;

				reload_flag = false;
				instance->reload();
			}
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		instance.reset();
		return 0;
	}

	int run_service()
	{
		SERVICE_TABLE_ENTRY dispatch_table[] =
		{
			{ (LPTSTR)service_name.c_str(), (LPSERVICE_MAIN_FUNCTION)&service_thread_main },
			{ 0, 0 }
		};

		BOOL start_service = StartServiceCtrlDispatcher(dispatch_table);
		if (start_service == FALSE)
			return 0;

		return 0;
	}

	void WINAPI service_thread_main(DWORD argc, LPTSTR* argv)
	{
		memset(&service_status, 0, sizeof(SERVICE_STATUS));
		service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		service_status.dwServiceSpecificExitCode = 0;
		if (!debug_mode)
			handle_service_status = RegisterServiceCtrlHandler(to_utf16(service_name).c_str(), service_ctrl);

		report_status(SERVICE_START_PENDING, NO_ERROR, 2000);

		std::vector<std::string> args;
		for (DWORD i = 0; i < argc; i++)
			args.push_back(from_utf16(argv[i]));

		report_status(SERVICE_RUNNING, NO_ERROR, 0);

		try
		{
			// the API wants us to do this during start pending, but that's not very good as the SCM will get angry fast if we don't boot within the timeout sent to it.
			instance = service_start(std::move(args));

			while (true)
			{
				std::unique_lock<std::mutex> lock(mutex);
				event_condition.wait(lock, [&]() { return stop_flag || reload_flag; });

				if (stop_flag)
					break;

				reload_flag = false;
				instance->reload();
			}
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}

		instance.reset();
		report_status(SERVICE_STOPPED, NO_ERROR, 2000);
	}

	BOOL WINAPI control_handler(DWORD ctrl_type)
	{
		if (instance == 0)
			return FALSE;

		switch (ctrl_type)
		{
		case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
		case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
			report_status(SERVICE_STOP_PENDING, NO_ERROR, 15000);
			{
				std::unique_lock<std::mutex> lock(mutex);
				stop_flag = true;
			}
			event_condition.notify_all();
			return TRUE;
		}
		return FALSE;
	}

	BOOL report_status(DWORD current_state, DWORD win32_exitcode, DWORD wait_hint)
	{
		if (debug_mode)
			return TRUE;

		if (current_state != SERVICE_START_PENDING)
			service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		service_status.dwCurrentState = current_state;
		service_status.dwWin32ExitCode = win32_exitcode;
		service_status.dwWaitHint = wait_hint;

		switch (current_state)
		{
		case SERVICE_RUNNING:
		case SERVICE_STOPPED:
			break;
		default:
			service_status.dwCheckPoint = check_point++;
		}

		return SetServiceStatus(handle_service_status, &service_status);
	}

	VOID WINAPI service_ctrl(DWORD ctrl_code)
	{
		if (instance == 0)
			return;

		if (ctrl_code == SERVICE_CONTROL_STOP)
		{
			report_status(SERVICE_STOP_PENDING, NO_ERROR, 1000);

			{
				std::unique_lock<std::mutex> lock(mutex);
				stop_flag = true;
			}
			event_condition.notify_all();
		}
		else
		{
			report_status(service_status.dwCurrentState, NO_ERROR, 0);
		}
	}

	int run_install()
	{
		WCHAR exe_filename[MAX_PATH];
		BOOL result = GetModuleFileName(0, exe_filename, MAX_PATH);
		exe_filename[MAX_PATH - 1] = 0;
		if (result == FALSE)
		{
			std::cout << "Could not install service. Unable to retrieve executable path." << std::endl;
			return 1;
		}

		SC_HANDLE handle_manager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
		if (handle_manager == 0)
		{
			std::cout << "Could not install service.Failed to open service control manager." << std::endl;
			return 2;
		}

		SC_HANDLE handle_service = CreateService(
			handle_manager, to_utf16(service_name).c_str(), to_utf16(service_name).c_str(),
			SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START,
			SERVICE_ERROR_NORMAL, exe_filename, 0, 0, 0, 0, 0);
		if (handle_service == 0)
		{
			CloseServiceHandle(handle_manager);
			std::cout << "Could not create service in service control manager." << std::endl;
			return 3;
		}

		CloseServiceHandle(handle_service);
		CloseServiceHandle(handle_manager);

		std::cout << service_name.c_str() << " installed." << std::endl;
		return 0;
	}

	int run_uninstall()
	{
		SC_HANDLE handle_manager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
		if (handle_manager == 0)
		{
			std::cout << "Could not uninstall service. Failed to open service control manager." << std::endl;
			return 1;
		}

		SC_HANDLE handle_service = OpenService(handle_manager, to_utf16(service_name).c_str(), SERVICE_ALL_ACCESS);
		if (handle_service == 0)
		{
			CloseServiceHandle(handle_manager);
			std::cout << "Could not open service in service control manager." << std::endl;
			return 2;
		}

		SERVICE_STATUS service_status;
		memset(&service_status, 0, sizeof(SERVICE_STATUS));
		if (ControlService(handle_service, SERVICE_CONTROL_STOP, &service_status))
		{
			std::cout << "Stopping " << service_name.c_str() << "..." << std::endl;
			Sleep(1000);
			while (QueryServiceStatus(handle_service, &service_status))
			{
				if (service_status.dwCurrentState == SERVICE_STOP_PENDING)
				{
					std::cout << ".";
					std::cout.flush();
					Sleep(1000);
				}
				else
				{
					break;
				}
			}
			std::cout << std::endl;
			if (service_status.dwCurrentState == SERVICE_STOPPED)
				std::cout << service_name.c_str() << " stopped." << std::endl;
			else
				std::cout << service_name.c_str() << " failed to stop." << std::endl;
		}

		if (DeleteService(handle_service))
			std::cout << service_name.c_str() << " removed." << std::endl;
		else
			std::cout << "Unable to uninstall service. DeleteService failed." << std::endl;

		CloseServiceHandle(handle_service);
		CloseServiceHandle(handle_manager);
		return 0;
	}

	void print_help()
	{
		std::cout << "Service parameters:" << std::endl;
		std::cout << "  -install            - Install service" << std::endl;
		std::cout << "  -uninstall          - Remove the service" << std::endl;
		std::cout << "  -debug              - Run service as a console application" << std::endl;
		std::cout << std::endl;
		std::cout << "StartServiceCtrlDispatcher being called." << std::endl;
		std::cout << "This may take several seconds. Please wait." << std::endl;
	}
}

int Service::main(int argc, char** argv, const std::string& serviceName, std::function<std::unique_ptr<Service>(std::vector<std::string> args)> serviceStart)
{
	service_start = serviceStart;

	service_name = serviceName;
	debug_mode = 0;
	check_point = 1;

	std::vector<std::string> args;
	for (int i = 0; i < argc; i++)
		args.push_back(argv[i]);

	if (argc == 2 && args[1] == "-debug")
	{
		return run_debug(std::move(args));
	}
	else if (argc == 2 && args[1] == "-install")
	{
		return run_install();
	}
	else if (argc == 2 && args[1] == "-uninstall")
	{
		return run_uninstall();
	}
	else
	{
		print_help();
		return run_service();
	}
}

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace
{
	std::function<std::unique_ptr<Service>(std::vector<std::string> args)> service_start;
	std::unique_ptr<Service> instance;
	bool stop_flag;
	bool reload_flag;
	std::condition_variable event_condition;
	std::mutex mutex;

	void service_thread_main(std::vector<std::string> args)
	{
		instance = service_start(std::move(args));
		while (true)
		{
			std::unique_lock<std::mutex> lock(mutex);
			event_condition.wait(lock, [&]() { return stop_flag; });

			if (stop_flag)
				break;

			reload_flag = false;
			instance->reload();
		}
		instance.reset();
	}

	void sig_term(int signal_code)
	{
		if (instance)
		{
			std::unique_lock<std::mutex> lock(mutex);
			stop_flag = true;
			lock.unlock();
			event_condition.notify_all();
		}
	}

	void sig_hup(int signal_code)
	{
		if (instance)
		{
			std::unique_lock<std::mutex> lock(mutex);
			reload_flag = true;
			lock.unlock();
			event_condition.notify_all();
		}
	}

	int run_daemon(std::vector<std::string> args)
	{
		try
		{
			int file = open(args[2].c_str(), O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); // ug+rx, o+r
			if (file == -1)
				throw std::runtime_error("Could not create pid file");

			struct sigaction action;
			memset(&action, 0, sizeof(struct sigaction));
			action.sa_handler = &sig_term;
			sigaction(SIGTERM, &action, nullptr);
			memset(&action, 0, sizeof(struct sigaction));
			action.sa_handler = &sig_hup;
			sigaction(SIGHUP, &action, nullptr);

			int pid = fork();
			if (pid)
			{
				std::string str_pid = std::to_string(pid) + "\n";
				write(file, str_pid.c_str(), str_pid.length());
				close(file);
				return pid;
			}
			else
			{
				close(file);
				close(0);
				close(1);
				close(2);

				// Starting service in seperate thread to avoid
				// signals sent to this pid causing EINTR errors
				// randomly in the service itself.
				std::thread thread([=]() {service_thread_main(args); });
				while (true)
				{
					std::unique_lock<std::mutex> lock(mutex);
					event_condition.wait(lock, [&]() { return stop_flag || reload_flag; });

					if (stop_flag)
						break;

				}
				thread.join();
				return 0;
			}
		}
		catch (std::exception& e)
		{
			std::cout << "Unhandled exception: " << e.what() << std::endl;
		}
		return 1;
	}

	int run_debug(std::vector<std::string> args)
	{
		struct sigaction action;
		memset(&action, 0, sizeof(struct sigaction));
		action.sa_handler = &Service_Unix::sig_term;
		sigaction(SIGTERM, &action, nullptr);
		memset(&action, 0, sizeof(struct sigaction));
		action.sa_handler = &Service_Unix::sig_hup;
		sigaction(SIGHUP, &action, nullptr);
		memset(&action, 0, sizeof(struct sigaction));
		action.sa_handler = &Service_Unix::sig_term;
		sigaction(SIGINT, &action, nullptr);

		// Starting service in separate thread to avoid
		// signals sent to this pid causing EINTR errors
		// randomly in the service itself.
		std::thread thread([=]() {service_thread_main(args); });

		while (true)
		{
			std::unique_lock<std::mutex> lock(mutex);
			event_condition.wait(lock, [&]() { return stop_flag || reload_flag; });

			if (stop_flag)
				break;
		}
		thread.join();
		return 0;
	}
}

int Service::main(int argc, char** argv, const std::string& serviceName, std::function<std::unique_ptr<Service>(std::vector<std::string> args)> serviceStart)
{
	service_start = serviceStart;

	std::vector<std::string> args;
	for (int i = 0; i < argc; i++)
		args.push_back(argv[i]);

	if (argc >= 3 && args[1] == "-daemon")
	{
		return run_daemon(args);
	}
	else if (argc == 2 && args[1] == "-debug")
	{
		return run_debug(args);
	}
	else
	{
		std::cout << "Service parameters:" << std::endl;
		std::cout << "  -daemon <pidfile>   - Start service as an unix daemon" << std::endl;
		std::cout << "  -debug              - Debug service" << std::endl;
		return 0;
	}
}

#endif
