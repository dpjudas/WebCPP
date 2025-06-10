#pragma once

class WebModule;
class WebContext;

enum class WebserverType
{
	httpSys,
	tcpSocket
};

class Webserver
{
public:
	Webserver();
	virtual ~Webserver() = default;

	void addModule(std::shared_ptr<WebModule> module);
	virtual void start(const std::string& listenUrl = "http://+:8080/") = 0;

	static std::unique_ptr<Webserver> create(WebserverType type);

	void processRequest(WebContext* context);

private:
	std::vector<std::shared_ptr<WebModule>> modules;
};
