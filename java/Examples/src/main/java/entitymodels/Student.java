package entitymodels;

import javax.persistence.*;
import java.util.List;

/**
 * Created on 1/16/15.
 */
@Entity
@NamedQuery(name="Student.find", query = "SELECT s FROM Student s WHERE s.name = :name")
public class Student {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    public int id;
    public String name;
    @ManyToMany(cascade = CascadeType.ALL, mappedBy = "students")
    public List<Course> courses;
}
