#include <unordered_map>
#include <variant>

namespace ApiRequestDto {
struct ModifyMessageRequestBody {
  std::unordered_map<std::string,
                     std::variant<std::string, bool, uint64_t, double>>
      metadata;
};
}  // namespace ApiRequestDto
