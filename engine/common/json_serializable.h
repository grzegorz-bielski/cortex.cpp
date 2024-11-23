#include <json/value.h>

template <typename T>
struct JsonSerializable {

  virtual Json::Value ToJson() = 0;

  // virtual T FromJson(const Json::Value& json) = 0;
  //
  // virtual T FromJson(Json::Value&& json) = 0;

  virtual ~JsonSerializable() = default;
};
