#include <chrono>
#include <iostream>
#include <thread>

#include "pickup/utils/StopWatch.hpp"

using namespace pickup::utils;

int main() {
  StopWatch sw;
  sw.start();

  std::cout << "Stopwatch started. Sleeping for 2 seconds..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));

  sw.stop();
  std::cout << "Stopwatch stopped." << std::endl;

  std::cout << "Elapsed time in milliseconds: " << sw.elapsed() << " ms" << std::endl;
  std::cout << "Elapsed time in seconds: " << sw.elapsed<std::chrono::seconds>() << " s" << std::endl;

  return 0;
}