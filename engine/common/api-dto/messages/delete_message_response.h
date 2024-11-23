#pragma once

#include "common/json_serializable.h"

namespace ApiResponseDto {
struct DeleteMessageResponse : JsonSerializable<DeleteMessageResponse> {
  std::string id;
  std::string object;
  bool deleted;

  Json::Value ToJson() override {
    Json::Value json;
    json["id"] = id;
    json["object"] = object;
    json["deleted"] = deleted;
    return json;
  }
};
}  // namespace ApiResponseDto
