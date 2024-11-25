#include "core/signal_handler.hpp"

namespace dg
{
  void SignalHandler::on(size_t signal, std::function<void(void*)> callback)
  {
    m_signalMap[signal] = callback;
  }

  void SignalHandler::send(size_t signal, void* data)
  {
    if (!m_signalMap.contains(signal)) return;

    m_signalMap[signal](data);
  }
}
