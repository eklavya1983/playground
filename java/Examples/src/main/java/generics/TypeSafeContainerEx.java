package generics;

import java.util.Hashtable;
import java.util.Map;

/**
 * Created on 2/2/15.
 */

public class TypeSafeContainerEx {
    static class Blah {
        public String toString() {return "blah";}
    }

    static abstract class Handler<T> {

        Handler(Class clazz) {
            this.clazz = clazz;
        }
        void deserialize(String s) throws IllegalAccessException, InstantiationException {
            obj = (T) clazz.newInstance();
        }
        abstract void handle();

        protected T obj;
        protected final Class clazz;
    }

    public <T> void register(Integer i, Handler<T> handler) {
        handlers.put(i, handler);
    }
    public void invokeHandler(Integer i) throws InstantiationException, IllegalAccessException {
        handlers.get(i).deserialize("blah");
        handlers.get(i).handle();
    }

    Map<Integer, Handler<?>> handlers = new Hashtable<>();

    public static void main(String args[]) throws IllegalAccessException, InstantiationException {
        TypeSafeContainerEx e = new TypeSafeContainerEx();
        e.register(0, new Handler<String>(String.class) {
            @Override
            void handle() {
                System.out.println(obj);
            }
        });
        e.register(1, new Handler<Blah>(Blah.class) {
            @Override
            void handle() {
                System.out.println(obj);
            }
        });
        e.invokeHandler(0);
        e.invokeHandler(1);
    }
}
