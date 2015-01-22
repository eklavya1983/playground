package entitymodels;

import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;

/**
 * Created on 1/16/15.
 */
@Entity
public class Entry {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    public long id;
    public String text;
}
