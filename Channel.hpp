#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <list>
#include <poll.h>
#include <fcntl.h>
#include <sstream>
#include "Server.hpp"
#include "User.hpp"

class Server;

class Channel
{
	private:
		std::string		_name;
		std::list<User> 	_users;
		std::list<User> 	_sbirri;
		std::list<User> 	_invited;
        std::string _topic = "Welcome to the channel!";
		std::string _pw;
		int _pwOn = 0;
		int _invOn = 0;
		int _topicOn = 0;
        int _empty = 0;
		int _limitOn = 0;
		int _limit = 50;
		int _stop = 0;

	public:
		Channel(std::string &name);
		~Channel();
		std::string& getName();
		std::list<User>& getListUsers();
		std::list<User>& getSbirri();
		std::list<User>& getInvited();
        std::string& getTopic();
		std::string &getUserNick(int socket);
		User &getUser(int socket);
		std::string getPw();
		int	isPwOn();
		int	isInvOn();
		void setInvOn();
		int	istopicOn();
		int	isLimitOn();
		int	getLimit();
		void setLimitOn(Server & serv, int limit);
		void setopicOn();
		void setPw(std::string &pw);
        void setTopic(std::string& topic);
		void setSbirri(User &sbirri);
		void setInvited(User &invited);
		void setName(std::string &name);
		void setListUsers(User &user);
        void setEmpty(int empty);
        int getEmpty();
		void initMode();
		void removePw();
		void removeInvOn();
		void removetopicOn();
		void removeLimitOn();
		int	isUserIn(std::string user);
		int	isUserSbirro(std::string user);
		int	isUserInvited(std::string user);
		void remove_User(Channel ch, std::string user);
		void removeSbirro(std::string sbirro);
		void removeInvited(std::string invited);
		int checkFull();
};

#endif