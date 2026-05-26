#pragma once

#include "plog/Log.h"
#include "plog/Init.h"

#include <string>

namespace dg
{
  struct DebugMessage
  {
    std::string text;
    plog::Severity severity;
  };

  template <class Formatter>
  class DebugMessagesStorer : public plog::IAppender
  {
    public:
      virtual void write(const plog::Record& record) PLOG_OVERRIDE
      {
        plog::util::nstring str = Formatter::format(record);
        messages.emplace_back(std::string(str), record.getSeverity());
      }

      std::vector<DebugMessage> messages;

  };
}
