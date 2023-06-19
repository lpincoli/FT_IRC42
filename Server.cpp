#include "Server.hpp"

Server::Server(int port, std::string pass) : _port(port), _password(pass){}

Server::~Server(){}


void	Server::generateBestemmieFromFile()
{
	std::ifstream file("insulti.txt");
	std::string str;
	while (std::getline(file, str))
	{
		_bestemmie.push_back(str);
	}
	file.close();
}

void    Server::printBestemmie()
{
    std::list<std::string>::iterator it = _bestemmie.begin();
    while (it != _bestemmie.end())
    {
        std::cout << *it << std::endl;
        it++;
    }
}

std::string    &Server::randomBestemmia()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int random = std::rand() % _bestemmie.size();

    std::list<std::string>::iterator it = _bestemmie.begin();
    std::advance(it, random);

    return *it;
}

void	Server::ft_delete_user(int j)
{
    std::string nick = getUserNameFromSock(j);
    std::list<Channel>::iterator itChannel = getChannels().begin();
    for( ; itChannel != getChannels().end(); itChannel++)
    {
        std::list<User>::iterator itUser = itChannel->getListUsers().begin();
        for( ; itUser != itChannel->getListUsers().end(); ++itUser)
        {
            if (itUser->getNickname() == nick)
            {
                partChannel(j, *itChannel);
                itChannel->getListUsers().erase(itUser);
                break;
            }
        }
    }
}

int	Server::createSocket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
	{
        std::cerr << "Failed to create socket." << std::endl;
        return (-1);
    }

    // Set up the address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;                   // IPv4
    serverAddress.sin_port = htons(_port);                // Port number
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);    // Any available interface

    // Bind the socket to the specified address and port
    if (bind(sockfd, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
	{
        std::cerr << "Failed to bind socket." << std::endl;
        close(sockfd);
        return (-1);
    }
    std::cout << "Socket created and bound successfully." << std::endl;
	_serverSock = sockfd;
	return (0);
}

int	Server::getPort()
{
	return (_port);
}
std::string &Server::getServerName()
{
    return (_nameServer);
}

void Server::setPort(int port)
{
    _port = port;
}

int	Server::getServerSock()
{
	return (_serverSock);
}

std::map<int, User>	&Server::getList()
{
	return (_clientSock);
}

std::string Server::getPassword()
{
    return (_password);
}

void Server::setPassword(std::string pass)
{
    _password = pass;
}

void Server::sendData(int sockfd, const std::string& data)
{
    send(sockfd, data.c_str(), data.length(), 0);
    std::cout << "Sent: " << data << std::endl;
}

std::list<Channel>& Server::getChannels()
{
    return (_channels);
}

std::string receiveData(int sockfd)
{
    const int bufferSize = 1024;
    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);

    std::string receivedData;

    while (true) {
        ssize_t bytesRead = recv(sockfd, buffer, bufferSize - 1, 0);
        if (bytesRead <= 0) {
            // Error or connection closed
            break;
        }

        receivedData += std::string(buffer, bytesRead);

        // Check if the received data ends with a newline character
        if (receivedData.back() == '\n') {
            break;
        }
    }
    return receivedData;
}

void    Server::printSbirri()
{
    std::cout << "Lista sbirri: " << std::endl;
    for (std::map<std::string, std::list<User> >::iterator it = _sbirri.begin(); it != _sbirri.end(); it++)
    {
        std::cout << "Canale: " << it->first << std::endl;
        for (std::list<User>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            std::cout << "Sbirro: " << it2->getNickname() << std::endl;
        }
    }
}

