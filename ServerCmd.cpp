#include "Server.hpp"

int Server::pass(std::string nick, int j, int mes)
{
    if (mes != 2)
        return (1);
	std::istringstream ss(nick);
	std::string passw;
	ss >> passw;
	std::cout << "Passw: " << passw << std::endl;
	std::cout << "Password: " << getPassword() << std::endl;
	if (passw == ":" + getPassword())
	{
		std::cout << "Password corretta, complimenti!" << std::endl;
		std::string msg = getServerName() + " Password corretta, complimenti!\r\n";
		send(j, msg.c_str(), msg.length(), 0);
	}
	else
	{
		std::cout << "Password errata, connessione chiusa!" << std::endl;
		std::string msg = "464 :IRCserv Password errata, connessione chiusa! /r/n";
		sendData(j, msg);
		close(j);
		getList().erase(j);
	}
	return 1;
}

int Server::ping(std::string nick, int j)
{
	std::string pong = "PONG " + getList()[j].getNickname() + "\r\n";
	sendData(j, pong);
	return 1;
}

int	Server::who(std::string nick, int j)
{
	std::istringstream ss(nick);
	std::string list;
	std::string str;
	ss >> str;
	std::cout << "WHO: " << str << std::endl;
	for(std::list<Channel>::iterator itChannel = getChannels().begin(); itChannel != getChannels().end(); ++itChannel)
	{
		if (itChannel->getName() == str)
		{
			list = ":" + getServerName() + "! 353 " + getList()[j].getNickname() + " = " + itChannel->getName() + " :";
			for (std::list<User>::iterator itClient = itChannel->getListUsers().begin(); itClient != itChannel->getListUsers().end(); ++itClient)
			{
		// :hostname 353 nickname = #channel :nickname nickname (can be more than one here or sent in multiple messages)
				list += itClient->getNickname() + " ";
			}
			list += "\r\n:" + getServerName() + "! 366 " + getList()[j].getNickname() + " " + itChannel->getName() + " :End of /NAMES list.\r\n";
			std::cout << "LIST: " << list << std::endl;
			sendData(j, list);
		}
	}
	// std::map<int, User>::iterator itUser = getList().begin();
	for(std::map<int, User>::iterator itUser = getList().begin(); itUser != getList().end(); ++itUser)
	{
		if (itUser->second.getNickname() == str)
		{
			list = ":" + getServerName() + "! 352 " + getList()[j].getNickname() + " =";
			for (std::list<Channel>::iterator itChannel = getChannels().begin(); itChannel != getChannels().end(); ++itChannel)
			{
				if (itChannel->isUserIn(str))
				{
					list += " " + itChannel->getName();
				}
			// :hostname 353 nickname = #channel :nickname nickname (can be more than one here or sent in multiple messages)
			}
			list += " " + itUser->second.getUsername() + " " /*+ itUser->second.getHostname() + " "*/ + getServerName() + " " + itUser->second.getNickname() + " H :0 " + itUser->second.getUsername() + "\r\n";
			std::cout << "LIST: " << list << std::endl;
			sendData(j, list);
		}
	}
	return (1);
}

int Server::privmsg(std::string command, int j)
{
	std::istringstream ss(command);
	std::string nick;
	ss >> nick;
	std::string sender_nick = getList()[j].getNickname();
	if (nick[0] == '#')
	{
		std::string msg;
		Channel& chan = getChanFromName(nick);
		std::list<User>::iterator it = chan.getListUsers().begin();
		if (chan.getEmpty() == true)
		{
			std::string msg = ":IRCserv 403 " + getList()[j].getNickname() + " " + nick + " :No such channel\r\n";
			sendData(getList()[j].getSocket(), msg);
			return (1);
		}
		for (it; it != chan.getListUsers().end(); it++)
		{
			if ((*it).getNickname() == sender_nick)
				break;
		}
		if (it == chan.getListUsers().end())
		{
			std::cout << "Utente non appartenente al canale!" << std::endl;
			sendData(j, "IRCserv Utente non appartenente al canale!\r\n");
			return (1);
		}
		std::list<User>::iterator ite = chan.getListUsers().begin();
		while (command.find(":"))
			ss >> command;
		command.erase(0, 1);
		msg =  ":" + sender_nick + " PRIVMSG " + nick + " :";
		while(ss)
		{
			msg += command;
			msg += " ";
			ss >> command;
		}
		msg.erase(msg.length() - 1, 1);
		msg += "\r\n";
		while (ite != chan.getListUsers().end())
		{
			if ((*ite).getNickname() != sender_nick)
				sendData((*ite).getSocket(), msg);
			ite++;
		}
	}
	else
	{
		std::string msg;
		if (isUserReal(nick))
		{
			int r_sock = getUserSockFromNick(nick);
			while (command.find(":"))
				ss >> command;
			command.erase(0, 1);
			msg =  ":" + sender_nick + "! PRIVMSG " + nick + " :";
			while(ss)
			{
				msg += command;
				msg += " ";
				ss >> command;
			}
			msg.erase(msg.length() - 1, 1);
			msg += "\r\n";

			sendData(r_sock, msg);
		}
	}
	return (1);
}

