#include <infra/KafkaClient.h>
#include <folly/futures/Future.h>
#include <librdkafka/rdkafkacpp.h>
#include <util/Log.h>
#include <infra/gen/status_types.h>

namespace infra {

struct KafkaEventCb : RdKafka::EventCb {
    KafkaEventCb(KafkaClient *parent)
        : parent_(parent)
    {
    }

    void event_cb (RdKafka::Event &event) 
    {
        parent_->eventCallback(event);
    }
    KafkaClient         *parent_ {nullptr};
};

KafkaClient::KafkaClient(const std::string logContext,
                         const std::string &brokers,
                         const std::string &consumerGroupId)
{
    logContext_ = logContext;
    brokers_ = brokers;
    consumerGroupId_ = consumerGroupId;
}

KafkaClient::~KafkaClient()
{
    aborted_ = true;
    if (consumeThread_) {
        consumeThread_->join();
        delete consumeThread_;
    }

    if (producer_) {
        delete producer_;
    }

    if (consumer_) {
        consumer_->close();
        delete consumer_;
    }

    if (eventCb_) delete eventCb_;

    RdKafka::wait_destroyed(5000);
}

void KafkaClient::init()
{
    /* Setup configuration */
    eventCb_ = new KafkaEventCb(this);
    std::unique_ptr<RdKafka::Conf> conf(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
    std::string errstr;
    conf->set("metadata.broker.list", brokers_, errstr);
    conf->set("event_cb", eventCb_, errstr);

    /* Initialize publisher */
    producer_ = RdKafka::Producer::create(conf.get(), errstr);
    if (!producer_) {
        CLog(ERROR) << "Unable to create producer";
        throw std::runtime_error("Unable to create producer");
    }
    CLog(INFO) << "created publisher";

    /* Initialize subscriber */
    auto status = conf->set("group.id",  consumerGroupId_, errstr);
    CHECK(status == RdKafka::Conf::CONF_OK);
    consumer_ = RdKafka::KafkaConsumer::create(conf.get(), errstr);
    if (!consumer_) {
        CLog(ERROR) << "Unable to create consumer";
        throw std::runtime_error("Unable to create consumer");
    }
    CLog(INFO) << "created subscriber";

    /* Start listening for subscribed messages */
    consumeThread_ = new std::thread([this]() {
        while (!aborted_) {
            std::unique_ptr<RdKafka::Message> msg(consumer_->consume(1000));
            consumeMessage_(msg.get(), NULL);
        }
    });
}

int KafkaClient::publishMessage(const std::string &topic,
                                const std::string &message)
{
    std::shared_ptr<RdKafka::Topic> t;
    {
        std::lock_guard<std::mutex> lock(publishTopicsLock_);
        auto itr = publishTopics_.find(topic);
        if (itr == publishTopics_.end()) {
            std::string errstr;
            t.reset(RdKafka::Topic::create(producer_, topic,
                                           nullptr, errstr));
            if (!t) {
                CLog(WARNING) << "failed to create topic: " << topic << " error:" << errstr;
                return STATUS_PUBLISH_FAILED;
            }
            publishTopics_[topic] = t;
        } else {
            t = itr->second;
        }
    }
    
    RdKafka::ErrorCode resp =
        producer_->produce(t.get(), RdKafka::Topic::PARTITION_UA,
                           RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
                           const_cast<char *>(message.c_str()), message.size(),
                           NULL, NULL);
    if (resp != RdKafka::ERR_NO_ERROR) {
        CLog(WARNING) << "failed to publish message to topic:" << topic
            << " error:" << RdKafka::err2str(resp);
        return STATUS_PUBLISH_FAILED;
    }
    return STATUS_OK;
}

int KafkaClient::subscribeToTopic(const std::string &topic, const MsgReceivedCb &cb)
{
    std::lock_guard<std::mutex> lock(subscriptionLock_);
    auto itr = subscriptionCbs_.find(topic);
    if (itr != subscriptionCbs_.end()) {
        CLog(WARNING) << "already subscribed to topic:" << topic << " ignoring request";
        return STATUS_INVALID;
    }
    subscriptionCbs_[topic] = cb;
    RdKafka::ErrorCode err = consumer_->subscribe({topic});
    if (err) {
        CLog(WARNING) << "failed to subscribe to topic:" << topic
            << " error:" << RdKafka::err2str(err);
        return STATUS_SUBSCRIBE_FAILED;
    }
    return STATUS_OK;
}

void KafkaClient::consumeMessage_(RdKafka::Message* message, void* opaque)
{
    switch (message->err()) {
        case RdKafka::ERR__TIMED_OUT:
            break;

        case RdKafka::ERR_NO_ERROR:
        {
            auto topic = message->topic_name();
            auto itr = subscriptionCbs_.find(topic);
            if (itr == subscriptionCbs_.end()) {
                CLog(WARNING) << "received message for topic:" << topic
                    << " that doesn't have registered callback.  Ignoring";
                break;
            }
            itr->second();
        }
#if 0
            /* Real message */
            msg_cnt++;
            msg_bytes += message->len();
            if (verbosity >= 3)
                std::cerr << "Read msg at offset " << message->offset() << std::endl;
            RdKafka::MessageTimestamp ts;
            ts = message->timestamp();
            if (verbosity >= 2 &&
                ts.type != RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE) {
                std::string tsname = "?";
                if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME)
                    tsname = "create time";
                else if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME)
                    tsname = "log append time";
                CLog(INFO) << "Timestamp: " << tsname << " " << ts.timestamp << std::endl;
            }
            if (verbosity >= 2 && message->key()) {
                CLog(INFO) << "Key: " << *message->key() << std::endl;
            }
            if (verbosity >= 1) {
                printf("%.*s\n",
                       static_cast<int>(message->len()),
                       static_cast<const char *>(message->payload()));
            }
#endif
            break;

        case RdKafka::ERR__PARTITION_EOF:
            /* Last message */
            break;

        case RdKafka::ERR__UNKNOWN_TOPIC:
        case RdKafka::ERR__UNKNOWN_PARTITION:
            CLog(WARNING) << "consume failed: " << message->errstr() << std::endl;
            break;

        default:
            /* Errors */
            CLog(WARNING) << "consume failed: " << message->errstr() << std::endl;
    }
}

void KafkaClient::eventCallback(RdKafka::Event &event)
{
    switch (event.type())
    {
        case RdKafka::Event::EVENT_ERROR:
            CLog(WARNING) << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
                event.str() << std::endl;
            break;

        case RdKafka::Event::EVENT_STATS:
            CLog(INFO) << "\"STATS\": " << event.str() << std::endl;
            break;

        case RdKafka::Event::EVENT_LOG:
            CLog(INFO) << "EVENT_LOG:" << event.severity()
                << "-" << event.fac() << "-" << event.str();
            break;

        case RdKafka::Event::EVENT_THROTTLE:
            CLog(INFO) << "THROTTLED: " << event.throttle_time() << "ms by " <<
                event.broker_name() << " id " << (int)event.broker_id() << std::endl;
            break;

        default:
            CLog(INFO) << "EVENT " << event.type() <<
                " (" << RdKafka::err2str(event.err()) << "): " <<
                event.str() << std::endl;
            break;
    }
}
}
