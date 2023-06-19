#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <list>
#include <poll.h>
#include <fcntl.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <map>

class User
{
private:
    std::string __nickname;
    std::string _username;
    std::string _password;
    int _socket;
    int _messages;
public:
    User();
    ~User();
    std::string& getNickname();
    std::string& getUsername();
    std::string& getPassword();
    int getMessages();
    int getSocket();
    void setNickname(std::string nickname);
    void setUsername(std::string username);
    void setPassword(std::string password);
    void setSocket(int socket);
    void setMessages(int messages);
};

#endif