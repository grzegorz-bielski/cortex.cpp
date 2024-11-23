#pragma once

#include <drogon/HttpController.h>
#include <trantor/utils/Logger.h>

using namespace drogon;

class Messages : public drogon::HttpController<Messages, false> {
 public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(Messages::CreateMessage, "/v1/threads/{1}/messages", Options,
                Post);

  ADD_METHOD_TO(Messages::ListMessages, "/v1/threads/{1}/messages", Get);
  ADD_METHOD_TO(Messages::RetrieveMessage, "/v1/threads/{1}/messages/{2}", Get);
  ADD_METHOD_TO(Messages::ModifyMessage, "/v1/threads/{1}/messages/{2}",
                Options, Post);
  ADD_METHOD_TO(Messages::DeleteMessage, "/v1/threads/{1}/messages/{2}",
                Options, Delete);
  METHOD_LIST_END

  explicit Messages() {}

  void CreateMessage(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback,
                     const std::string& thread_id);

  void ListMessages(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback,
                    const std::string& thread_id, std::optional<uint8_t> limit,
                    std::optional<std::string> order,
                    std::optional<std::string> after,
                    std::optional<std::string> before,
                    std::optional<std::string> run_id) const;

  void RetrieveMessage(const HttpRequestPtr& req,
                       std::function<void(const HttpResponsePtr&)>&& callback,
                       const std::string& thread_id,
                       const std::string& message_id) const;

  void ModifyMessage(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback,
                     const std::string& thread_id,
                     const std::string& message_id);

  void DeleteMessage(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback,
                     const std::string& thread_id,
                     const std::string& message_id);
};
