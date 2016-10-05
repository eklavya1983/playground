#pragma once

#include <infra/gen/commontypes_types.h>
#include <infra/gen/commontypes_constants.h>
#include <folly/Conv.h>
#include <infra/Serializer.tcc>

namespace infra {

template <class T>
struct KVBinaryDataExt {
    KVBinaryDataExt(const T& data) {
        data_ = data;
    }

    KVBinaryDataExt(const KVBinaryData &kvb)
    {
        props_ = kvb.props;
        deserializeFromThriftJson<T>(kvb.data, data_, ""); 
    }

    const T& data() const {
        return data_;
    }

    T& data() {
        return data_;
    }

    template <class PropT>
    PropT getProp(const std::string &key) {
        return folly::to<PropT>(props_.at(key));
    }

    template <class PropT>
    void setProp(const std::string &key, const PropT &val) {
        props_[key] = folly::to<PropT>(val);
    }

    KVBinaryData toKVBinaryData() {
        KVBinaryData kvb;
        kvb.props = props_;
        serializeToThriftJson<T>(data_, kvb.data, "");
        return kvb;
    }

    static KVBinaryDataExt fromKVBinaryDataThriftJson(const std::string &tJson) {
        KVBinaryData kvb;
        deserializeFromThriftJson<KVBinaryData>(tJson, kvb, "");
        return KVBinaryDataExt(kvb);
    }

    std::string toKVBinaryDataThriftJson() {
        auto kvb = toKVBinaryData();
        std::string tJson;
        serializeToThriftJson<KVBinaryData>(kvb, tJson, "");
        return tJson;
    }

 private:
    T                                       data_;
    std::map<std::string, std::string>      props_;
};

template <class T>
void setProp(KVBinaryData &kvb, const std::string &key, const T &val)
{
    kvb.props[key] = folly::to<std::string>(val);
}

template <class T>
T getProp(const KVBinaryData &kvb, const std::string &key)
{
    return folly::to<T>(kvb.props.at(key));
}

inline void setVersion(KVBinaryData &kvb, int64_t version)
{
    setProp<int64_t>(kvb, g_commontypes_constants.KEY_VERSION, version);
}

inline int64_t getVersion(const KVBinaryData &kvb)
{
    return getProp<int64_t>(kvb, g_commontypes_constants.KEY_VERSION);
}

inline void setType(KVBinaryData &kvb, const std::string &type)
{
    setProp<std::string>(kvb, g_commontypes_constants.KEY_TYPE, type);
}

inline std::string getType(const KVBinaryData &kvb)
{
    return getProp<std::string>(kvb, g_commontypes_constants.KEY_TYPE);
}

inline void setId(KVBinaryData &kvb, const std::string &id)
{
    setProp<std::string>(kvb, g_commontypes_constants.KEY_ID, id);
}

inline std::string getId(const KVBinaryData &kvb)
{
    return getProp<std::string>(kvb, g_commontypes_constants.KEY_ID);
}

template <class T>
T deserializeThriftJsonData(const KVBinaryData &kvb, const std::string &logContext)
{
    T ret;
    deserializeFromThriftJson<T>(kvb.data, ret,logContext); 
}

}  // namespace infra
