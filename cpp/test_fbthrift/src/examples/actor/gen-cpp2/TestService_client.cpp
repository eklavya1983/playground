/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "TestService.h"

#include "TestService.tcc"

#include <thrift/lib/cpp2/protocol/DebugProtocol.h>
#include <thrift/lib/cpp2/protocol/VirtualProtocol.h>

namespace actor { namespace cpp2 {

std::string TestServiceAsyncClient::getServiceName() {
  return "TestService";
}

void TestServiceAsyncClient::reqResp(std::unique_ptr<apache::thrift::RequestCallback> callback, const std::string& req) {
  reqResp(::apache::thrift::RpcOptions(), std::move(callback), req);
}

void TestServiceAsyncClient::callback_reqResp(std::unique_ptr<apache::thrift::RequestCallback> callback, const std::string& req) {
  reqResp(::apache::thrift::RpcOptions(), std::move(callback), req);
}

void TestServiceAsyncClient::reqResp(const apache::thrift::RpcOptions& rpcOptions, std::unique_ptr<apache::thrift::RequestCallback> callback, const std::string& req) {
  switch(getChannel()->getProtocolId()) {
    case apache::thrift::protocol::T_BINARY_PROTOCOL:
    {
      apache::thrift::BinaryProtocolWriter writer;
      reqRespT(&writer, rpcOptions, std::move(callback), req);
      break;
    }
    case apache::thrift::protocol::T_COMPACT_PROTOCOL:
    {
      apache::thrift::CompactProtocolWriter writer;
      reqRespT(&writer, rpcOptions, std::move(callback), req);
      break;
    }
    case apache::thrift::protocol::T_SIMPLE_JSON_PROTOCOL:
    {
      apache::thrift::SimpleJSONProtocolWriter writer;
      reqRespT(&writer, rpcOptions, std::move(callback), req);
      break;
    }
    default:
    {
      throw apache::thrift::TApplicationException("Could not find Protocol");
    }
  }
}

void TestServiceAsyncClient::sync_reqResp(std::string& _return, const std::string& req) {
  sync_reqResp(::apache::thrift::RpcOptions(), _return, req);
}

void TestServiceAsyncClient::sync_reqResp(const apache::thrift::RpcOptions& rpcOptions, std::string& _return, const std::string& req) {
  apache::thrift::ClientReceiveState _returnState;
  std::unique_ptr<apache::thrift::RequestCallback> callback69(new apache::thrift::ClientSyncCallback(&_returnState, getChannel()->getEventBase(), false));
  reqResp(rpcOptions, std::move(callback69), req);
  getChannel()->getEventBase()->loopForever();
  if (!_returnState.buf()) {
    assert(_returnState.exception());
    std::rethrow_exception(_returnState.exception());
  }
  recv_reqResp(_return, _returnState);
}

void TestServiceAsyncClient::reqResp(std::function<void (::apache::thrift::ClientReceiveState&&)> callback, const std::string& req) {
  reqResp(std::unique_ptr<apache::thrift::RequestCallback>(new apache::thrift::FunctionReplyCallback(std::move(callback))),req);
}

void TestServiceAsyncClient::functor_reqResp(std::function<void (::apache::thrift::ClientReceiveState&&)> callback, const std::string& req) {
  reqResp(std::unique_ptr<apache::thrift::RequestCallback>(new apache::thrift::FunctionReplyCallback(std::move(callback))),req);
}

folly::Future<std::string> TestServiceAsyncClient::future_reqResp(const std::string& req) {
  return future_reqResp(::apache::thrift::RpcOptions(), req);
}

folly::Future<std::string> TestServiceAsyncClient::future_reqResp(const apache::thrift::RpcOptions& rpcOptions, const std::string& req) {
  folly::Promise<std::string> promise70;
  auto future71 = promise70.getFuture();
  std::unique_ptr<apache::thrift::RequestCallback> callback72(new apache::thrift::FutureCallback<std::string>(std::move(promise70), recv_wrapped_reqResp));
  reqResp(rpcOptions, std::move(callback72), req);
  return std::move(future71);
}

folly::exception_wrapper TestServiceAsyncClient::recv_wrapped_reqResp(std::string& _return, ::apache::thrift::ClientReceiveState& state) {
  auto ew = state.exceptionWrapper();
  if (ew) {
    return ew;
  }
  if (!state.buf()) {
    return folly::make_exception_wrapper<apache::thrift::TApplicationException>("recv_ called without result");
  }
  switch(state.protocolId()) {
    case apache::thrift::protocol::T_BINARY_PROTOCOL:
    {
      apache::thrift::BinaryProtocolReader reader;
      return recv_wrapped_reqRespT(&reader, _return, state);
    }
    case apache::thrift::protocol::T_COMPACT_PROTOCOL:
    {
      apache::thrift::CompactProtocolReader reader;
      return recv_wrapped_reqRespT(&reader, _return, state);
    }
    case apache::thrift::protocol::T_SIMPLE_JSON_PROTOCOL:
    {
      apache::thrift::SimpleJSONProtocolReader reader;
      return recv_wrapped_reqRespT(&reader, _return, state);
    }
    default:
    {
    }
  }
  return folly::make_exception_wrapper<apache::thrift::TApplicationException>("Could not find Protocol");
}

void TestServiceAsyncClient::recv_reqResp(std::string& _return, ::apache::thrift::ClientReceiveState& state) {
  auto ew = recv_wrapped_reqResp(_return, state);
  if (ew) {
    ew.throwException();
  }
}

void TestServiceAsyncClient::recv_instance_reqResp(std::string& _return, ::apache::thrift::ClientReceiveState& state) {
  return recv_reqResp(_return, state);
}

folly::exception_wrapper TestServiceAsyncClient::recv_instance_wrapped_reqResp(std::string& _return, ::apache::thrift::ClientReceiveState& state) {
  return recv_wrapped_reqResp(_return, state);
}

void TestServiceAsyncClient::asyncReq(std::unique_ptr<apache::thrift::RequestCallback> callback, int64_t id, const std::string& payload) {
  asyncReq(::apache::thrift::RpcOptions(), std::move(callback), id, payload);
}

void TestServiceAsyncClient::callback_asyncReq(std::unique_ptr<apache::thrift::RequestCallback> callback, int64_t id, const std::string& payload) {
  asyncReq(::apache::thrift::RpcOptions(), std::move(callback), id, payload);
}

void TestServiceAsyncClient::asyncReq(const apache::thrift::RpcOptions& rpcOptions, std::unique_ptr<apache::thrift::RequestCallback> callback, int64_t id, const std::string& payload) {
  switch(getChannel()->getProtocolId()) {
    case apache::thrift::protocol::T_BINARY_PROTOCOL:
    {
      apache::thrift::BinaryProtocolWriter writer;
      asyncReqT(&writer, rpcOptions, std::move(callback), id, payload);
      break;
    }
    case apache::thrift::protocol::T_COMPACT_PROTOCOL:
    {
      apache::thrift::CompactProtocolWriter writer;
      asyncReqT(&writer, rpcOptions, std::move(callback), id, payload);
      break;
    }
    case apache::thrift::protocol::T_SIMPLE_JSON_PROTOCOL:
    {
      apache::thrift::SimpleJSONProtocolWriter writer;
      asyncReqT(&writer, rpcOptions, std::move(callback), id, payload);
      break;
    }
    default:
    {
      throw apache::thrift::TApplicationException("Could not find Protocol");
    }
  }
}

void TestServiceAsyncClient::sync_asyncReq(int64_t id, const std::string& payload) {
  sync_asyncReq(::apache::thrift::RpcOptions(), id, payload);
}

void TestServiceAsyncClient::sync_asyncReq(const apache::thrift::RpcOptions& rpcOptions, int64_t id, const std::string& payload) {
  apache::thrift::ClientReceiveState _returnState;
  std::unique_ptr<apache::thrift::RequestCallback> callback74(new apache::thrift::ClientSyncCallback(&_returnState, getChannel()->getEventBase(), true));
  asyncReq(rpcOptions, std::move(callback74), id, payload);
  getChannel()->getEventBase()->loopForever();
}

void TestServiceAsyncClient::asyncReq(std::function<void (::apache::thrift::ClientReceiveState&&)> callback, int64_t id, const std::string& payload) {
  asyncReq(std::unique_ptr<apache::thrift::RequestCallback>(new apache::thrift::FunctionReplyCallback(std::move(callback))),id,payload);
}

void TestServiceAsyncClient::functor_asyncReq(std::function<void (::apache::thrift::ClientReceiveState&&)> callback, int64_t id, const std::string& payload) {
  asyncReq(std::unique_ptr<apache::thrift::RequestCallback>(new apache::thrift::FunctionReplyCallback(std::move(callback))),id,payload);
}

folly::Future<void> TestServiceAsyncClient::future_asyncReq(int64_t id, const std::string& payload) {
  return future_asyncReq(::apache::thrift::RpcOptions(), id, payload);
}

folly::Future<void> TestServiceAsyncClient::future_asyncReq(const apache::thrift::RpcOptions& rpcOptions, int64_t id, const std::string& payload) {
  folly::Promise<void> promise75;
  auto future76 = promise75.getFuture();
  std::unique_ptr<apache::thrift::RequestCallback> callback77(new apache::thrift::FutureCallback<void>(std::move(promise75), true));
  asyncReq(rpcOptions, std::move(callback77), id, payload);
  return std::move(future76);
}

void TestServiceAsyncClient::asyncResp(std::unique_ptr<apache::thrift::RequestCallback> callback, int64_t id, const std::string& payload) {
  asyncResp(::apache::thrift::RpcOptions(), std::move(callback), id, payload);
}

void TestServiceAsyncClient::callback_asyncResp(std::unique_ptr<apache::thrift::RequestCallback> callback, int64_t id, const std::string& payload) {
  asyncResp(::apache::thrift::RpcOptions(), std::move(callback), id, payload);
}

void TestServiceAsyncClient::asyncResp(const apache::thrift::RpcOptions& rpcOptions, std::unique_ptr<apache::thrift::RequestCallback> callback, int64_t id, const std::string& payload) {
  switch(getChannel()->getProtocolId()) {
    case apache::thrift::protocol::T_BINARY_PROTOCOL:
    {
      apache::thrift::BinaryProtocolWriter writer;
      asyncRespT(&writer, rpcOptions, std::move(callback), id, payload);
      break;
    }
    case apache::thrift::protocol::T_COMPACT_PROTOCOL:
    {
      apache::thrift::CompactProtocolWriter writer;
      asyncRespT(&writer, rpcOptions, std::move(callback), id, payload);
      break;
    }
    case apache::thrift::protocol::T_SIMPLE_JSON_PROTOCOL:
    {
      apache::thrift::SimpleJSONProtocolWriter writer;
      asyncRespT(&writer, rpcOptions, std::move(callback), id, payload);
      break;
    }
    default:
    {
      throw apache::thrift::TApplicationException("Could not find Protocol");
    }
  }
}

void TestServiceAsyncClient::sync_asyncResp(int64_t id, const std::string& payload) {
  sync_asyncResp(::apache::thrift::RpcOptions(), id, payload);
}

void TestServiceAsyncClient::sync_asyncResp(const apache::thrift::RpcOptions& rpcOptions, int64_t id, const std::string& payload) {
  apache::thrift::ClientReceiveState _returnState;
  std::unique_ptr<apache::thrift::RequestCallback> callback79(new apache::thrift::ClientSyncCallback(&_returnState, getChannel()->getEventBase(), true));
  asyncResp(rpcOptions, std::move(callback79), id, payload);
  getChannel()->getEventBase()->loopForever();
}

void TestServiceAsyncClient::asyncResp(std::function<void (::apache::thrift::ClientReceiveState&&)> callback, int64_t id, const std::string& payload) {
  asyncResp(std::unique_ptr<apache::thrift::RequestCallback>(new apache::thrift::FunctionReplyCallback(std::move(callback))),id,payload);
}

void TestServiceAsyncClient::functor_asyncResp(std::function<void (::apache::thrift::ClientReceiveState&&)> callback, int64_t id, const std::string& payload) {
  asyncResp(std::unique_ptr<apache::thrift::RequestCallback>(new apache::thrift::FunctionReplyCallback(std::move(callback))),id,payload);
}

folly::Future<void> TestServiceAsyncClient::future_asyncResp(int64_t id, const std::string& payload) {
  return future_asyncResp(::apache::thrift::RpcOptions(), id, payload);
}

folly::Future<void> TestServiceAsyncClient::future_asyncResp(const apache::thrift::RpcOptions& rpcOptions, int64_t id, const std::string& payload) {
  folly::Promise<void> promise80;
  auto future81 = promise80.getFuture();
  std::unique_ptr<apache::thrift::RequestCallback> callback82(new apache::thrift::FutureCallback<void>(std::move(promise80), true));
  asyncResp(rpcOptions, std::move(callback82), id, payload);
  return std::move(future81);
}

template uint32_t TestService_reqResp_pargs::read<apache::thrift::BinaryProtocolReader>(apache::thrift::BinaryProtocolReader*);
template uint32_t TestService_reqResp_pargs::write<apache::thrift::BinaryProtocolWriter>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t TestService_reqResp_pargs::serializedSize<apache::thrift::BinaryProtocolWriter>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t TestService_reqResp_pargs::serializedSizeZC<apache::thrift::BinaryProtocolWriter>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t TestService_reqResp_pargs::read<apache::thrift::CompactProtocolReader>(apache::thrift::CompactProtocolReader*);
template uint32_t TestService_reqResp_pargs::write<apache::thrift::CompactProtocolWriter>(apache::thrift::CompactProtocolWriter*) const;
template uint32_t TestService_reqResp_pargs::serializedSize<apache::thrift::CompactProtocolWriter>(apache::thrift::CompactProtocolWriter*) const;
template uint32_t TestService_reqResp_pargs::serializedSizeZC<apache::thrift::CompactProtocolWriter>(apache::thrift::CompactProtocolWriter*) const;
template uint32_t TestService_reqResp_pargs::read<apache::thrift::SimpleJSONProtocolReader>(apache::thrift::SimpleJSONProtocolReader*);
template uint32_t TestService_reqResp_pargs::write<apache::thrift::SimpleJSONProtocolWriter>(apache::thrift::SimpleJSONProtocolWriter*) const;
template uint32_t TestService_reqResp_pargs::serializedSize<apache::thrift::SimpleJSONProtocolWriter>(apache::thrift::SimpleJSONProtocolWriter*) const;
template uint32_t TestService_reqResp_pargs::serializedSizeZC<apache::thrift::SimpleJSONProtocolWriter>(apache::thrift::SimpleJSONProtocolWriter*) const;
template uint32_t TestService_reqResp_pargs::write<apache::thrift::DebugProtocolWriter>(apache::thrift::DebugProtocolWriter*) const;
template uint32_t TestService_reqResp_pargs::read<apache::thrift::VirtualReaderBase>(apache::thrift::VirtualReaderBase*);
template uint32_t TestService_reqResp_presult::read<apache::thrift::BinaryProtocolReader>(apache::thrift::BinaryProtocolReader*);
template uint32_t TestService_reqResp_presult::write<apache::thrift::BinaryProtocolWriter>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t TestService_reqResp_presult::serializedSize<apache::thrift::BinaryProtocolWriter>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t TestService_reqResp_presult::serializedSizeZC<apache::thrift::BinaryProtocolWriter>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t TestService_reqResp_presult::read<apache::thrift::CompactProtocolReader>(apache::thrift::CompactProtocolReader*);
template uint32_t TestService_reqResp_presult::write<apache::thrift::CompactProtocolWriter>(apache::thrift::CompactProtocolWriter*) const;
template uint32_t TestService_reqResp_presult::serializedSize<apache::thrift::CompactProtocolWriter>(apache::thrift::CompactProtocolWriter*) const;
template uint32_t TestService_reqResp_presult::serializedSizeZC<apache::thrift::CompactProtocolWriter>(apache::thrift::CompactProtocolWriter*) const;
template uint32_t TestService_reqResp_presult::read<apache::thrift::SimpleJSONProtocolReader>(apache::thrift::SimpleJSONProtocolReader*);
template uint32_t TestService_reqResp_presult::write<apache::thrift::SimpleJSONProtocolWriter>(apache::thrift::SimpleJSONProtocolWriter*) const;
template uint32_t TestService_reqResp_presult::serializedSize<apache::thrift::SimpleJSONProtocolWriter>(apache::thrift::SimpleJSONProtocolWriter*) const;
template uint32_t TestService_reqResp_presult::serializedSizeZC<apache::thrift::SimpleJSONProtocolWriter>(apache::thrift::SimpleJSONProtocolWriter*) const;
template uint32_t TestService_reqResp_presult::write<apache::thrift::DebugProtocolWriter>(apache::thrift::DebugProtocolWriter*) const;
template uint32_t TestService_reqResp_presult::read<apache::thrift::VirtualReaderBase>(apache::thrift::VirtualReaderBase*);
template uint32_t TestService_asyncReq_pargs::read<apache::thrift::BinaryProtocolReader>(apache::thrift::BinaryProtocolReader*);
template uint32_t TestService_asyncReq_pargs::write<apache::thrift::BinaryProtocolWriter>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t TestService_asyncReq_pargs::serializedSize<apache::thrift::BinaryProtocolWriter>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t TestService_asyncReq_pargs::serializedSizeZC<apache::thrift::BinaryProtocolWriter>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t TestService_asyncReq_pargs::read<apache::thrift::CompactProtocolReader>(apache::thrift::CompactProtocolReader*);
template uint32_t TestService_asyncReq_pargs::write<apache::thrift::CompactProtocolWriter>(apache::thrift::CompactProtocolWriter*) const;
template uint32_t TestService_asyncReq_pargs::serializedSize<apache::thrift::CompactProtocolWriter>(apache::thrift::CompactProtocolWriter*) const;
template uint32_t TestService_asyncReq_pargs::serializedSizeZC<apache::thrift::CompactProtocolWriter>(apache::thrift::CompactProtocolWriter*) const;
template uint32_t TestService_asyncReq_pargs::read<apache::thrift::SimpleJSONProtocolReader>(apache::thrift::SimpleJSONProtocolReader*);
template uint32_t TestService_asyncReq_pargs::write<apache::thrift::SimpleJSONProtocolWriter>(apache::thrift::SimpleJSONProtocolWriter*) const;
template uint32_t TestService_asyncReq_pargs::serializedSize<apache::thrift::SimpleJSONProtocolWriter>(apache::thrift::SimpleJSONProtocolWriter*) const;
template uint32_t TestService_asyncReq_pargs::serializedSizeZC<apache::thrift::SimpleJSONProtocolWriter>(apache::thrift::SimpleJSONProtocolWriter*) const;
template uint32_t TestService_asyncReq_pargs::write<apache::thrift::DebugProtocolWriter>(apache::thrift::DebugProtocolWriter*) const;
template uint32_t TestService_asyncReq_pargs::read<apache::thrift::VirtualReaderBase>(apache::thrift::VirtualReaderBase*);
template uint32_t TestService_asyncResp_pargs::read<apache::thrift::BinaryProtocolReader>(apache::thrift::BinaryProtocolReader*);
template uint32_t TestService_asyncResp_pargs::write<apache::thrift::BinaryProtocolWriter>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t TestService_asyncResp_pargs::serializedSize<apache::thrift::BinaryProtocolWriter>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t TestService_asyncResp_pargs::serializedSizeZC<apache::thrift::BinaryProtocolWriter>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t TestService_asyncResp_pargs::read<apache::thrift::CompactProtocolReader>(apache::thrift::CompactProtocolReader*);
template uint32_t TestService_asyncResp_pargs::write<apache::thrift::CompactProtocolWriter>(apache::thrift::CompactProtocolWriter*) const;
template uint32_t TestService_asyncResp_pargs::serializedSize<apache::thrift::CompactProtocolWriter>(apache::thrift::CompactProtocolWriter*) const;
template uint32_t TestService_asyncResp_pargs::serializedSizeZC<apache::thrift::CompactProtocolWriter>(apache::thrift::CompactProtocolWriter*) const;
template uint32_t TestService_asyncResp_pargs::read<apache::thrift::SimpleJSONProtocolReader>(apache::thrift::SimpleJSONProtocolReader*);
template uint32_t TestService_asyncResp_pargs::write<apache::thrift::SimpleJSONProtocolWriter>(apache::thrift::SimpleJSONProtocolWriter*) const;
template uint32_t TestService_asyncResp_pargs::serializedSize<apache::thrift::SimpleJSONProtocolWriter>(apache::thrift::SimpleJSONProtocolWriter*) const;
template uint32_t TestService_asyncResp_pargs::serializedSizeZC<apache::thrift::SimpleJSONProtocolWriter>(apache::thrift::SimpleJSONProtocolWriter*) const;
template uint32_t TestService_asyncResp_pargs::write<apache::thrift::DebugProtocolWriter>(apache::thrift::DebugProtocolWriter*) const;
template uint32_t TestService_asyncResp_pargs::read<apache::thrift::VirtualReaderBase>(apache::thrift::VirtualReaderBase*);
}} // actor::cpp2
namespace apache { namespace thrift {

}} // apache::thrift