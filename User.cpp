#include "User.hpp"

User::User(){}

User::~User(){}

std::string& User::getNickname()
{
    return (__nickname);
}

std::string& User::getUsername()
{
    return (_username);
}

std::string& User::getPassword()
{
    return (_password);
}

int User::getSocket()
{
    return (_socket);
}

void User::setNickname(std::string nickname)
{
    __nickname = nickname;
}

void User::setUsername(std::string username)
{
    _username = username;
}

void User::setPassword(std::string password)
{
    _password = password;
}

void User::setSocket(int socket)
{
    _socket = socket;
}

int User::getMessages()
{
    return (_messages);
}

void User::setMessages(int  messages)
{
    _messages = messages;
}