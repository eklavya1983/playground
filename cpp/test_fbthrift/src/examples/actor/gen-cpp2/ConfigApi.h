/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#pragma once

#include <thrift/lib/cpp2/ServiceIncludes.h>
#include <thrift/lib/cpp/TApplicationException.h>
#include <thrift/lib/cpp2/async/FutureRequest.h>
#include <folly/futures/Future.h>
#include "Service_types.h"
#include <thrift/lib/cpp2/protocol/BinaryProtocol.h>
#include <thrift/lib/cpp2/protocol/CompactProtocol.h>
#include <thrift/lib/cpp2/protocol/SimpleJSONProtocol.h>


#include "ServiceApi.h"

namespace actor { namespace cpp2 {

class ConfigApiSvAsyncIf {
 public:
  virtual ~ConfigApiSvAsyncIf() {}
  virtual void async_tm_registerActorSystem(std::unique_ptr<apache::thrift::HandlerCallback<std::unique_ptr< ::actor::cpp2::ActorId>>> callback, std::unique_ptr< ::actor::cpp2::ActorInfo> info) = 0;
  virtual void async_registerActorSystem(std::unique_ptr<apache::thrift::HandlerCallback<std::unique_ptr< ::actor::cpp2::ActorId>>> callback, std::unique_ptr< ::actor::cpp2::ActorInfo> info) = delete;
  virtual folly::Future<std::unique_ptr< ::actor::cpp2::ActorId>> future_registerActorSystem(std::unique_ptr< ::actor::cpp2::ActorInfo> info) = 0;
};

class ConfigApiAsyncProcessor;

class ConfigApiSvIf : public ConfigApiSvAsyncIf, virtual public  ::actor::cpp2::ServiceApiSvIf {
 public:
  typedef ConfigApiAsyncProcessor ProcessorType;

