#include <event2/event.h>

#include <iostream>
using namespace std;

int main() {
  cout << "Hello, libevent!" << endl;

  // Initialize a new event base
  struct event_base* base = event_base_new();
  if (!base) {
    cerr << "Could not initialize libevent!" << endl;
    return 1;
  }

  cout << "Libevent initialized successfully." << endl;

  // Clean up
  event_base_free(base);
  return 0;
}
