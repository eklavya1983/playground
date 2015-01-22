import java.util.Arrays;
import java.util.List;
import java.util.Optional;

/**
 * Created by on 1/14/15.
 */
public class CollectionsEx {
    public static void main(String args[]) {
        List<String> strings = Arrays.asList("Rao", "bayyana", "hello");
        Optional<String> reduce = strings.stream()
                .map(s -> s.toLowerCase())
                .reduce((s1, s2) -> s1.length() > s2.length() ? s1 : s2);
        reduce.ifPresent(s -> System.out.println(s));

        System.out.println(String.join(",", strings));
    }
}