  virtual ~ConfigApiSvIf() {}
  virtual std::unique_ptr<apache::thrift::AsyncProcessor> getProcessor();
  apache::thrift::concurrency::PriorityThreadManager::PRIORITY getprio_registerActorSystem(apache::thrift::Cpp2RequestContext* reqCtx);
  virtual void registerActorSystem( ::actor::cpp2::ActorId& _return, std::unique_ptr< ::actor::cpp2::ActorInfo> info);
  folly::Future<std::unique_ptr< ::actor::cpp2::ActorId>> future_registerActorSystem(std::unique_ptr< ::actor::cpp2::ActorInfo> info);
  virtual void async_tm_registerActorSystem(std::unique_ptr<apache::thrift::HandlerCallback<std::unique_ptr< ::actor::cpp2::ActorId>>> callback, std::unique_ptr< ::actor::cpp2::ActorInfo> info);
};

class ConfigApiSvNull : public ConfigApiSvIf, virtual public  ::actor::cpp2::ServiceApiSvIf {
 public:
  virtual ~ConfigApiSvNull() {}
  virtual void registerActorSystem( ::actor::cpp2::ActorId& _return, std::unique_ptr< ::actor::cpp2::ActorInfo> info);
};

class ConfigApiAsyncProcessor : public  ::actor::cpp2::ServiceApiAsyncProcessor {
 public:
  virtual std::string getServiceName();
 protected:
  ConfigApiSvIf* iface_;
  virtual void process(std::unique_ptr<apache::thrift::ResponseChannel::Request> req, std::unique_ptr<folly::IOBuf> buf, apache::thrift::protocol::PROTOCOL_TYPES protType,apache::thrift::Cpp2RequestContext* context,apache::thrift::async::TEventBase* eb, apache::thrift::concurrency::ThreadManager* tm);
  virtual bool isOnewayMethod(const folly::IOBuf* buf, const apache::thrift::transport::THeader* header);
 private:
  static std::unordered_set<std::string> onewayMethods;
  typedef void (ConfigApiAsyncProcessor::*binaryProcessFunction)(std::unique_ptr<apache::thrift::ResponseChannel::Request> req, std::unique_ptr<folly::IOBuf> buf, std::unique_ptr<apache::thrift::BinaryProtocolReader> iprot, apache::thrift::Cpp2RequestContext* context, apache::thrift::async::TEventBase* eb, apache::thrift::concurrency::ThreadManager* tm);
  typedef std::unordered_map<std::string, binaryProcessFunction> binaryProcessMap;
  static ConfigApiAsyncProcessor::binaryProcessMap binaryProcessMap_;
  typedef void (ConfigApiAsyncProcessor::*compactProcessFunction)(std::unique_ptr<apache::thrift::ResponseChannel::Request> req, std::unique_ptr<folly::IOBuf> buf, std::unique_ptr<apache::thrift::CompactProtocolReader> iprot, apache::thrift::Cpp2RequestContext* context, apache::thrift::async::TEventBase* eb, apache::thrift::concurrency::ThreadManager* tm);
  typedef std::unordered_map<std::string, compactProcessFunction> compactProcessMap;
  static ConfigApiAsyncProcessor::compactProcessMap compactProcessMap_;
  typedef void (ConfigApiAsyncProcessor::*simple_jsonProcessFunction)(std::unique_ptr<apache::thrift::ResponseChannel::Request> req, std::unique_ptr<folly::IOBuf> buf, std::unique_ptr<apache::thrift::SimpleJSONProtocolReader> iprot, apache::thrift::Cpp2RequestContext* context, apache::thrift::async::TEventBase* eb, apache::thrift::concurrency::ThreadManager* tm);
  typedef std::unordered_map<std::string, simple_jsonProcessFunction> simple_jsonProcessMap;
  static ConfigApiAsyncProcessor::simple_jsonProcessMap simple_jsonProcessMap_;
  template <typename ProtocolIn_, typename ProtocolOut_>
  void _processInThread_registerActorSystem(std::unique_ptr<apache::thrift::ResponseChannel::Request> req, std::unique_ptr<folly::IOBuf> buf, std::unique_ptr<ProtocolIn_> iprot, apache::thrift::Cpp2RequestContext* ctx, apache::thrift::async::TEventBase* eb, apache::thrift::concurrency::ThreadManager* tm);
  template <typename ProtocolIn_, typename ProtocolOut_>
  void process_registerActorSystem(std::unique_ptr<apache::thrift::ResponseChannel::Request> req, std::unique_ptr<folly::IOBuf> buf, std::unique_ptr<ProtocolIn_> iprot,apache::thrift::Cpp2RequestContext* ctx,apache::thrift::async::TEventBase* eb, apache::thrift::concurrency::ThreadManager* tm);
  template <class ProtocolIn_, class ProtocolOut_>
  static folly::IOBufQueue return_registerActorSystem(int32_t protoSeqId, std::unique_ptr<apache::thrift::ContextStack> ctx,  ::actor::cpp2::ActorId const& _return);
  template <class ProtocolIn_, class ProtocolOut_>
  static void throw_registerActorSystem(std::unique_ptr<apache::thrift::ResponseChannel::Request> req,int32_t protoSeqId,std::unique_ptr<apache::thrift::ContextStack> ctx,std::exception_ptr ep,apache::thrift::Cpp2RequestContext* reqCtx);
  template <class ProtocolIn_, class ProtocolOut_>
  static void throw_wrapped_registerActorSystem(std::unique_ptr<apache::thrift::ResponseChannel::Request> req,int32_t protoSeqId,std::unique_ptr<apache::thrift::ContextStack> ctx,folly::exception_wrapper ew,apache::thrift::Cpp2RequestContext* reqCtx);
 public:
  ConfigApiAsyncProcessor(ConfigApiSvIf* iface) :
       ::actor::cpp2::ServiceApiAsyncProcessor(iface),
      iface_(iface) {}