int Server::join(std::string command, int j)
{
	std::istringstream ss(command);
	std::string channel;
	ss >> channel;
	std::list<Channel>::iterator it = getChannels().begin();
	for(it; it != getChannels().end(); it++)
	{
		if ((*it).getName() == channel)
			break;
	}
	if (it == getChannels().end())
	{
		Channel new_channel(channel);
		new_channel.setListUsers(getList()[j]);
		new_channel.setSbirri(getList()[j]);
		getChannels().push_back(new_channel);
		std::cout << "Canale non trovato, creato!" << std::endl;
		ft_send_channel(*this, channel, j);
		Channel& ch = getChanFromName(channel);
		std::string msg = ":IRCserv MODE "  + channel + " +o " + getUserNameFromSock(j) + "\r\n";
		ft_send_all_chan(*this, ch, msg);
	}
	else
	{
		Channel& ch = getChanFromName(channel);
		
		if (ch.isInvOn() && !ch.isUserInvited(getUserNameFromSock(j)))
		{
			sendData(j, "You need to be invited to join this channel!\r\n");
				return (1);
		}
		else if(ch.isPwOn() != 0 && !ch.isUserInvited(getUserNameFromSock(j)))
		{
			if(ss)
			{
				std::string pw_ins;
				ss >> pw_ins;
				if (pw_ins != ch.getPw())
				{
					sendData(j, "You need to join with the correct channel password!\r\n");
					return (1);
				}
			}
			else
			{
				sendData(j, "IRCserv You need to join with the channel password!\r\n");
				return (1);
			}
		}
		if (ch.isLimitOn() && ch.getLimit() == ch.getListUsers().size())
		{
			sendData(j, "Channel User limit reached!\r\n");
			return (1);
		}
		(*it).setListUsers(getList()[j]);
		ft_update_list(*this, channel, j, (*it).getListUsers());
		ft_send_channel(*this, channel, j);
		for (std::list<User>::iterator sbirr = ch.getSbirri().begin(); sbirr != ch.getSbirri().end(); sbirr++)
		{
			std::string msg3 = ":IRCserv MODE "  + channel + " +o " + (*sbirr).getNickname() + "\r\n";
			sendData(j, msg3);
		}
        if (ch.isUserInvited(getUserNameFromSock(j)))
            ch.removeInvited(getUserNameFromSock(j));
	}
	return (1);
}

int		Server::part(std::string command, int j)
{
	std::stringstream ss(command);
	std::string channel;
	ss >> channel;
	Channel& ch = getChanFromName(channel);
	std::list<User>::iterator it = ch.getListUsers().begin(); 
	if(!isChanReal(channel))// channel not found
	{
		std::cout << "Canale non trovato!" << std::endl;
		sendData(j, "IRCserv Canale non trovato!\r\n");
		return (1);
	}
	if(ch.isUserIn(getUserNameFromSock(j)))
	{
		for(; it != ch.getListUsers().end(); it++)
		{
			if (it->getSocket() == j)
			{
				ch.getListUsers().erase(it);
				ch.removeSbirro(it->getNickname());
				std::cout << "Utente " << it->getNickname() << "rimosso dal canale!" << std::endl;
				break ;
			}
		}
	}
	else
	{
		sendData(j, "IRCserv Non puoi uscire da un canale a cui non appartieni!\r\n");
		return (1);
	}
	std::string msg = ":" + getList()[j].getNickname() + " PART " + channel + " \r\n";
	std::string msg1 = ":" + getList()[j].getNickname() +  "!" + " PART :" + channel + "\n";
	ft_send_all_chan(*this, ch, msg1);
	sendData(j, msg);
	ft_check_channel(ch);
	return (1);
}


