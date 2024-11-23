#include <json/value.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "json_serializable.h"

// Represents a message within a thread.
namespace ThreadMessage {

// The status of the message, which can be either in_progress, incomplete, or completed.
enum class Status { IN_PROGRESS, INCOMPLETE, COMPLETED };

inline std::string StatusToString(Status status) {
  switch (status) {
    case Status::IN_PROGRESS:
      return "in_progress";
    case Status::INCOMPLETE:
      return "incomplete";
    case Status::COMPLETED:
      return "completed";
    default:
      throw new std::invalid_argument("Invalid status: " +
                                      std::to_string((int)status));
  }
}

// On an incomplete message, details about why the message is incomplete.
struct IncompleteDetail : JsonSerializable<IncompleteDetail> {
  // The reason the message is incomplete.
  std::string reason;

  Json::Value ToJson() override {
    Json::Value json;
    json["reason"] = reason;
    return json;
  }
};

// The entity that produced the message. One of user or assistant.
enum class Role { USER, ASSISTANT };

inline std::string RoleToString(Role role) {
  switch (role) {
    case Role::USER:
      return "user";
    case Role::ASSISTANT:
      return "assistant";
    default:
      throw new std::invalid_argument("Invalid role: " +
                                      std::to_string((int)role));
  }
}

struct Content : JsonSerializable<Content> {
  std::string type;

  Content(const std::string& type) : type{type} {}
};

// References an image File in the content of a message.
struct ImageFileContent : Content {

  ImageFileContent() : Content("image_file") {}

  struct ImageFile {
    // The File ID of the image in the message content. Set purpose="vision" when uploading the File if you need to later display the file content.
    std::string file_id;

    // Specifies the detail level of the image if specified by the user. low uses fewer tokens, you can opt in to high resolution using high.
    std::string detail;
  };

  ImageFile image_file;

  Json::Value ToJson() override {
    Json::Value json;
    json["type"] = type;
    json["image_file"]["file_id"] = image_file.file_id;
    json["image_file"]["detail"] = image_file.detail;
    return json;
  }
};

// References an image URL in the content of a message.
struct ImageUrlContent : Content {

  // The type of the content part.
  ImageUrlContent(const std::string& type) : Content(type) {}

  struct ImageUrl {
    // The external URL of the image, must be a supported image types: jpeg, jpg, png, gif, webp.
    std::string url;

    // Specifies the detail level of the image. low uses fewer tokens, you can opt in to high resolution using high. Default value is auto
    std::string detail;
  };

  ImageUrl image_url;

  Json::Value ToJson() override {
    Json::Value json;
    json["type"] = type;
    json["image_url"]["url"] = image_url.url;
    json["image_url"]["detail"] = image_url.detail;
    return json;
  }
};

// The text content that is part of a message.
struct TextContent : Content {
  // Always text.
  TextContent() : Content("text") {}

  struct Annotation : JsonSerializable<Annotation> {
    std::string type;

    // The text in the message content that needs to be replaced.
    std::string text;

    uint32_t start_index;

    uint32_t end_index;

    Annotation(const std::string& type, const std::string& text,
               uint32_t start_index, uint32_t end_index)
        : type{type},
          text{text},
          start_index{start_index},
          end_index{end_index} {}
  };

  // A citation within the message that points to a specific quote from a specific File associated with the assistant or the message. Generated when the assistant uses the "file_search" tool to search files.
  struct FileCitationWrapper : Annotation {

    // Always file_citation.
    FileCitationWrapper(const std::string& text, uint32_t start_index,
                        uint32_t end_index)
        : Annotation("file_citation", text, start_index, end_index) {}

    struct FileCitation {
      // The ID of the specific File the citation is from.
      std::string file_id;
    };

    FileCitation file_citation;

    Json::Value ToJson() override {
      Json::Value json;
      json["text"] = text;
      json["type"] = type;
      json["file_citation"]["file_id"] = file_citation.file_id;
      json["start_index"] = start_index;
      json["end_index"] = end_index;
      return json;
    }
  };

  // A URL for the file that's generated when the assistant used the code_interpreter tool to generate a file.
  struct FilePathWrapper : Annotation {

    // Always file_path.
    FilePathWrapper(const std::string& text, uint32_t start_index,
                    uint32_t end_index)
        : Annotation("file_path", text, start_index, end_index) {}

    struct FilePath {
      // The ID of the file that was generated.
      std::string file_id;
    };

    FilePath file_path;

    Json::Value ToJson() override {
      Json::Value json;
      json["text"] = text;
      json["type"] = type;
      json["file_path"]["file_id"] = file_path.file_id;
      json["start_index"] = start_index;
      json["end_index"] = end_index;
      return json;
    }
  };

  struct Text : JsonSerializable<Text> {
    // The data that makes up the text.
    std::string value;

    std::vector<Annotation> annotations;

