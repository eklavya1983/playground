#include <actor/Actor.h>

#include <utility>
#include <iostream>
#include <thread>

using namespace actor;

struct MyActor : NotificationQueueActor {
    using NotificationQueueActor::NotificationQueueActor;

    virtual bool handle(ActorMsg &&msg) {
        switch (msg.first.type) {
        case PUT:
            std::cout << "Received put\n";
            break;
        case GET:
            std::cout << "Received get\n";
            break;
        }
        return true;
    }
};

int main(int argc, char** argv) {
    folly::EventBase eventBase;
    std::thread t([&]() {eventBase.loopForever();});

    MyActor a(&eventBase);
    a.send({{PUT, 0}, nullptr});
    a.send({{GET, 0}, nullptr});
    
    sleep(10);
    eventBase.terminateLoopSoon();
    t.join();
    return 0;
}
