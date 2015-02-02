package akka;

import akka.actor.*;
import akka.event.Logging;
import akka.event.LoggingAdapter;
import scala.Option;
import scala.concurrent.Await;
import scala.concurrent.Future;
import scala.concurrent.duration.Duration;
import scala.concurrent.duration.FiniteDuration;

/**
 * Created on 2/1/15.
 */
public class Lifecycle {
    static final class ThrowException {}

    static class ParentActor extends UntypedActor {
        LoggingAdapter log = Logging.getLogger(getContext().system(), this);

        ActorRef childRef;
        {
            childRef = getContext().actorOf(Props.create(L1Child.class), "L1Child");
        }

        @Override
        public void onReceive(Object message) throws Exception {
            if (message instanceof Exception) {
                throw (Exception) message;
            } else {
                unhandled(message);
            }
        }
        @Override
        public void preStart() throws Exception {
            log.info("preStart");
            super.preStart();
        }

        @Override
        public void postStop() throws Exception {
            log.info("postStop");
            super.postStop();
        }

        @Override
        public void preRestart(Throwable reason, Option<Object> message) throws Exception {
            log.info("preRestart reason: " + reason.getMessage());
            super.preRestart(reason, message);
        }

        @Override
        public void postRestart(Throwable reason) throws Exception {
            log.info("postRestart reason: " + reason.getMessage());
            super.postRestart(reason);
        }
    }

    static class L1Child extends UntypedActor {
        LoggingAdapter log = Logging.getLogger(getContext().system(), this);
        ActorRef childRef;
        {
            childRef = getContext().actorOf(Props.create(L2Child.class), "L2Child");
        }
        @Override
        public void onReceive(Object message) throws Exception {
            if (message instanceof Exception) {
                throw (Exception) message;
            } else {
                unhandled(message);
            }
        }

        @Override
        public void preStart() throws Exception {
            log.info("preStart");
            super.preStart();
        }

        @Override
        public void postStop() throws Exception {
            log.info("postStop");
            super.postStop();
        }

        @Override
        public void preRestart(Throwable reason, Option<Object> message) throws Exception {
            log.info("preRestart reason: " + reason.getMessage());
            super.preRestart(reason, message);
        }

        @Override
        public void postRestart(Throwable reason) throws Exception {
            log.info("postRestart reason: " + reason.getMessage());
            super.postRestart(reason);
        }
    }

    static class L2Child extends UntypedActor {
        LoggingAdapter log = Logging.getLogger(getContext().system(), this);
        @Override
        public void onReceive(Object message) throws Exception {
            if (message instanceof Exception) {
                throw (Exception) message;
            } else {
                unhandled(message);
            }
        }

        @Override
        public void preStart() throws Exception {
            log.info("preStart");
            super.preStart();
        }

        @Override
        public void postStop() throws Exception {
            log.info("postStop");
            super.postStop();
        }

        @Override
        public void preRestart(Throwable reason, Option<Object> message) throws Exception {
            log.info("preRestart reason: " + reason.getMessage());
            super.preRestart(reason, message);
        }

        @Override
        public void postRestart(Throwable reason) throws Exception {
            log.info("postRestart reason: " + reason.getMessage());
            super.postRestart(reason);
        }
    }

    public static void main(String args[]) throws Exception {
        ActorSystem system = ActorSystem.create("actor-system");
        LoggingAdapter log = Logging.getLogger(system, "main");
        ActorRef parentRef = system.actorOf(Props.create(ParentActor.class), "parent");

        Thread.sleep(1000);
        log.info("Sending an exception to parent");
        parentRef.tell(new NullPointerException("null"), ActorRef.noSender());

        Thread.sleep(1000);
        log.info("Sending an exception to L1Child");
        Future<ActorRef> l1Query = system.actorSelection("akka://actor-system/user/parent/L1Child").resolveOne((FiniteDuration) Duration.create("5 second"));
        ActorRef l1Child = Await.result(l1Query, Duration.create("5 second"));
        l1Child.tell(new NullPointerException("null"), ActorRef.noSender());

        Thread.sleep(1000);
        log.info("Sending an exception to L2Child");
        Future<ActorRef> l2Query = system.actorSelection("akka://actor-system/user/parent/L1Child/L2Child")
                .resolveOne((FiniteDuration) Duration.create("5 second"));
        ActorRef l2Child = Await.result(l2Query, Duration.create("5 second"));
        l2Child.tell(new NullPointerException("null"), ActorRef.noSender());

        Thread.sleep(1000);
        log.info("Issuing stop on parentRef");
        system.stop(parentRef);

        Thread.sleep(1000);
        log.info("Issuing shutdown actor system");
        system.shutdown();
    }
}