    Json::Value ToJson() override {
      Json::Value json;
      json["value"] = value;
      Json::Value annotations_json_arr{Json::arrayValue};
      for (auto& annotation : annotations) {
        annotations_json_arr.append(annotation.ToJson());
      }
      json["annotations"] = annotations_json_arr;
      return json;
    };
  };

  Text text;
};

// The refusal content generated by the assistant.
struct Refusal : Content {

  // Always refusal.
  std::string type = "refusal";

  std::string refusal;

  Json::Value ToJson() override {
    Json::Value json;
    json["type"] = type;
    json["refusal"] = refusal;
    return json;
  }
};

// A list of files attached to the message, and the tools they were added to.
struct Attachment : JsonSerializable<Attachment> {

  // The ID of the file to attach to the message.
  std::string file_id;

  // The tools to add this file to.
  struct Tool {
    std::string type;

    Tool(const std::string& type) : type{type} {}
  };

  // The type of tool being defined: code_interpreter
  struct CodeInterpreter : Tool {
    CodeInterpreter() : Tool{"code_interpreter"} {}
  };

  // The type of tool being defined: file_search
  struct FileSearch : Tool {
    FileSearch() : Tool{"file_search"} {}
  };

  std::vector<Tool> tools;

  Json::Value ToJson() override {
    Json::Value json;
    json["file_id"] = file_id;
    Json::Value tools_json_arr{Json::arrayValue};
    for (auto& tool : tools) {
      Json::Value tool_json;
      tool_json["type"] = tool.type;
      tools_json_arr.append(tool_json);
    }
    json["tools"] = tools_json_arr;
    return json;
  }
};

struct Message : JsonSerializable<Message> {

  // The identifier, which can be referenced in API endpoints.
  std::string id;

  // The object type, which is always thread.message.
  std::string object = "thread.message";

  // The Unix timestamp (in seconds) for when the message was created.
  uint32_t created_at;

  // The thread ID that this message belongs to.
  std::string thread_id;

  // The status of the message, which can be either in_progress, incomplete, or completed.
  Status status;

  // On an incomplete message, details about why the message is incomplete.
  std::optional<IncompleteDetail> incomplete_details;

  // The Unix timestamp (in seconds) for when the message was completed.
  std::optional<uint32_t> completed_at;

  // The Unix timestamp (in seconds) for when the message was marked as incomplete.
  std::optional<uint32_t> incomplete_at;

  Role role;

  // The content of the message in array of text and/or images.
  std::vector<Content> content;

  // If applicable, the ID of the assistant that authored this message.
  std::optional<std::string> assistant_id;

  // The ID of the run associated with the creation of this message. Value is null when messages are created manually using the create message or create thread endpoints.
  std::optional<std::string> run_id;

  // A list of files attached to the message, and the tools they were added to.
  std::optional<std::vector<Attachment>> attachments;

  // Set of 16 key-value pairs that can be attached to an object. This can be useful for storing additional information about the object in a structured format. Keys can be a maximum of 64 characters long and values can be a maximum of 512 characters long.
  std::unordered_map<std::string,
                     std::variant<std::string, bool, uint64_t, double>>
      metadata;

  Json::Value ToJson() override {
    Json::Value json;

    json["id"] = id;
    json["object"] = object;
    json["created_at"] = created_at;
    json["thread_id"] = thread_id;
    json["status"] = StatusToString(status);

    if (incomplete_details.has_value()) {
      json["incomplete_details"] = incomplete_details->ToJson();
    }
    if (completed_at.has_value()) {
      json["completed_at"] = *completed_at;
    }
    if (incomplete_at.has_value()) {
      json["incomplete_at"] = *incomplete_at;
    }

    json["role"] = RoleToString(role);

    Json::Value content_json_arr{Json::arrayValue};
    for (auto& child_content : content) {
      content_json_arr.append(child_content.ToJson());
    }
    json["content"] = content_json_arr;
    if (assistant_id.has_value()) {
      json["assistant_id"] = *assistant_id;
    }
    if (run_id.has_value()) {
      json["run_id"] = *run_id;
    }
    if (attachments.has_value()) {
      Json::Value attachments_json_arr{Json::arrayValue};
      for (auto& attachment : *attachments) {
        attachments_json_arr.append(attachment.ToJson());
      }
      json["attachments"] = attachments_json_arr;
    }

    Json::Value metadata_json{Json::objectValue};
    for (const auto& [key, value] : metadata) {
      if (std::holds_alternative<bool>(value)) {
        metadata_json[key] = std::get<bool>(value);
      } else if (std::holds_alternative<uint64_t>(value)) {
        metadata_json[key] = std::get<uint64_t>(value);
      } else if (std::holds_alternative<double>(value)) {
        metadata_json[key] = std::get<double>(value);
      } else {
        metadata_json[key] = std::get<std::string>(value);
      }
    }
    json["metadata"] = metadata_json;

    return json;
  }
};
};  // namespace ThreadMessage
