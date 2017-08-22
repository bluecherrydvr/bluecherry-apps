#pragma once

#include <string>
#include <memory>
#include <map>

class trigger_processor;

// Singleton

class trigger_server {
public:
  static trigger_server& Instance()
  {
    static trigger_server theSingleInstance;
    return theSingleInstance;
  }

  int reconfigure(const std::string& socketPath);
  void destroy();
  void servingLoop();

  void register_processor(int camera_id, trigger_processor *processor);
  void unregister_processor(int camera_id, trigger_processor *processor);
  trigger_processor *find_processor(int camera_id);

private:
  trigger_server();
  ~trigger_server();
  trigger_server(const trigger_server& root);
  trigger_server& operator=(const trigger_server&);

  void serveClient(int clientFd);
  int openBindListenUnixSocket(const std::string& socketPath);
  bool socketWaitReadable(int fd, int timeout_ms);
  std::string _socketPath;
  int _bindFd;
  volatile bool _hangupThread;
  pthread_t _tid;
  bool _threadIsUp;

  pthread_mutex_t processor_registry_lock;
  std::map<int, trigger_processor*>processor_registry;
};
