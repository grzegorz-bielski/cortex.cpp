#pragma once

#include <variant>
#include <vector>
#include "common/message.h"

namespace ApiRequestDto {
struct CreateMessageRequestBody {
  ThreadMessage::Role role;

  std::variant<std::string, std::vector<ThreadMessage::Content>> content;

  std::optional<std::vector<ThreadMessage::Attachment>> attachments;

  std::unordered_map<std::string,
                     std::variant<std::string, bool, uint64_t, double>>
      metadata;
};
}  // namespace ApiRequestDto
