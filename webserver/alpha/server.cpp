#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main()
{
    const int SERVER_PORT = 80;

    // socket takes a domain (address family), a type, and a protocol.
    //
    // The address family defines the types of addresses to be processed.
    // Members of AF_INET are IPv4 addresses.
    // Members of AF_INET6 are IPv6 addresses.
    // Members of AF_UNIX are Unit domain socket names like /var/run/mysqld/mysqld.sock
    // AF_UNIX is for local communication(?)
    // Members of AF_IPX are IPX addresses and so on.
    //
    // The socket type determines the type of the connection. This is not necessarily
    // the protocol. This just defines the general behavior of the socket.
    // SOCK_STREAM is a reliable, connection based stream.
    // SOCK_DGRAM is an unreliable method.
    //
    // The protocol defines the specific protocol to use. Some socket types can use
    // different protocols like SOCK_RAW may use ICMP or IP etc.
    // It can be left as 0 because there are default values for common setups:
    // AF_INET + SOCK_STREAM = default protocol is TCP.
    // AF_INET + SOCK_DGRAM = default protocol is UDP.
    //
    // All address families can be found from /usr/include/bits/socket.h
    // All socket types can be found from /usr/include/bits/socket_type.h
    // Protocol definitions can be found from /usr/include/netinet/in.h
    int server_sock_filedesc = socket(AF_INET, SOCK_STREAM, 0);

    // represents and Internet socket address. Defined in netinet/in.h
    sockaddr_in server_sock_address;

    // why call bzero here?
    
    server_sock_address.sin_family = AF_INET;

    // htons converts a short uint value from host to network byte order (Host To Network Short)
    // There's also htonl, ntohs, ntohl
    // This is because of endianness. Linux on x86 is little endian but when you send
    // data across a network it should be big endian.
    server_sock_address.sin_port = htons(SERVER_PORT);

    // the socket address is contained within an in_addr struct.
    // INADDR_ANY is used to later bind the socket to all local network interfaces.
    // https://man7.org/linux/man-pages/man7/ip.7.html
    server_sock_address.sin_addr.s_addr = INADDR_ANY;

    // A socket is not bound to any address by default.
    int bind_status = bind(server_sock_filedesc, (sockaddr*) &server_sock_address, sizeof(server_sock_address));

    if (bind_status != 0)
    {
        cerr << "Bind error: " << strerror(errno) << endl;
    }

    // make the socket listen for connections. 5 clients can join a queue
    // before we start rejecting connections(?)
    listen(server_sock_filedesc, 5);

    sockaddr_in client_sock_address;
    socklen_t client_sock_addr_len = sizeof(client_sock_address);

    // accept blocks until a client connects and then creates a new socket with the same
    // properties as the one it is given (the server socket in this case).
    int client_sock_filedesc = accept(server_sock_filedesc, (sockaddr*) &client_sock_address, &client_sock_addr_len);

    if (client_sock_filedesc < 0)
    {
        cerr << "Accept error: " << strerror(errno) << endl;
    }

    char buffer[256];
    // recv also blocks until there is something to read. This means the client has to
    // write something for this to continue.
    int read_status = recv(client_sock_filedesc, &buffer, sizeof(buffer), 0);

    if (read_status < 0)
    {
        cerr << "Read error: " << strerror(errno) << endl;
    }

    cout << "Received (len: " << strlen(buffer) << "): \n" << buffer << endl;

    string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
    response.append(to_string(response.size()));
    response.append("\n\n<html><h1>Hello world</h1></html>");

    cout << "Sending: \n" << response << endl;

    int bytes_sent = 0;
    while (bytes_sent < response.size())
    {
        int write_status = send(client_sock_filedesc, response.c_str(), response.size(), 0);

        if (write_status < 0)
        {
            cerr << "Write error: " << strerror(errno) << endl;
        }

        bytes_sent += write_status;
    }

    shutdown(client_sock_filedesc, SHUT_RDWR);
    shutdown(server_sock_filedesc, SHUT_RDWR);

    return 0;
}