Channel& Server::getChanFromName(std::string channel)
{
    static Channel empty(empty);
    for (std::list<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
    {
        if (it->getName() == channel)
            return (*it);
    }
    empty.setEmpty(1);
    return(empty);
}

int Server::getUserSockFromNick(std::string name)
{
    for (std::map<int, User>::iterator it = _clientSock.begin(); it != _clientSock.end(); it++)
    {
        if (it->second.getNickname() == name)
            return (it->first);
    }
    return (0);
}


int	Server::isChanReal(std::string channel)
{
    std::list<Channel>::iterator it = _channels.begin();
    for(; it != _channels.end(); it++)
    {
        if((*it).getName() == channel)
            return(1);
    }
    return (0);
}

int Server::isUserReal(std::string user)
{
    std::map<int, User>::iterator it = _clientSock.begin();
    for(; it != _clientSock.end(); it++)
    {
        if(it->second.getNickname() == user)
            return(1);
    }
    return (0);
}

std::string Server::getUserNameFromSock(int sock)
{
    for (std::map<int, User>::iterator it = _clientSock.begin(); it != _clientSock.end(); it++)
    {
        if (it->first == sock)
            return (it->second.getNickname());
    }
    return (0);
}

void	Server::ft_send_all_chan(Server slav, Channel ch, std::string msg)
{
    std::list<User>::iterator it = ch.getListUsers().begin();
    for (; it != ch.getListUsers().end(); it++)
    {
        slav.sendData(it->getSocket(), msg);
    }
}

void	Server::ft_check_channel(Channel& ch)
{
    if(ch.getListUsers().size() == 1)
    {
        ch.setSbirri((*ch.getListUsers().begin()));
        std::string msg = ":IRCserv MODE "  + ch.getName() + " +o " + (ch.getListUsers().begin()->getNickname()) + "\r\n";
        ft_send_all_chan((*this), ch, msg);
        std::cout << "L'utente " << (ch.getListUsers().begin()->getNickname()) << " ora è uno sbirro!" << std::endl;
    }
    else if (ch.getListUsers().size() == 0)
    {
        for(std::list<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
        {
            if (it->getName() == ch.getName())
            {
                _channels.erase(it);
                break ;
            }
        }
    }
}

void	Server::bot_canali(Server slav, int j)
{
    std::string msg = "Fiodena Lista canali: ";
    for (std::list<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
    {
        if (it->isUserIn(_clientSock[j].getNickname()) || !(it->isPwOn()) || !(it->isInvOn()))
            msg += it->getName() + " ";
    }
    msg += "\r\n";
    slav.sendData(j, msg);
}


void	Server::ft_no_sbirro_coglio(int j, Channel& ch, std::string mode)
{
    if (mode == "+k")
    {
        std::string msg = "IRCserv Non sei uno sbirro, non puoi impostare una password!\r\n";
        sendData(j, msg);
    }
    else if (mode == "-k")
    {
        std::string msg = "IRCserv Non sei uno sbirro, non puoi togliere la password!\r\n";
        sendData(j, msg);
    }
    else if (mode == "+i")
    {
        std::string msg = "IRCserv Non sei uno sbirro, non puoi impostare la modalità inviti!\r\n";
        sendData(j, msg);
    }
    else if (mode == "-i")
    {
        std::string msg = "IRCserv Non sei uno sbirro, non puoi togliere la modalità inviti!\r\n";
        sendData(j, msg);
    }
    else if (mode == "+t")
    {
        std::string msg = "IRCserv Non sei uno sbirro, non puoi impostare la modalità topic!\r\n";
        sendData(j, msg);
    }
    else if (mode == "-t")
    {
        std::string msg = "IRCserv Non sei uno sbirro, non puoi togliere la modalità topic!\r\n";
        sendData(j, msg);
    }
    else if (mode == "-l")
    {
        std::string msg = "IRCserv Non sei uno sbirro, non puoi togliere la modalità limit!\r\n";
        sendData(j, msg);
    }
    else if (mode == "+l")
    {
        std::string msg = "IRCserv Non sei uno sbirro, non puoi impostare la modalità limit!\r\n";
        sendData(j, msg);
    }
}