void	Server::partChannel(int j, Channel& channel)
{
		std::string msg1 = ":" + getList()[j].getNickname() +  "!" + " PART :" + channel.getName() + "\n";
		ft_send_all_chan(*this, channel, msg1);
}


int Server::kick(std::string command, int j)
{
	std::stringstream ss(command);
	std::string channel;
	ss >> channel;		//save name of channel
	User sender_user = getList().find(j)->second; // get User from socket
	if(!isChanReal(channel))// channel not found
	{
		std::cout << "Canale non trovato!" << std::endl;
		sendData(j, "IRCserv Canale non trovato!\r\n");
		return (1);
	}
	Channel& ch = getChanFromName(channel);// get Channel class
	std::list<User>::iterator it = ch.getListUsers().begin(); 
	std::string nick;
	ss >> nick;				//get nick of kicked
	if(!ch.isUserSbirro(getUserNameFromSock(j)))
	{
		sendData(j, "IRCserv Non sei uno sbirro del canale, non puoi cacciare gente!\r\n");
		return (1);
	}
	else if (!ch.isUserIn(nick))
	{
		sendData(j, "IRCserv L'utente da cacciare non è parte del canale!\r\n");
		return (1);
	}
	else
	{
		for(; it != ch.getListUsers().end(); it++)
		{
			if (it->getNickname() == nick)
				break ;								
		}
	}
	std::string reason;
	std::string temp;
	while(ss)
	{
		ss >> temp;	
		reason += temp;
		reason += " ";
		temp = "";
	}
	if (it != ch.getListUsers().end())
	{
		std::string msg = ":" + getList()[j].getNickname() + " KICK " + channel + " " + nick + " " + reason + "\r\n";
		std::string msg1 = ":" + getList()[j].getNickname() +  "!" + " KICK " + channel + " " + nick + " " + reason + "\n";
		ft_send_all_chan(*this, ch, msg1);
		sendData(j, msg);
		if(ch.isUserSbirro(nick))
			ch.removeSbirro(nick);
		ch.getListUsers().erase(it);
		std::cout << "Utente " << it->getNickname() << " rimosso dal canale!" << std::endl;
	}
	ft_check_channel(ch);
	return (1);
}

int Server::topic(std::string command, int j)
{
	std::stringstream ss(command);
	std::string channel;
	ss >> channel;
	if(!isChanReal(channel))
	{
		std::cout << "Canale non trovato!" << std::endl;
		sendData(j, "IRCserv Canale non trovato!\r\n");
		return (1);
	}
	Channel ch = getChanFromName(channel);
	if(!ch.isUserSbirro(getUserNameFromSock(j)) && ch.istopicOn())
	{
		sendData(j, "IRCserv Non sei uno sbirro del canale, non puoi cambiare il topic!\r\n");
		return (1);
	}
	std::string tmp;
	std::string topic;
	ss >> topic;
	while (topic.find(":"))
		ss >> topic;
		command.erase(0, 1);
	while(ss >> tmp)
		topic += " " + tmp;
	std::string msg = ":" + getList()[j].getNickname() + " TOPIC " + channel + " " + topic + "\r\n";
	std::string msg1 = ":" + getList()[j].getNickname() +  "!" + " TOPIC " + channel + " " + topic + "\n";
	ft_send_all(*this, channel, j, ch.getListUsers(), msg1);
	ch.setTopic(topic);
	return (1);
}

int Server::invite(std::string command, int j)
{
	std::stringstream ss(command);
	std::string pippo;
	std::string channel;
	ss >> pippo;
	ss >> channel;
	if(!isChanReal(channel))
	{
		std::cout << "Canale non trovato!" << std::endl;
		sendData(j, "IRCserv Canale non trovato!\r\n");
		return (1);
	}
	Channel& ch = getChanFromName(channel);
	if(!ch.isUserIn(getUserNameFromSock(j)))
	{
		sendData(j, "IRCserv Non appartieni al canale, non puoi invitare gente!\r\n");
		return (1);
	}
	if (!isUserReal(pippo))
	{
		sendData(j, "IRCserv L'utente invitato non esiste!\r\n");
		return (1);
	}
	std::map<int, User>::iterator finder = getList().find(getUserSockFromNick(pippo));
	ch.setInvited(finder->second);
	std::string msg = ":" + pippo + " INVITE " + getList()[j].getNickname() + " " + channel + "\r\n";
	sendData(getUserSockFromNick(pippo), msg);
	return (1);
}

