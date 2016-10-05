#pragma once
#include <util/Log.h>
#include <memory>
#include <thrift/lib/cpp/protocol/TJSONProtocol.h>

namespace infra {

namespace tt  = apache::thrift::transport;
namespace tp  = apache::thrift::protocol;

template<class PayloadT, class ProtocolT>
void serializeToThriftProto(const PayloadT &payload,
                            std::string &payloadBuf,
                            const std::string &logCtx)
{
    uint32_t bufSz = 512;
    std::shared_ptr<tt::TMemoryBuffer> buffer(new tt::TMemoryBuffer(bufSz));
    std::shared_ptr<tp::TProtocol> protoBuf(new ProtocolT(buffer));
    try {
        auto written = payload.write(protoBuf.get());
        CHECK(written > 0);
    } catch(std::exception &e) {
        COMMONLOG(LOG, WARNING, logCtx) << "Exception in serializing: " << e.what();
        throw;
    }
    payloadBuf = std::move(buffer->getBufferAsString());
}

template<class PayloadT, class ProtocolT>
void deserializeFromThriftProto(const std::string& payloadBuf,
                               PayloadT& payload,
                               const std::string &logCtx)
{
    std::shared_ptr<tt::TMemoryBuffer> memory_buf(
        new tt::TMemoryBuffer(reinterpret_cast<uint8_t*>(
                const_cast<char*>(payloadBuf.data())), payloadBuf.size()));
    std::shared_ptr<tp::TProtocol> protoBuf(new ProtocolT(memory_buf));

    try {
        auto read = payload.read(protoBuf.get());
        CHECK(read > 0);
    } catch(std::exception &e) {
        COMMONLOG(LOG, WARNING, logCtx) << "Exception in deserializing: " << e.what();
        throw;
    }
}

template<class PayloadT>
void serializeToThriftJson(const PayloadT &payload,
                           std::string &payloadBuf,
                           const std::string &logCtx)
{
    serializeToThriftProto<PayloadT, tp::TJSONProtocol>(payload, payloadBuf, logCtx);
}

template<class PayloadT>
void deserializeFromThriftJson(const std::string &payloadBuf,
                               PayloadT &payload,
                               const std::string &logCtx)
{
    deserializeFromThriftProto<PayloadT, tp::TJSONProtocol>(payloadBuf, payload, logCtx);
}

#if 0
template<class PayloadT>
void serializeVersionData(const int64_t &version,
                          const PayloadT &payload,
                          std::string &payloadBuf,
                          const std::string &logCtx)
{
    VersionedData data;
    data.version = version;
    serializeToThriftJson<PayloadT>(payload, data.data, logCtx);
    serializeToThriftJson<VersionedData>(data, payloadBuf, logCtx);
}
#endif

} // namespace fds
