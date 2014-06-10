#pragma once

#include <string>
#include <memory>

#include <pugixml.hpp>

typedef void (*StatusCallback)(pugi::xml_document& xmldoc);

class status_server {
public:
  status_server(StatusCallback cb);
  int reconfigure(const std::string& socketPath);
  void destroy();
  void servingLoop();

private:
  void serveClient(int clientFd);
  int openBindListenUnixSocket(const std::string& socketPath);
  bool socketWaitReadable(int fd, int timeout_ms);
  StatusCallback _callback;
  std::string _socketPath;
  int _bindFd;
  volatile bool _hangupThread;
  pthread_t _tid;
  bool _threadIsUp;
};
