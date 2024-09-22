#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <thread>

using namespace std;
using namespace filesystem;

bool check_error(int status, bool auto_exit = 1);
void handle_connection(int cli_sock_filedesc);
void handle_get(int cli_sock_filedesc, string request_path);
void send_response(int cli_sock_filedesc, string response, string status_line = "", string headers = "", bool auto_header = 1);
void send_file(int cli_sock_filedesc, string local_path);
void send_http_status(int cli_sock_filedesc, int http_status);

int main()
{
    int srv_sock_filedesc = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in srv_sock_addr;
    srv_sock_addr.sin_family = AF_INET;
    srv_sock_addr.sin_addr.s_addr = INADDR_ANY;
    srv_sock_addr.sin_port = htons(80);

    int bind_status = bind(srv_sock_filedesc, (sockaddr*) &srv_sock_addr, sizeof(srv_sock_addr));
    check_error(bind_status);

    int listen_status = listen(srv_sock_filedesc, 100);
    check_error(listen_status);

    cout << "Listening on port 80...\n";

    while (1)
    {
        sockaddr_in cli_sock_addr;
        socklen_t addr_len = sizeof(cli_sock_addr);
        int cli_sock_filedesc = accept(srv_sock_filedesc, (sockaddr*) &cli_sock_addr, &addr_len);
        bool error = check_error(cli_sock_filedesc, 0);

        if (error) continue;

        cout << "---- Connection accepted\n";

        thread new_thread(handle_connection, cli_sock_filedesc);
        new_thread.detach();
        //handle_connection(cli_sock_filedesc);
    }

    return 0;
}

bool check_error(int status, bool auto_exit)
{
    if (status < 0)
    {
        cerr << "Error: " << strerror(errno) << endl;

        if (auto_exit)
        {
            exit(1);
        }

        return 1;
    }

    return 0;
}

void handle_connection(int cli_sock_filedesc)
{
    char buffer[256];
    string request = "";

    while (1)
    {
        fill(begin(buffer), end(buffer), 0);
        int recv_status = recv(cli_sock_filedesc, &buffer, sizeof(buffer), 0);
        bool error = check_error(recv_status, 0);

        if (error)
        {
            send_http_status(cli_sock_filedesc, 400);
            return;
        }

        request.append(buffer);

        int end_pos = request.find("\n", request.size() - size(buffer));

        if (end_pos != string::npos) break;
    }

    while (1)
    {
        if (request.size() < 2) break;

        int newline_pos = request.find_first_of("\n");
        string line = request.substr(0, newline_pos);

        if (newline_pos >= 0)
        {
            request = request.substr(newline_pos + 1);
        }
        else
        {
            request = "";
        }

        int whitespace_pos = line.find_first_of(" ");
        string first = line.substr(0, whitespace_pos);
        line = line.substr(whitespace_pos + 1);

        whitespace_pos = line.find_first_of(" ");
        string second = line.substr(0, whitespace_pos);

        if (first == "GET")
        {
            handle_get(cli_sock_filedesc, second);
        }
    }

    cout << "---- Connection closed\n\n";
    shutdown(cli_sock_filedesc, SHUT_RDWR);
}

void handle_get(int cli_sock_filedesc, string request_path)
{
    cout << "Handling GET " << request_path << " ...\n";

    if (request_path == "/")
    {
        send_file(cli_sock_filedesc, "index.html");
        return;
    }

    string current_path = filesystem::current_path();
    string full_req_path = current_path.append(request_path);
    send_file(cli_sock_filedesc, full_req_path);
}

void send_response(int cli_sock_filedesc, string content, string status_line, string headers, bool auto_header)
{
    if (status_line == "")
    {
        status_line = "HTTP/1.1 200 OK\n";
    }

    if (auto_header && content.size() > 0 && headers == "")
    {
        headers = "Content-Type: text/html\nContent-Length: " + to_string(content.size()) + "\n\n";
    }

    string response = status_line + headers + content;

    int bytes_sent = 0;
    while (bytes_sent < response.size())
    {
        int send_status = send(cli_sock_filedesc, response.c_str(), response.size(), 0);
        bool error = check_error(send_status);

        if (error) return;

        bytes_sent += send_status;
    }

    cout << "Sent " << bytes_sent << " bytes.\n";
}

void send_file(int cli_sock_filedesc, string local_path)
{
    cout << "Serving " << local_path << "...\n";

    ifstream index_fd;
    index_fd.open(local_path);

    if (!index_fd.is_open())
    {
        cout << local_path << " couldn't be opened! Does it exist?\n";
        send_http_status(cli_sock_filedesc, 404);
        return;
    }

    string content;

    while (1)
    {
        string line;
        getline(index_fd, line);

        if (line.size() == 0) break;

        content.append(line);
        content.append("\n");
    }

    index_fd.close();
    send_response(cli_sock_filedesc, content);
}

void send_http_status(int cli_sock_filedesc, int http_status)
{
    string status_line;
    string content = "";

    if (http_status == 500)
    {
        status_line = "HTTP/1.1 500 Internal Server Error\n";
        content = "<!DOCTYPE html><html>500 Internal Server Error</html>";
    }
    else if (http_status == 400)
    {
        status_line = "HTTP/1.1 400 Bad Request\n\n";
        content = "<!DOCTYPE html><html>400 Bad Request</html>";
    }
    else if (http_status == 404)
    {
        status_line = "HTTP/1.1 404 Not Found\n\n";
        content = "<!DOCTYPE html><html>404 Not Found</html>";
    }
    else
    {
        status_line = "HTTP/1.1 " + to_string(http_status) + "\n\n";
        content = "<!DOCTYPE html><html>" + to_string(http_status) + "</html>";
    }

    send_response(cli_sock_filedesc, content, status_line, "", 0);
}