  virtual ~ConfigApiAsyncProcessor() {}
};

class ConfigApiAsyncClient : public  ::actor::cpp2::ServiceApiAsyncClient {
 public:
  virtual std::string getServiceName();
  typedef std::unique_ptr<apache::thrift::RequestChannel, apache::thrift::async::TDelayedDestruction::Destructor> channel_ptr;

  virtual ~ConfigApiAsyncClient() {}

  ConfigApiAsyncClient(std::shared_ptr<apache::thrift::RequestChannel> channel) :
       ::actor::cpp2::ServiceApiAsyncClient(channel) {
    connectionContext_ = std::unique_ptr<TClientBase::ConnContext>(new TClientBase::ConnContext(channel_->getHeader(),nullptr));
  }
  virtual void registerActorSystem(std::unique_ptr<apache::thrift::RequestCallback> callback, const  ::actor::cpp2::ActorInfo& info);
  virtual void callback_registerActorSystem(std::unique_ptr<apache::thrift::RequestCallback> callback, const  ::actor::cpp2::ActorInfo& info);
  virtual void registerActorSystem(const apache::thrift::RpcOptions& rpcOptions, std::unique_ptr<apache::thrift::RequestCallback> callback, const  ::actor::cpp2::ActorInfo& info);
  virtual void sync_registerActorSystem( ::actor::cpp2::ActorId& _return, const  ::actor::cpp2::ActorInfo& info);
  virtual void sync_registerActorSystem(const apache::thrift::RpcOptions& rpcOptions,  ::actor::cpp2::ActorId& _return, const  ::actor::cpp2::ActorInfo& info);
  virtual void registerActorSystem(std::function<void (::apache::thrift::ClientReceiveState&&)> callback, const  ::actor::cpp2::ActorInfo& info);
  virtual void functor_registerActorSystem(std::function<void (::apache::thrift::ClientReceiveState&&)> callback, const  ::actor::cpp2::ActorInfo& info);
  virtual folly::Future< ::actor::cpp2::ActorId> future_registerActorSystem(const  ::actor::cpp2::ActorInfo& info);
  virtual folly::Future< ::actor::cpp2::ActorId> future_registerActorSystem(const apache::thrift::RpcOptions& rpcOptions, const  ::actor::cpp2::ActorInfo& info);
  static folly::exception_wrapper recv_wrapped_registerActorSystem( ::actor::cpp2::ActorId& _return, ::apache::thrift::ClientReceiveState& state);
  static void recv_registerActorSystem( ::actor::cpp2::ActorId& _return, ::apache::thrift::ClientReceiveState& state);
  // Mock friendly virtual instance method
  virtual void recv_instance_registerActorSystem( ::actor::cpp2::ActorId& _return, ::apache::thrift::ClientReceiveState& state);
  virtual folly::exception_wrapper recv_instance_wrapped_registerActorSystem( ::actor::cpp2::ActorId& _return, ::apache::thrift::ClientReceiveState& state);
  template <typename Protocol_>
  void registerActorSystemT(Protocol_* prot, const apache::thrift::RpcOptions& rpcOptions, std::unique_ptr<apache::thrift::RequestCallback> callback, const  ::actor::cpp2::ActorInfo& info);
  template <typename Protocol_>
  static folly::exception_wrapper recv_wrapped_registerActorSystemT(Protocol_* prot,  ::actor::cpp2::ActorId& _return, ::apache::thrift::ClientReceiveState& state);
  template <typename Protocol_>
  static void recv_registerActorSystemT(Protocol_* prot,  ::actor::cpp2::ActorId& _return, ::apache::thrift::ClientReceiveState& state);
};

class ConfigApi_registerActorSystem_pargs : private boost::totally_ordered<ConfigApi_registerActorSystem_pargs> {
 public:
  virtual ~ConfigApi_registerActorSystem_pargs() throw() {}

