package akka;

import akka.actor.*;
import akka.event.Logging;
import akka.event.LoggingAdapter;

/**
 * Created on 2/1/15.
 */

public class ChildWatch {
    static class StopChild {}
    static class ParentWork {}
    static class ChildWork {}

    static class ParentActor extends UntypedActor {
        LoggingAdapter log = Logging.getLogger(getContext().system(), this);

        ActorRef childRef;
        {
            childRef = getContext().actorOf(Props.create(ChildActor.class), "child");
            getContext().watch(childRef);
        }

        @Override
        public void onReceive(Object message) throws Exception {
            if (message instanceof StopChild) {
                log.info("Issue stop child");
                getContext().stop(childRef);
            } else if (message instanceof Terminated) {
                log.info("Child terminated");
            } else if (message instanceof ParentWork) {
                log.info("excuting parent work");
            } else if (message instanceof ChildWork) {
                log.info("forwarding child work");
                childRef.forward(message, getContext());
            } else {
                    unhandled(message);
            }

        }
    }

    static class ChildActor extends UntypedActor {
        LoggingAdapter log = Logging.getLogger(getContext().system(), this);

        @Override
        public void onReceive(Object message) throws Exception {
            if (message instanceof ChildWork) {
                log.info("executing child work");
            } else {
                unhandled(message);
            }
        }
    }

    public static void main(String args[]) throws InterruptedException {
        ActorSystem actorSystem = ActorSystem.create("actor-system");
        ActorRef parentRef = actorSystem.actorOf(Props.create(ParentActor.class), "parent");
        parentRef.tell(new ParentWork(), null);
        parentRef.tell(new ChildWork(), null);
        parentRef.tell(new StopChild(), null);

        Thread.sleep(1000);
        actorSystem.shutdown();

    }
}