int Server::insulta(std::string command, int j)
{
	std::stringstream ss(command);
	std::string bastarduser;
	std::string channel;
	std::string temp;
	ss >> channel;
	while (ss)
	{
		ss >> temp;
		bastarduser += temp;
		bastarduser += " ";
		temp = "";
	}
	if(!isChanReal(channel))
	{
		std::cout << "Canale non trovato!" << std::endl;
		sendData(j, "IRCserv Canale non trovato!\r\n");
		return (1);
	}
	Channel& ch = getChanFromName(channel);
	if (command == "INSULTAMI" || command == "insultami")
		bastarduser = getUserNameFromSock(j);
	ft_send_all_chan(*this, ch, "Fiodena " + bastarduser + " " + randomBestemmia() + "\r\n");
	return (1);
}

int Server::secret(std::string command, int j)
{
	std::stringstream ss(command);
	std::string secret;
	std::string channel;
	std::string temp;
	ss >> channel;
	while (ss)
	{
		ss >> temp;
		secret += temp;
		secret += " ";
		temp = "";
	}
	if(!isChanReal(channel))
	{
		std::cout << "Canale non trovato!" << std::endl;
		sendData(j, "IRCserv Canale non trovato!\r\n");
		return (1);
	}
	Channel& ch = getChanFromName(channel);
	ft_send_all_chan(*this, ch, "Segreto " + secret + "\r\n");
	return (1);
}

int Server::channel(std::string command, int j)
{
	std::string msg = "IRCserv Canali disponibili: ";
	bot_canali(*this, j);
	return (1);
}

