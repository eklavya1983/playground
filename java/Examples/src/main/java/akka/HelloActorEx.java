package akka;

/**
 * Created on 1/31/15.
 */
import akka.actor.ActorRef;
import akka.actor.ActorSystem;
import akka.actor.Props;
import akka.actor.UntypedActor;

/**
 * Created on 1/31/15.
 */
public class HelloActorEx {
    private static class HelloActor extends UntypedActor {

        @Override
        public void onReceive(Object message) throws Exception {
            if (message instanceof String) {
                System.out.println("Hello " + (String) message);
            } else {
                unhandled("Unknown message");
            }
        }
    }

    public static void main(String args[]) throws InterruptedException {
        ActorSystem actorSystem = ActorSystem.create("om-actor-system");
        ActorRef actorRef = actorSystem.actorOf(Props.create(HelloActor.class), "hello-actor");
        actorRef.tell("Rao", null);

        Thread.sleep(10);
        actorSystem.shutdown();
    }

}
