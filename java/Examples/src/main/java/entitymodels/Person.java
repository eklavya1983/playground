package entitymodels; /**
 * Created by nbayyana on 1/12/15.
 */
import java.io.Serializable;
import java.util.HashSet;
import java.util.Set;

import javax.persistence.*;

/**
 * @author Josue Junior
 *
 */
@Entity
@NamedQuery(name="Person.find", query = "SELECT p FROM Person p WHERE p.name = :name")
public class Person implements Serializable {

    private static final long serialVersionUID = -7250234396452258822L;

    @Id
    @Column(name = "id_person")
    @GeneratedValue(strategy = GenerationType.AUTO)

    private Integer id;
    private String name;
    private Integer age;
    @ElementCollection
    private Set<String> nicknames;

    public Set<String> getNicknames() {
        return nicknames;
    }

    public void setNicknames(Set<String> nicknames) {
        this.nicknames = nicknames;
    }


    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Integer getAge() {
        return age;
    }

    public void setAge(Integer age) {
        this.age = age;
    }

}
