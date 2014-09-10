#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <apex.hpp>
#include <sstream>

#define NUM_THREADS 3

class ApexProxy {
private:
  std::string _name;
public:
  ApexProxy(const char * func, const char * file, int line);
  ~ApexProxy();
};

ApexProxy::ApexProxy(const char * func, const char * file, int line) {
  std::ostringstream s;
  s << func << " [" << file << ":" << line << "]";
  _name = std::string(s.str());
  apex::start(_name);
}

ApexProxy::~ApexProxy() {
  apex::stop(_name);
};

typedef void*(*start_routine_t)(void*);

void* someThread(void* tmp)
{
  apex::register_thread("threadTest thread");
  ApexProxy proxy = ApexProxy(__func__, __FILE__, __LINE__);
  printf("PID of this process: %d\n", getpid());
#if defined (__APPLE__)
  printf("The ID of this thread is: %lu\n", (unsigned long)pthread_self());
#else
  printf("The ID of this thread is: %u\n", (unsigned int)pthread_self());
#endif
  sleep(2); // Keep it alive so we're sure the second thread gets a unique ID.
  //apex::finalize();
  return NULL;
}


int main(int argc, char **argv)
{
  apex::init(argc, argv, NULL);
  apex::set_node_id(0);
  //ApexProxy proxy = ApexProxy(__func__, __FILE__, __LINE__);
  printf("PID of this process: %d\n", getpid());
  pthread_t thread[NUM_THREADS];
  int i;
  for (i = 0 ; i < NUM_THREADS ; i++) {
    pthread_create(&(thread[i]), NULL, someThread, NULL);
    sleep(1); // Hack to avoid race for stdout.
  }
  for (i = 0 ; i < NUM_THREADS ; i++) {
    pthread_join(thread[i], NULL);
  }
  apex::finalize();
  return(0);
}

