/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "ConfigApi.h"

#include "ConfigApi.tcc"

#include <thrift/lib/cpp2/protocol/Protocol.h>
#include <thrift/lib/cpp2/protocol/BinaryProtocol.h>
#include <thrift/lib/cpp2/protocol/CompactProtocol.h>
#include <thrift/lib/cpp2/protocol/SimpleJSONProtocol.h>
namespace actor { namespace cpp2 {

std::unique_ptr<apache::thrift::AsyncProcessor> ConfigApiSvIf::getProcessor() {
  return std::unique_ptr<apache::thrift::AsyncProcessor>(new ConfigApiAsyncProcessor((ConfigApiSvIf*)this));
}

apache::thrift::concurrency::PriorityThreadManager::PRIORITY ConfigApiSvIf::getprio_registerActorSystem(apache::thrift::Cpp2RequestContext* reqCtx) {
  apache::thrift::concurrency::PRIORITY prio44 = reqCtx->getCallPriority();
  if (prio44 != apache::thrift::concurrency::N_PRIORITIES) {
    return prio44;
  }
  return apache::thrift::concurrency::NORMAL;
}

void ConfigApiSvIf::registerActorSystem( ::actor::cpp2::ActorId& _return, std::unique_ptr< ::actor::cpp2::ActorInfo> info) {
  throw apache::thrift::TApplicationException("Function registerActorSystem is unimplemented");
}

folly::Future<std::unique_ptr< ::actor::cpp2::ActorId>> ConfigApiSvIf::future_registerActorSystem(std::unique_ptr< ::actor::cpp2::ActorInfo> info) {
  folly::Promise<std::unique_ptr< ::actor::cpp2::ActorId>> promise45;
  try {
    std::unique_ptr< ::actor::cpp2::ActorId> _return(new  ::actor::cpp2::ActorId);
    registerActorSystem(*_return, std::move(info));
    promise45.setValue(std::move(_return));
  } catch(const std::exception& ex) {
    promise45.setException(folly::exception_wrapper(std::current_exception()));
  }
  return promise45.getFuture();
}

void ConfigApiSvIf::async_tm_registerActorSystem(std::unique_ptr<apache::thrift::HandlerCallback<std::unique_ptr< ::actor::cpp2::ActorId>>> callback, std::unique_ptr< ::actor::cpp2::ActorInfo> info) {
  auto callbackp = callback.release();
  setEventBase(callbackp->getEventBase());
  setThreadManager(callbackp->getThreadManager());
  auto tmp_move_info46 = std::move(info);
  auto move_info47 = folly::makeMoveWrapper(std::move(tmp_move_info46));
  setConnectionContext(callbackp->getConnectionContext());
  try {
    auto future48 = future_registerActorSystem(std::move(*move_info47));
    future48.then([=](folly::Try<std::unique_ptr< ::actor::cpp2::ActorId>>&& _return) {
      try {
        callbackp->resultInThread(std::move(_return.value()));
      } catch(...) {
        callbackp->exceptionInThread(std::current_exception());
      }
    }
    );
  } catch(const std::exception& ex) {
    callbackp->exceptionInThread(std::current_exception());
  }
}

void ConfigApiSvNull::registerActorSystem( ::actor::cpp2::ActorId& _return, std::unique_ptr< ::actor::cpp2::ActorInfo> info) {}

std::string ConfigApiAsyncProcessor::getServiceName() {
  return "ConfigApi";
}

void ConfigApiAsyncProcessor::process(std::unique_ptr<apache::thrift::ResponseChannel::Request> req, std::unique_ptr<folly::IOBuf> buf, apache::thrift::protocol::PROTOCOL_TYPES protType,apache::thrift::Cpp2RequestContext* context,apache::thrift::async::TEventBase* eb, apache::thrift::concurrency::ThreadManager* tm) {
  std::string fname;
  apache::thrift::MessageType mtype;
  int32_t protoSeqId = 0;
  switch(protType) {
    case apache::thrift::protocol::T_BINARY_PROTOCOL:
    {
      std::unique_ptr<apache::thrift::BinaryProtocolReader> iprot(new apache::thrift::BinaryProtocolReader());
      iprot->setInput(buf.get());
      try {
        iprot->readMessageBegin(fname, mtype, protoSeqId);
      } catch(const apache::thrift::TException& ex) {
        LOG(ERROR) << "received invalid message from client: " << ex.what();
        apache::thrift::BinaryProtocolWriter prot;
        if (req) {
          LOG(ERROR) << "invalid message from client" << " in function process";
          apache::thrift::TApplicationException x("invalid message from client");
          folly::IOBufQueue queue = serializeException("process", &prot, protoSeqId, nullptr, x);
          queue.append(apache::thrift::transport::THeader::transform(queue.move(), context->getTransforms(), context->getMinCompressBytes()));
          auto queue_mw = folly::makeMoveWrapper(std::move(queue));
          auto req_mw = folly::makeMoveWrapper(std::move(req));
          eb->runInEventBaseThread([=]() mutable {
            (*req_mw)->sendReply(queue_mw->move());
          }
          );
          return;
        }
        else {
          LOG(ERROR) << "invalid message from client" << " in oneway function process";
        }
        return;
      }
      if (mtype != apache::thrift::T_CALL && mtype != apache::thrift::T_ONEWAY) {
        LOG(ERROR) << "received invalid message of type " << mtype;
        apache::thrift::BinaryProtocolWriter prot;
        if (req) {
          LOG(ERROR) << "invalid message arguments" << " in function process";
          apache::thrift::TApplicationException x("invalid message arguments");
          folly::IOBufQueue queue = serializeException("process", &prot, protoSeqId, nullptr, x);
          queue.append(apache::thrift::transport::THeader::transform(queue.move(), context->getTransforms(), context->getMinCompressBytes()));
          auto queue_mw = folly::makeMoveWrapper(std::move(queue));
          auto req_mw = folly::makeMoveWrapper(std::move(req));
          eb->runInEventBaseThread([=]() mutable {
            (*req_mw)->sendReply(queue_mw->move());
          }
          );
          return;
        }
        else {
          LOG(ERROR) << "invalid message arguments" << " in oneway function process";
        }
      }
      auto pfn = binaryProcessMap_.find(fname);
      if (pfn == binaryProcessMap_.end()) {
         ::actor::cpp2::ServiceApiAsyncProcessor::process(std::move(req), std::move(buf), protType, context, eb, tm);
        return;
      }
      (this->*(pfn->second))(std::move(req), std::move(buf), std::move(iprot), context, eb, tm);
      return;
    }
    case apache::thrift::protocol::T_COMPACT_PROTOCOL:
    {
      std::unique_ptr<apache::thrift::CompactProtocolReader> iprot(new apache::thrift::CompactProtocolReader());
      iprot->setInput(buf.get());
      try {
        iprot->readMessageBegin(fname, mtype, protoSeqId);
      } catch(const apache::thrift::TException& ex) {
        LOG(ERROR) << "received invalid message from client: " << ex.what();
        apache::thrift::CompactProtocolWriter prot;
        if (req) {
          LOG(ERROR) << "invalid message from client" << " in function process";
          apache::thrift::TApplicationException x("invalid message from client");
          folly::IOBufQueue queue = serializeException("process", &prot, protoSeqId, nullptr, x);
          queue.append(apache::thrift::transport::THeader::transform(queue.move(), context->getTransforms(), context->getMinCompressBytes()));
          auto queue_mw = folly::makeMoveWrapper(std::move(queue));
          auto req_mw = folly::makeMoveWrapper(std::move(req));
          eb->runInEventBaseThread([=]() mutable {
            (*req_mw)->sendReply(queue_mw->move());
          }
          );
          return;
        }
        else {
          LOG(ERROR) << "invalid message from client" << " in oneway function process";
        }
        return;
      }
      if (mtype != apache::thrift::T_CALL && mtype != apache::thrift::T_ONEWAY) {
        LOG(ERROR) << "received invalid message of type " << mtype;
        apache::thrift::CompactProtocolWriter prot;
        if (req) {
          LOG(ERROR) << "invalid message arguments" << " in function process";
          apache::thrift::TApplicationException x("invalid message arguments");
          folly::IOBufQueue queue = serializeException("process", &prot, protoSeqId, nullptr, x);
          queue.append(apache::thrift::transport::THeader::transform(queue.move(), context->getTransforms(), context->getMinCompressBytes()));
          auto queue_mw = folly::makeMoveWrapper(std::move(queue));
          auto req_mw = folly::makeMoveWrapper(std::move(req));
          eb->runInEventBaseThread([=]() mutable {
            (*req_mw)->sendReply(queue_mw->move());
          }
          );
          return;
        }
        else {
          LOG(ERROR) << "invalid message arguments" << " in oneway function process";
        }
      }
      auto pfn = compactProcessMap_.find(fname);
      if (pfn == compactProcessMap_.end()) {
         ::actor::cpp2::ServiceApiAsyncProcessor::process(std::move(req), std::move(buf), protType, context, eb, tm);
        return;
      }
      (this->*(pfn->second))(std::move(req), std::move(buf), std::move(iprot), context, eb, tm);
      return;
    }
    case apache::thrift::protocol::T_SIMPLE_JSON_PROTOCOL:
    {
      std::unique_ptr<apache::thrift::SimpleJSONProtocolReader> iprot(new apache::thrift::SimpleJSONProtocolReader());
      iprot->setInput(buf.get());
      try {
        iprot->readMessageBegin(fname, mtype, protoSeqId);
      } catch(const apache::thrift::TException& ex) {
        LOG(ERROR) << "received invalid message from client: " << ex.what();
        apache::thrift::SimpleJSONProtocolWriter prot;
        if (req) {
          LOG(ERROR) << "invalid message from client" << " in function process";
          apache::thrift::TApplicationException x("invalid message from client");
          folly::IOBufQueue queue = serializeException("process", &prot, protoSeqId, nullptr, x);
          queue.append(apache::thrift::transport::THeader::transform(queue.move(), context->getTransforms(), context->getMinCompressBytes()));
          auto queue_mw = folly::makeMoveWrapper(std::move(queue));
          auto req_mw = folly::makeMoveWrapper(std::move(req));
          eb->runInEventBaseThread([=]() mutable {
            (*req_mw)->sendReply(queue_mw->move());
          }
          );
          return;
        }
        else {
          LOG(ERROR) << "invalid message from client" << " in oneway function process";
        }
        return;
      }
      if (mtype != apache::thrift::T_CALL && mtype != apache::thrift::T_ONEWAY) {
        LOG(ERROR) << "received invalid message of type " << mtype;
        apache::thrift::SimpleJSONProtocolWriter prot;
        if (req) {
          LOG(ERROR) << "invalid message arguments" << " in function process";
          apache::thrift::TApplicationException x("invalid message arguments");
          folly::IOBufQueue queue = serializeException("process", &prot, protoSeqId, nullptr, x);
          queue.append(apache::thrift::transport::THeader::transform(queue.move(), context->getTransforms(), context->getMinCompressBytes()));
          auto queue_mw = folly::makeMoveWrapper(std::move(queue));
          auto req_mw = folly::makeMoveWrapper(std::move(req));
          eb->runInEventBaseThread([=]() mutable {
            (*req_mw)->sendReply(queue_mw->move());
          }
          );
          return;
        }
        else {
          LOG(ERROR) << "invalid message arguments" << " in oneway function process";
        }
      }
      auto pfn = simple_jsonProcessMap_.find(fname);
      if (pfn == simple_jsonProcessMap_.end()) {
         ::actor::cpp2::ServiceApiAsyncProcessor::process(std::move(req), std::move(buf), protType, context, eb, tm);
        return;
      }
      (this->*(pfn->second))(std::move(req), std::move(buf), std::move(iprot), context, eb, tm);
      return;
    }
    default:
    {
      LOG(ERROR) << "invalid protType: " << protType;
      return;
      break;
    }
  }
}

bool ConfigApiAsyncProcessor::isOnewayMethod(const folly::IOBuf* buf, const apache::thrift::transport::THeader* header) {
  std::string fname;
  apache::thrift::MessageType mtype;
  int32_t protoSeqId = 0;
  apache::thrift::protocol::PROTOCOL_TYPES protType = static_cast<apache::thrift::protocol::PROTOCOL_TYPES>(header->getProtocolId());
  switch(protType) {
    case apache::thrift::protocol::T_BINARY_PROTOCOL:
    {
      apache::thrift::BinaryProtocolReader iprot;
      iprot.setInput(buf);
      try {
        iprot.readMessageBegin(fname, mtype, protoSeqId);
        auto it = onewayMethods.find(fname);
        return it != onewayMethods.end();
      } catch(const apache::thrift::TException& ex) {
        LOG(ERROR) << "received invalid message from client: " << ex.what();
        return false;
      }
    }
    case apache::thrift::protocol::T_COMPACT_PROTOCOL:
    {
      apache::thrift::CompactProtocolReader iprot;
      iprot.setInput(buf);
      try {
        iprot.readMessageBegin(fname, mtype, protoSeqId);
        auto it = onewayMethods.find(fname);
        return it != onewayMethods.end();
      } catch(const apache::thrift::TException& ex) {
        LOG(ERROR) << "received invalid message from client: " << ex.what();
        return false;
      }
    }
    case apache::thrift::protocol::T_SIMPLE_JSON_PROTOCOL:
    {
      apache::thrift::SimpleJSONProtocolReader iprot;
      iprot.setInput(buf);
      try {
        iprot.readMessageBegin(fname, mtype, protoSeqId);
        auto it = onewayMethods.find(fname);
        return it != onewayMethods.end();
      } catch(const apache::thrift::TException& ex) {
        LOG(ERROR) << "received invalid message from client: " << ex.what();
        return false;
      }
    }
    default:
    {
      LOG(ERROR) << "invalid protType: " << protType;
      break;
    }
  }
  return false;
}

std::unordered_set<std::string> ConfigApiAsyncProcessor::onewayMethods {};
ConfigApiAsyncProcessor::binaryProcessMap ConfigApiAsyncProcessor::binaryProcessMap_ {
  {"registerActorSystem", &ConfigApiAsyncProcessor::_processInThread_registerActorSystem<apache::thrift::BinaryProtocolReader, apache::thrift::BinaryProtocolWriter>}
};
ConfigApiAsyncProcessor::compactProcessMap ConfigApiAsyncProcessor::compactProcessMap_ {
  {"registerActorSystem", &ConfigApiAsyncProcessor::_processInThread_registerActorSystem<apache::thrift::CompactProtocolReader, apache::thrift::CompactProtocolWriter>}
};
ConfigApiAsyncProcessor::simple_jsonProcessMap ConfigApiAsyncProcessor::simple_jsonProcessMap_ {
  {"registerActorSystem", &ConfigApiAsyncProcessor::_processInThread_registerActorSystem<apache::thrift::SimpleJSONProtocolReader, apache::thrift::SimpleJSONProtocolWriter>}
};

}} // actor::cpp2
namespace apache { namespace thrift {

}} // apache::thrift