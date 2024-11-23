#include <json/value.h>

template <typename T>
struct JsonSerializable {

  virtual Json::Value ToJson() = 0;

  T FromJson(const Json::Value& json) {
    throw std::runtime_error("Not implemented");
  }

  T FromJson(Json::Value&& json) {
    throw std::runtime_error("Not implemented");
  }

  virtual ~JsonSerializable() = default;
};
