package generics;

import java.util.Hashtable;
import java.util.Map;

/**
 * Created on 2/2/15.
 */

public class TypeSafeContainerEx2 {
    static class Blah {
        public String toString() {return "blah";}
    }

    static abstract class Handler {

        Handler(Class clazz) {
            this.clazz = clazz;
        }
        public <T> T deserialize(String s) throws IllegalAccessException, InstantiationException {
            T obj = (T) clazz.newInstance();
            return obj;
        }
        public abstract <T> void handle(T obj);

        protected final Class clazz;
    }

    public <T> void register(Integer i, Handler handler) {
        handlers.put(i, handler);
    }
    public void invokeHandler(Integer i) throws InstantiationException, IllegalAccessException {
        handlers.get(i).handle(handlers.get(i).deserialize("blah"));
    }

    Map<Integer, Handler> handlers = new Hashtable<>();

    public static void main(String args[]) throws IllegalAccessException, InstantiationException {
        TypeSafeContainerEx2 e = new TypeSafeContainerEx2();
        e.register(0, new Handler(String.class) {
            @Override
            public <String> void handle(String obj) {
                System.out.println(obj);
            }
        });
        e.register(1, new Handler(Blah.class) {
            @Override
            public <Blah> void handle(Blah obj) {
               System.out.println(obj);
            }
        });
        e.invokeHandler(0);
        e.invokeHandler(1);
    }
}
