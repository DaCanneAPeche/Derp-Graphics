#include "core/timer.hpp"

namespace dg
{

  void Timer::start()
  {
    startingTime = std::chrono::steady_clock::now();
    m_hasStarted = true;
  }

  void Timer::update()
  {
    assert(m_hasStarted && "Timer needs to start before being updated");
    if (getElapsedTime() >= m_timeout) m_callback();
  }

  timerUnit Timer::getElapsedTime()
  {
    assert(m_hasStarted && "Timer needs to start before getting elapsed time");

    auto currentTime = std::chrono::steady_clock::now(); 
    return std::chrono::duration_cast<timerUnit>(currentTime - startingTime);
  }

}
