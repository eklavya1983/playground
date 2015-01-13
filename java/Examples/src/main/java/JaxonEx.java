import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

import java.awt.*;
import java.io.ByteArrayOutputStream;
import java.io.IOException;

/**
 * Created by nbayyana on 1/10/15.
 */
class Student {
    public String name;
    public int age;

    @Override
    public String toString() {
        return name + " " + age;
    }
}

public class JaxonEx {
    public static void main(String args[]) throws IOException {
        ObjectMapper mapper = new ObjectMapper();
        String jsonStr = "{\"name\" : \"Rao\", \"age\" : 31}";
        Student s = null;
        try {
            s = mapper.readValue(jsonStr, Student.class);
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println(s);

        ByteArrayOutputStream outStream = new ByteArrayOutputStream();
        Student s2 = new Student();
        s2.name = "hello";
        s2.age = 32;
        try {
            mapper.writeValue(outStream, s2);
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            System.out.println(outStream);
        }
        System.out.println("hello");

        /* Tree way */
        JsonNode jsonNode = mapper.readTree(jsonStr);
        System.out.println("Value: " + jsonNode.get("name").asText());
    }
}
