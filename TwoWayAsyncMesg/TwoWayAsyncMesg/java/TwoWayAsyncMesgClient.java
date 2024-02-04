/*
 * Implementation of a two way messaging client in Java
 */

// Package for socket related stuff
import java.net.*;

// Package for I/O related stuff
import java.io.*;


/*
 * This class does all of two way messaging client's job
 * It simultaneously watches both keyboard and socket for input
 *
 * It consists of 2 threads: parent thread (code inside main method)
 * and child thread (code inside run method)
 *
 * Parent thread spawns a child thread and then
 * reads from the socket and writes to the screen
 *
 * Child thread reads from the keyboard and writes to socket
 *
 * Since a thread is being created with this class object,
 * this class declaration includes "implements Runnable"
 */
public class TwoWayAsyncMesgClient implements Runnable
{
	// For reading messages from the keyboard
	private BufferedReader fromUserReader;

	// For writing messages to the socket
	private PrintWriter toSockWriter;

	// Constructor sets the reader and writer for the child thread
	public TwoWayAsyncMesgClient(BufferedReader reader, PrintWriter writer)
	{
		fromUserReader = reader;
		toSockWriter = writer;
	}

	// The child thread starts here
	public void run()
	{
		// Read from the keyboard and write to socket
		try {
			// Keep doing till user types EOF (Ctrl-D)
			while (true) {
				// Read a line from the user
				String line = fromUserReader.readLine();

				// If we get null, it means EOF, so quit
				if (line == null) {
					System.out.println("*** Client closing connection");
					break;
				}

				// Write the line to the socket
				toSockWriter.println(line);
			}
		}
		catch (Exception e) {
			System.out.println(e);
			System.exit(1);
		}

		// End the other thread too
		System.exit(0);
	}

	/*
	 * The messaging client program starts from here.
	 * It sets up streams for reading & writing from keyboard and socket
	 * Spawns a thread which does the stuff under the run() method
	 * Then, it continues to read from socket and write to display
	 */
	public static void main(String args[]) {
		

		// Client needs server's contact information and user name
		if (args.length != 3) {
			System.out.println("usage: java TwoWayAsyncMesgClient <host> <port> <ClientName>");
			System.exit(1);
		}

		// Client name stored in variable


		// Connect to the server at the given host and port
		Socket sock = null;
		try {
			sock = new Socket(args[0], Integer.parseInt(args[1]));
			System.out.println(
					"Connected to server at " + args[0] + ":" + args[1]);
		}
		catch(Exception e) {
			System.out.println(e);
			System.exit(1);
		}

		// Set up a thread to read from user and write to socket
		try {
			// Prepare to write to socket with auto flush on
			PrintWriter toSockWriter =
					new PrintWriter(sock.getOutputStream(), true);

			// Prepare to read from keyboard
			BufferedReader fromUserReader = new BufferedReader(
					new InputStreamReader(System.in));

			// Spawn a thread to read from user and write to socket
			Thread child = new Thread(
					new TwoWayAsyncMesgClient(fromUserReader, toSockWriter));
			child.start();
		}
		catch(Exception e) {
			System.out.println(e);
			System.exit(1);
		}

		// Now read from socket and display to user
		try {
			// Prepare to read from socket
			BufferedReader fromSockReader = new BufferedReader(
					new InputStreamReader(sock.getInputStream()));

			// Keep doing till server is done
			while (true) {
				// Read a line from the socket
				String line = fromSockReader.readLine();

				// If we get null, it means EOF
				if (line == null) {
					// Tell user server quit
					System.out.println("*** Server closed connection");
					break;
				}

				// Write the line to the user
				System.out.println( + line);
			}
		}
		catch(Exception e) {
			System.out.println(e);
			System.exit(1);
		}

		// End the other thread too
		System.exit(0);
	}
}