   ::actor::cpp2::ActorInfo* info;

  struct __isset {
    __isset() {
      __clear();
    }

    void __clear() {
      info = false;
    }

    bool info;
  } __isset;

  template <class Protocol_>
  uint32_t read(Protocol_* iprot);
  template <class Protocol_>
  uint32_t serializedSize(Protocol_* prot_) const;
  template <class Protocol_>
  uint32_t serializedSizeZC(Protocol_* prot_) const;
  template <class Protocol_>
  uint32_t write(Protocol_* prot_) const;
};

class ConfigApi_registerActorSystem_presult : private boost::totally_ordered<ConfigApi_registerActorSystem_presult> {
 public:
  virtual ~ConfigApi_registerActorSystem_presult() throw() {}

   ::actor::cpp2::ActorId* success;
   ::actor::cpp2::RegisterException e;

  struct __isset {
    __isset() {
      __clear();
    }

    void __clear() {
      success = false;
      e = false;
    }

    bool success;
    bool e;
  } __isset;

  template <class Protocol_>
  uint32_t read(Protocol_* iprot);
  template <class Protocol_>
  uint32_t serializedSize(Protocol_* prot_) const;
  template <class Protocol_>
  uint32_t serializedSizeZC(Protocol_* prot_) const;
  template <class Protocol_>
  uint32_t write(Protocol_* prot_) const;
};

}} // actor::cpp2
namespace apache { namespace thrift {

template <> template <class Protocol> inline uint32_t Cpp2Ops< ::actor::cpp2::ConfigApi_registerActorSystem_pargs>::write(Protocol* proto, const  ::actor::cpp2::ConfigApi_registerActorSystem_pargs* obj) {
  return obj->write(proto);
}

template <> template <class Protocol> inline uint32_t Cpp2Ops< ::actor::cpp2::ConfigApi_registerActorSystem_pargs>::read(Protocol* proto,   ::actor::cpp2::ConfigApi_registerActorSystem_pargs* obj) {
  return obj->read(proto);
}

template <> template <class Protocol> inline uint32_t Cpp2Ops< ::actor::cpp2::ConfigApi_registerActorSystem_pargs>::serializedSize(Protocol* proto, const  ::actor::cpp2::ConfigApi_registerActorSystem_pargs* obj) {
  return obj->serializedSize(proto);
}

template <> template <class Protocol> inline uint32_t Cpp2Ops< ::actor::cpp2::ConfigApi_registerActorSystem_pargs>::serializedSizeZC(Protocol* proto, const  ::actor::cpp2::ConfigApi_registerActorSystem_pargs* obj) {
  return obj->serializedSizeZC(proto);
}

template <> template <class Protocol> inline uint32_t Cpp2Ops< ::actor::cpp2::ConfigApi_registerActorSystem_presult>::write(Protocol* proto, const  ::actor::cpp2::ConfigApi_registerActorSystem_presult* obj) {
  return obj->write(proto);
}

template <> template <class Protocol> inline uint32_t Cpp2Ops< ::actor::cpp2::ConfigApi_registerActorSystem_presult>::read(Protocol* proto,   ::actor::cpp2::ConfigApi_registerActorSystem_presult* obj) {
  return obj->read(proto);
}

template <> template <class Protocol> inline uint32_t Cpp2Ops< ::actor::cpp2::ConfigApi_registerActorSystem_presult>::serializedSize(Protocol* proto, const  ::actor::cpp2::ConfigApi_registerActorSystem_presult* obj) {
  return obj->serializedSize(proto);
}

template <> template <class Protocol> inline uint32_t Cpp2Ops< ::actor::cpp2::ConfigApi_registerActorSystem_presult>::serializedSizeZC(Protocol* proto, const  ::actor::cpp2::ConfigApi_registerActorSystem_presult* obj) {
  return obj->serializedSizeZC(proto);
}

}} // apache::thrift