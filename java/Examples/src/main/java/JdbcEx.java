/**
 * Created by eklavya1983 on 1/14/15.
 */
// TODO(eklavya1983): Uncomment and fix
public class JdbcEx {
//    private static final String TABLE_CREATE =
//            "CREATE TABLE ENTRIES " +
//                    "(id INTEGER PRIMARY KEY AUTOINCREMENT,  " +
//                    "userid VARCHAR(255),  " +
//                    "timestamp LONG, " +
//                    "location VARCHAR(255), " +
//                    "type VARCHAR(255), " +
//                    "uri VARCHAR(255), " +
//                    "text VARCHAR(255))";
//
//    private static final String ENTRY_INSERT_TEMPL =
//            "INSERT INTO ENTRIES (userid, timestamp, location, type, uri, text) " +
//                    "VALUES (?, ?, ?, ?, ?, ?)";
//    public static Result createTable() {
//        Logger.info("Creating table..");
//        Connection connection = DB.getConnection();
//        Statement st = null;
//        try {
//            st = connection.createStatement();
//            st.executeUpdate(TABLE_CREATE);
//        } catch (SQLException e) {
//            e.printStackTrace();
//        } finally {
//            if (st != null) {
//                try {
//                    st.close();
//                } catch (SQLException e) {
//                    e.printStackTrace();
//                }
//            }
//            try {
//                connection.close();
//            } catch (SQLException e) {
//                e.printStackTrace();
//            }
//        }
//        return Results.ok();
//    }
//
//    public static Result insertEntry() {
//        /* Extract data from the schema */
//        long entryId = -1;
//        ObjectNode objNode = (ObjectNode) request().body().asJson();
//        String userid = objNode.get("userid").asText();
//        long timestamp = objNode.get("timestamp").asLong();
//        String location = objNode.get("location").asText();
//        String type = objNode.get("type").asText();
//        String uri = objNode.get("uri").asText();
//        String text = objNode.get("text").asText();
//
//        /* Update database */
//        Connection connection = DB.getConnection();
//        PreparedStatement st = null;
//        try {
//            st = connection.prepareStatement(ENTRY_INSERT_TEMPL, new String[] {"id"});
//            st.setString(1, userid);
//            st.setLong(2, timestamp);
//            st.setString(3, location);
//            st.setString(4, type);
//            st.setString(5, uri);
//            st.setString(6, text);
//            int res = st.executeUpdate();
//            if (res > 0) {
//                ResultSet genKeys = st.getGeneratedKeys();
//                if (genKeys != null && genKeys.next()){
//                    entryId = genKeys.getLong(1);
//                    Logger.info("Generated id: " + entryId);
//                }
//            }
//        } catch (SQLException e) {
//            e.printStackTrace();
//        } finally {
//            if (st != null) {
//                try {
//                    st.close();
//                } catch (SQLException e) {
//                    e.printStackTrace();
//                }
//            }
//            try {
//                connection.close();
//            } catch (SQLException e) {
//                e.printStackTrace();
//            }
//        }
//        if (entryId >= 0) {
//            return Results.ok(new LongNode(entryId));
//        }
//        return Results.internalServerError();
//    }

}
