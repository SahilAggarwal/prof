import java.sql.*;
import java.util.Calendar;
public class DBTest {
	
	private static final String url = "jdbc:mysql://localhost/EMP";
	private static final String user = "root";
	private static final String password = "root";

	public static void main(String args[]) {
		long start = Calendar.getInstance().getTimeInMillis();
		long end;
		Statement stmt = null;
		try {
			Class.forName("com.mysql.jdbc.Driver");
			Connection con = DriverManager.getConnection(url, user, password);
			System.out.println("Connection Success");

			stmt = con.createStatement();
	
			String query = "DELETE from EMPLOYEE";

			stmt.executeUpdate(query);

			System.out.println("Table empty now");

			int age;
			int salary;
			for(int i = 1; i < 100; i++) {
				age = i*10;
				salary = i*100 + 500;
				
				query = "INSERT INTO EMPLOYEE " +
					"VALUES ("+ i + "," + age + ",'sahil'," + salary+")";

				stmt.executeUpdate(query);
			}
			end = Calendar.getInstance().getTimeInMillis();

			System.out.println("Records added");
			System.out.printf("Time Taken: %d ms\n",end-start);
	 
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
