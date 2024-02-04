/*
 * Implementation of a two way messaging server in Java
 */

// I/O related package
import java.io.*;

// Socket related package
import java.net.*;

/*
 * This class does all of two way messaging server's job
 * It simultaneously watches both keyboard and socket for input
 *
 * It consists of 2 threads: parent thread (code inside init method)
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
public class TwoWayAsyncMesgServer implements Runnable
{
	// For reading messages from the keyboard
	private BufferedReader fromUserReader;

	// For writing messages to the socket
	private PrintWriter toSockWriter;

	// Constructor sets the reader and writer for the child thread
	public TwoWayAsyncMesgServer(BufferedReader reader, PrintWriter writer)
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
					System.out.println("*** Server closing connection");
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
	 * The messaging server program starts from here.
	 * It sets up streams for reading & writing from keyboard and socket
	 * Spawns a thread which does the stuff under the run() method
	 * Then, it continues to read from socket and write to display
	 */
	public static void main(String args[])
	{
		// Server needs a port to listen on
		if (args.length != 1) {
			System.out.println("usage: java TwoWayAsyncMesgServer <port>");
			System.exit(1);
		}

		// Get the port on which server should listen */
		int serverPort = Integer.parseInt(args[0]);

		// Be prepared to catch socket related exceptions
		Socket clientSock = null;
		try {
			// Create a server socket with the given port
			ServerSocket serverSocket = new ServerSocket(serverPort);

			// Wait for a client and accept it
			System.out.println("Waiting for a client ...");
			clientSock = serverSocket.accept();
			System.out.println("Connected to a client at ('" +
									((InetSocketAddress) clientSock.getRemoteSocketAddress()).getAddress().getHostAddress()
									+ "', '" +
									((InetSocketAddress) clientSock.getRemoteSocketAddress()).getPort()
									+ "')"
									);
		}
		catch(Exception e) {
			System.out.println(e);
			System.exit(1);
		}

		// Set up a thread to read from user and send to client
		try {
			// Prepare to write to socket with auto flush on
			PrintWriter toSockWriter =
					new PrintWriter(clientSock.getOutputStream(), true);

			// Prepare to read from keyboard
			BufferedReader fromUserReader = new BufferedReader(
					new InputStreamReader(System.in));

			// Spawn a thread to read from user and write to socket
			Thread child = new Thread(
					new TwoWayAsyncMesgServer(fromUserReader, toSockWriter));
			child.start();
		}
		catch(Exception e) {
			System.out.println(e);
			System.exit(1);
		}

		// Now parent thread reads from client and display to user
		try {
			// Prepare to read from socket
			BufferedReader fromSockReader = new BufferedReader(
					new InputStreamReader(clientSock.getInputStream()));

			// Keep doing till server is done
			while (true) {
				// Read a line from the socket
				String line = fromSockReader.readLine();

				// If we get null, it means EOF
				if (line == null) {
					// Tell user client quit
					System.out.println("*** Client closed connection");
					break;
				}

				// Write the line to the user
				System.out.println("Client: " + line);
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
