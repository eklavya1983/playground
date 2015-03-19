package entitymodels;

import javax.persistence.*;
import java.util.List;

/**
 * Created on 1/16/15.
 */
@Entity
@NamedQuery(name="Tag.find", query = "SELECT t FROM Tag t WHERE t.name = :name")
public class Tag {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    public int id;
    public String name;
    @OneToMany
    public List<Entry> entries;
}
