#ifndef SERVER_HPP
#define SERVER_HPP

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
#include "User.hpp"
#include <signal.h>
#include "Channel.hpp"
#include <fstream>
#include <ctime>
#include <netinet/in.h>
#include <arpa/inet.h>

class User;
class Channel;
 
class Server
{
	private:
		std::string		_password;
		int				_port;
		int  			_serverSock;
		std::map<int, User>	_clientSock;
		std::map<std::string, std::list<User> > _sbirri;
		std::list<Channel> _channels;
		std::list<std::string> _bestemmie;
		std::string 	_nameServer = "IRCslav";
	public:
		Server(int port, std::string pass);
		~Server();
		int	createSocket();
		int	getPort();
		std::string &getServerName();
		void setPort(int port);
		std::map<int, User>& getList();
		int getServerSock();
		std::string getPassword();
		void setPassword(std::string pass);
		int getUserSockFromNick(std::string name);
		std::string getUserNameFromSock(int sock);
		void sendData(int sockfd, const std::string& data);
		std::list<Channel>& getChannels();
		Channel& getChanFromName(std::string name);
		int	isChanReal(std::string channel);
		int isUserReal(std::string user);
		void printSbirri();
		void	ft_send_all_chan(Server slav, Channel ch, std::string msg);
		void	ft_check_channel(Channel& ch);
		void	ft_no_sbirro_coglio(int j, Channel& ch, std::string mode);
		void	generateBestemmieFromFile();
		void    printBestemmie();
		std::string    &randomBestemmia();
		void	bot_canali(Server slav, int j);
		void	ft_delete_user(int j);
		void	partChannel(int j, Channel& channel);
		int	pass(std::string nick, int j, int ms);
		int nick(std::string command, int j, int mes);
		int user(std::string command, int j, int mes);
		int	ping(std::string nick, int j);
		int	who(std::string nick, int j);
		int	join(std::string command, int j);
		int	privmsg(std::string command, int j);
		int	part(std::string command, int j);
		int kick(std::string command, int j);
		int topic(std::string command, int j);
		int invite(std::string command, int j);
		int insulta(std::string command, int j);
		int secret(std::string command, int j);
		int channel(std::string command, int j);
		int mode(std::string command, int j);
		int quit(std::string command, int j);
};

void	ft_update_list(Server slav, std::string channel, int j, std::list<User> userlist);
void	ft_part_list(Server slav, std::string channel, int j, std::list<User> userlist);
void	ft_send_channel(Server slav, std::string channel, int j);
void	ft_kick_list(Server slav, std::string channel, int j,std::list<User> *userlist, std::string nick);
void	ft_send_all(Server slav, std::string channel, int j, std::list<User> userlist, std::string msg);
void	ft_send_channel(Server slav, std::string channel, int j);
std::string receiveData(int sockfd);
int		init(int ac, char **av, Server &slav);
int 	cycle(Server &slav);


#endif
