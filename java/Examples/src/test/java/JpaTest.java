/**
 * Created by nbayyana on 1/12/15.
 */
import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;
import javax.persistence.TypedQuery;


import entitymodels.*;
import org.junit.*;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;


/**
 *
 */
public class JpaTest {

    static EntityManagerFactory emf;
    EntityManager em;

    @BeforeClass
    public static void setUp() {
        emf = Persistence.createEntityManagerFactory("pu-sqlite-jpa");
    }

    @Before
    public void initEntityManager() {
        em = emf.createEntityManager();
    }

    @Test
    public void testPersist() {
        try {

            em.getTransaction().begin();

            //Persist in database
            Person person = new Person();
            Set<String> nicknames = new HashSet<String>();
            nicknames.add("bunny");
            nicknames.add("funny");

            person.setName("person2");
            person.setNicknames(nicknames);
            em.persist(person);
            em.getTransaction().commit();

            //Find by id
            Person personDB = em.find(Person.class, person.getId());
            System.out.println("Name: " + personDB.getName());

            Assert.assertEquals(person.getName(), personDB.getName());

            /* Find by typed query */
            TypedQuery<Person> query = em.createNamedQuery("Person.find", Person.class);
            query.setParameter("name", "person2");
            List<Person> resultList = query.getResultList();
            Assert.assertEquals(resultList.size(), 1);
            personDB = resultList.get(0);
            Assert.assertEquals(person.getName(), personDB.getName());
            Assert.assertEquals(person.getNicknames(), personDB.getNicknames());

        } catch (Throwable e) {
            e.printStackTrace();
            Assert.fail();
        }
    }

    @Ignore
    @Test
    public void testUpdate() {
        try {

            em.getTransaction().begin();

            //Find by id in database
            Person person = em.find(Person.class, 10); //See file import.sql
            person.setAge(22);

            //Persist in database
            em.merge(person);
            em.getTransaction().commit();

            //Find by id
            Person personDB = em.find(Person.class, 10);
            Assert.assertEquals(person.getAge(), personDB.getAge());

        } catch (Throwable e) {
            e.printStackTrace();
            Assert.fail();
        }
    }

    @Test
    public void testOneToMany() {
        try {
            em.getTransaction().begin();

            Entry e1 = new Entry();
            e1.text = "entry text1";

            Entry e2 = new Entry();
            e2.text = "entry text2";

            Entry e3 = new Entry();
            e3.text = "entry text3";

            Tag t = new Tag();
            t.name = "Tag1";
            t.entries = new ArrayList();
            t.entries.add(e1);
            t.entries.add(e2);
            t.entries.add(e3);

            em.persist(e1);
            em.persist(e2);
            em.persist(e3);

            em.persist(t);
            em.getTransaction().commit();

            Tag dbTag = (Tag) em.find(Tag.class, t.id);
            Assert.assertTrue(dbTag.entries.equals(t.entries));
        } catch (Exception e) {
            e.printStackTrace();
            Assert.fail();
        }
    }

    @Test
    public void testTagAdd() {
        /**
         * 1. Have a name query to check if tag exists with tag name
         * 2. If tag doesn't exist create it
         * 3. Add entry
         * 4. Add update the tag
         */
        try {
            String tagToLook = "testTagAdd";
            String entryText = "testTagAdd:entry";
            TypedQuery<Tag> query = em.createNamedQuery("Tag.find", Tag.class);
            query.setParameter("name", tagToLook);
            List<Tag> resultList = query.getResultList();
            Assert.assertEquals(resultList.size(), 0);

            Tag tag = new Tag();
            tag.name = tagToLook;
            em.getTransaction().begin();
            em.persist(tag);
            em.getTransaction().commit();

            Entry e1 = new Entry();
            e1.text = entryText + "1";
            Entry e2 = new Entry();
            e2.text = entryText + "2";
            List<Tag> resultList1 = query.getResultList();
            Assert.assertEquals(resultList1.size(), 1);
            Tag dbTag = resultList1.get(0);
            Assert.assertEquals(tag.id, dbTag.id);

            dbTag.entries = new ArrayList();
            dbTag.entries.add(e1);
            dbTag.entries.add(e2);

            em.getTransaction().begin();
            em.persist(e1);
            em.persist(e2);
            em.persist(dbTag);
            em.getTransaction().commit();

            dbTag = query.getSingleResult();
            Assert.assertEquals(dbTag.entries.size(), 2);

        } catch (Exception e) {
            e.printStackTrace();
            Assert.fail();
        }
    }

    @Test
    public void testManyToMany() {
        ArrayList<Course> courses = new ArrayList<Course>();
        ArrayList<Student> students = new ArrayList<Student>();

        Student s1 = new Student();
        s1.name = "s1";
        Student s2 = new Student();
        s2.name = "s2";
        Student s3 = new Student();
        s3.name = "s3";
        Student s4 = new Student();
        s4.name = "s4";

        em.getTransaction().begin();
        em.persist(s1);
        em.persist(s2);
        em.persist(s3);
        em.persist(s4);
        em.getTransaction().commit();

        Course c1 = new Course();
        c1.name = "course1";
        c1.students = new ArrayList<Student>();
        c1.students.add(s1);
        c1.students.add(s2);

        Course c2 = new Course();
        c2.name = "course2";
        c2.students = new ArrayList<Student>();
        c2.students.add(s3);
        c2.students.add(s4);

        em.getTransaction().begin();
        em.persist(c1);
        em.persist(c2);
        em.getTransaction().commit();

        TypedQuery<Student> query = em.createNamedQuery("Student.find", Student.class);
        query.setParameter("name", "s1");
        List<Student> resultList = query.getResultList();
        Assert.assertEquals(resultList.size(), 1);
    }

//    @Test
//    public void testFindById(){
//        try {
//
//            //Find by id in database
//            Integer personId = 10;
//            entitymodels.Person person = em.find(entitymodels.Person.class, personId); //See file import.sql
//
//            Assert.assertEquals(personId, person.getId());
//
//        } catch (Throwable e) {
//            e.printStackTrace();
//            Assert.fail();
//        }
//    }
//
//    @Test
//    public void testRemove(){
//        try {
//
//            //Find by id in database and remove
//            Integer personId = 10;
//            entitymodels.Person person = em.find(entitymodels.Person.class, personId); //See file import.sql
//            em.remove(person);
//            em.getTransaction().commit();
//
//            //Find by id
//            entitymodels.Person personDB = em.find(entitymodels.Person.class, personId); //See file import.sql
//
//            Assert.assertNull(personDB);
//
//        } catch (Throwable e) {
//            e.printStackTrace();
//            Assert.fail();
//        }
//    }

    @After
    public void closeEntityManager() {
        em.close();
    }

    @AfterClass
    public static void closeEntityManagerFactory() {
        emf.close();
    }

}