int Server::mode(std::string command, int j)
{
	std::stringstream ss(command);
	std::string channel;
	ss >> channel;
	std::string mode;
	ss >> mode;
	std::string nick;
	ss >> nick;
	if(!isChanReal(channel))
	{
		std::cout << "Canale non trovato!" << std::endl;
		sendData(j, "IRCserv Canale non trovato!\r\n");
		return (1);
	}
	Channel& ch = getChanFromName(channel);
	std::list<User>::iterator it = ch.getListUsers().begin(); 
	if (mode == "+o" || mode == "-o")
	{
		if(!ch.isUserIn(getUserNameFromSock(j)) || !ch.isUserSbirro(getUserNameFromSock(j)))
		{
			sendData(j, "IRCserv Non sei uno sbirro del canale, non puoi assumere gente!\r\n");
			return (1);
		}
		else if (!ch.isUserIn(nick))
		{
			sendData(j, "IRCserv L'utente da fare sbirro non è parte del canale!\r\n");
			return (1);
		}
		else if (mode == "+o")
		{
			if(ch.isUserSbirro(nick))
			{
				sendData(j, "IRCserv L'utente è già uno sbirro del canale!\r\n");
				return (1);
			}
			else
			{
				std::map<int, User>::iterator finder = getList().find(getUserSockFromNick(nick));
				ch.setSbirri(finder->second);
				std::string msg = ":"+ getList()[j].getNickname() + " MODE "  + channel + " +o " + nick + "\r\n";
				ft_send_all_chan(*this, ch, msg);
				std::cout << "L'utente " << nick << " ora è uno sbirro!" << std::endl;
			}
		}
		else
		{
			if(ch.isUserSbirro(nick))
			{
				std::list<User>::iterator finder = ch.getSbirri().begin();
				for (; finder != ch.getSbirri().end(); finder++)
				{
					if (finder->getNickname() == nick)
					{
						ch.removeSbirro(nick);
						std::cout << "L'utente " << nick << " non è più uno sbirro!" << std::endl;
						std::string msg = ":"+ getList()[j].getNickname() + " MODE "  + channel + " -o " + nick + "\r\n";
						ft_send_all_chan(*this, ch, msg);
						break ;
					}
				}
			}
			else
			{
				sendData(j, "IRCserv L'utente è già un comune cittadino del canale!\r\n");
				return (1);
			}
		}
	}
	if (ch.isUserSbirro(getUserNameFromSock(j)))
	{
		if (mode == "+k")
		{
			ch.setPw(nick);
			std::string msg = ":"+ getList()[j].getNickname() + " MODE "  + channel + " +k " + nick + "\r\n";
			ft_send_all_chan(*this, ch, msg);
		}
		else if (mode == "-k")
		{
			ch.removePw();
			std::string msg = ":"+ getList()[j].getNickname() + " MODE "  + channel + " -k " + nick + "\r\n";
			ft_send_all_chan(*this, ch, msg);
		}
		else if (mode == "+i")
		{
			ch.setInvOn();
			std::string msg = ":"+ getList()[j].getNickname() + " MODE "  + channel + " +i " + "\r\n";
			ft_send_all_chan(*this, ch, msg);
		}
		else if (mode == "-i")
		{
			ch.removeInvOn();
			std::string msg = ":"+ getList()[j].getNickname() + " MODE "  + channel + " -i " + "\r\n";
			ft_send_all_chan(*this, ch, msg);
		}
		else if (mode == "+t")
		{
			ch.setopicOn();
			std::string msg = ":"+ getList()[j].getNickname() + " MODE "  + channel + " +t " + "\r\n";
			ft_send_all_chan(*this, ch, msg);
		}
		else if (mode == "-t")
		{
			ch.removetopicOn();
			std::string msg = ":"+ getList()[j].getNickname() + " MODE "  + channel + " -t " + "\r\n";
			ft_send_all_chan(*this, ch, msg);
		}
		else if (mode == "-l")
		{
			ch.removeLimitOn();
			std::string msg = ":"+ getList()[j].getNickname() + " MODE "  + channel + " -l " + "\r\n";
			ft_send_all_chan(*this, ch, msg);
		}
		else if (mode == "+l")
		{
			int limit = 500;
			if (nick != " ")
			{
				try
				{
					std::stoi(nick);
				}
				catch(const std::exception& e)
				{
					std::string msg = "IRCserv Il parametro inserito non è un numero!\r\n";
					sendData(j, msg);
					return (1);
				}		
			}
			limit = std::stoi(nick);
			if (limit < 1)
			{
				std::string msg = "IRCserv Il parametro inserito non è valido!\r\n";
				sendData(j, msg);
				return (1);
			}
			std::cout << "limit: " << limit << std::endl;
			ch.setLimitOn(*this, limit);
			std::string msg = ":"+ getList()[j].getNickname() + " MODE "  + channel + " +l " + std::to_string(limit) + " " + "\r\n";
			ft_send_all_chan(*this, ch, msg);
		}
	}
	else
		ft_no_sbirro_coglio(j, ch, mode);
	return (1);
}

int Server::quit(std::string command, int j)
{
	ft_delete_user(j);
	close(j);
	getList().erase(j);
	return(1) ;
}

int Server::nick(std::string command, int j, int mes)
{
    if (mes != 3)
        return (1);
	std::stringstream ss(command);
	std::string nick;
	ss >> nick;
	std::cout << "Nick arrivato: " << nick << std::endl;
	getList()[j].setNickname(nick);
	std::cout << "Nick salvato: " << getList()[j].getNickname() << std::endl;
    return (1);
}

int Server::user(std::string command, int j, int mes)
{
	if (mes != 4)
		return (1);
	std::stringstream ss(command);
	std::string user;
	while (command.find(":"))
		ss >> command;
	command.erase(0, 1);
	while(ss)
	{
		user += command;
		user += " ";
		ss >> command;
	}
	user.erase(user.length() - 1, 1);
	std::cout << "User arrivato: " << user << std::endl;
	getList()[j].setUsername(user);
	std::cout << "User salvato: " << getList()[j].getUsername() << std::endl;
	std::string welcome = "001 "+ getList()[j].getNickname() + " :Welcome to the server IRCslav, " + getList()[j].getNickname() + "\r\n";
	sendData(j, welcome);
	welcome = "IRCserv This is NOT the global chat, to join a channel type /join <channel_name>!\r\n";
	sendData(j, welcome);
	return (1);
}
