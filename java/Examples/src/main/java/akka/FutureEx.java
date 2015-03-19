package akka;

import akka.actor.ActorSystem;
import akka.dispatch.Futures;
import akka.dispatch.Mapper;
import akka.dispatch.OnSuccess;
import akka.event.Logging;
import akka.event.LoggingAdapter;
import scala.concurrent.Await;
import scala.concurrent.Future;
import scala.concurrent.Promise;
import scala.concurrent.duration.Duration;
import scala.util.Success;

import java.util.concurrent.Callable;

/**
 * Created on 2/2/15.
 */
public class FutureEx {
    static ActorSystem system = ActorSystem.create("actor-system");
    static LoggingAdapter logger = Logging.getLogger(system, "main");

    public final static class PrintResult<T> extends OnSuccess<T> {
        @Override
        public void onSuccess(T result) throws Throwable {
            logger.info(result.toString());
        }
    }

    public void successFuture() {
        Future<String> hello = Futures.successful("hello");
        hello.onSuccess(new OnSuccess<String>() {
            @Override
            public void onSuccess(String result) throws Throwable {
                logger.info("result is " + result);
            }
        }, system.dispatcher());
    }

    public void directFuture() throws Exception {
        Future<String> f = Futures.future(new Callable<String>() {
            @Override
            public String call() throws Exception {
                return "hello";
            }
        }, system.dispatcher());

        String result = Await.result(f, Duration.create("1 second"));
        logger.info("result is " + result);
    }

    public void mapFuture() throws InterruptedException {
        Future<String> f1 = Futures.future(()-> "1234", system.dispatcher());
        Thread.sleep(10);
        f1.onSuccess(new PrintResult<String>(), system.dispatcher());
        Future<Integer> intf1 = f1.map(new Mapper<String, Integer>() {
            @Override
            public Integer apply(String v1) {
                return Integer.valueOf(v1);
            }
        }, system.dispatcher());

        intf1.onSuccess(new PrintResult<Integer>(), system.dispatcher());
    }

    public void flatmapFuture() throws InterruptedException {
        Future<String> f1 = Futures.future(()-> "4568", system.dispatcher());
        Thread.sleep(10);
        f1.onSuccess(new PrintResult<String>(), system.dispatcher());
        Future<Integer> intf1 = f1.flatMap(new Mapper<String, Future<Integer>>() {
            @Override
            public Future<Integer> apply(String parameter) {
                return Futures.future(new Callable<Integer>() {
                    @Override
                    public Integer call() throws Exception {
                        return Integer.valueOf(parameter);
                    }
                }, system.dispatcher());
            }
        }, system.dispatcher());

        intf1.onSuccess(new PrintResult<Integer>(), system.dispatcher());
    }

    public void simplePromise() {
        Promise<String> p = Futures.promise();
        Future<String> future = p.future();
        p.complete(new Success<String>("simple promise"));
        future.onSuccess(new PrintResult<String>(), system.dispatcher());
    }

    public static void main(String args[]) throws Exception {
        FutureEx ex = new FutureEx();
        ex.directFuture();
        ex.successFuture();
        ex.mapFuture();
        ex.flatmapFuture();
        ex.simplePromise();

        Thread.sleep(2000);
        system.shutdown();

//        final ExecutionContext ec = ex.system.dispatcher();
//
//        Future<String> f = Futures.future(()->"hello", ec);
//        f.onSuccess(new OnSuccess<String>() {
//            @Override
//            public void onSuccess(String result) throws Throwable {
//                System.out.println("f succeeded");
//            }
//        }, ec);
//        f.onSuccess(proc(s -> {
//            System.out.println(s);
//            return;
//        }), ec);
//
//        Future<String> f1 = Futures.future(new Callable<String>() {
//            public String call() {
//                return "Hello" + "World";
//            }
//        }, ec);
//
//        Future<Integer> f2 = f1.map(new Mapper<String, Integer>() {
//            public Integer apply(String s) {
//                return s.length();
//            }
//        }, ec);
//
//        f2.onSuccess(new PrintResult<Integer>(), system.dispatcher());
    }
}
