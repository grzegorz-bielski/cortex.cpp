#pragma once

#include <SQLiteCpp/Database.h>
#include <trantor/utils/Logger.h>
#include <string>
#include <vector>
#include "utils/result.hpp"
#include "utils/json_helper.h"

namespace cortex::db {
struct HardwareEntry {
  std::string uuid;
  std::string type;
  int hardware_id;
  int software_id;
  bool activated;
  std::string ToJsonString() const {
    Json::Value root;
    root["uuid"] = uuid;
    root["type"] = type;
    root["hardware_id"] = hardware_id;
    root["software_id"] = software_id;
    root["activated"] = activated;
    return json_helper::DumpJsonString(root);
  }
};

class Hardwares {

 private:
  SQLite::Database& db_;


 public:
  Hardwares();
  Hardwares(SQLite::Database& db);
  ~Hardwares();

  cpp::result<std::vector<HardwareEntry>, std::string> LoadHardwareList() const;
  cpp::result<bool, std::string> AddHardwareEntry(const HardwareEntry& new_entry);
  cpp::result<bool, std::string> UpdateHardwareEntry(
      const std::string& id, const HardwareEntry& updated_entry);
  cpp::result<bool, std::string> DeleteHardwareEntry(
      const std::string& id);
};
}  // namespace cortex::db