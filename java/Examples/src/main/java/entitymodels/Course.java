package entitymodels;

import javax.persistence.*;
import java.util.List;

/**
 * Created on 1/16/15.
 */
@Entity
public class Course {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    public long id;
    public String name;
    @ManyToMany(cascade = CascadeType.ALL)
    @JoinTable(name="COURSE_STUDENT")
    public List<Student> students;
}
