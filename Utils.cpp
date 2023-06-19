#include "Server.hpp"

void	ft_update_list(Server slav, std::string channel, int j, std::list<User> userlist)
{
	std::cout << "Canale trovato, aggiunto l'utente!" << std::endl;
	std::string msg = ":" + slav.getList()[j].getNickname() + "! JOIN " + channel + "\r\n";
	std::list<User>::iterator ite = userlist.begin();
	std::string users;
	while (ite != userlist.end())
	{
		users += (*ite).getNickname();
		users += " ";
		ite++;
	}
	ite = userlist.begin();
	std::string server = "IRCserv";
	Channel& ch = slav.getChanFromName(channel);
	while (ite != userlist.end())
	{
		slav.sendData((*ite).getSocket(), msg);
		ite++;
	}
	for (std::list<User>::iterator sbirr = ch.getSbirri().begin(); sbirr != ch.getSbirri().end(); sbirr++)
	{
		std::string msg3 = ":IRCserv MODE "  + channel + " +o " + (*sbirr).getNickname() + "\r\n";
		slav.ft_send_all_chan(slav, ch, msg3);
	}
}


void	ft_part_list(Server slav, std::string channel, int j, std::list<User> userlist)
{
	std::cout << "Canale trovato, aggiunto l'utente!" << std::endl;
	std::string msg = ":PART " + channel + "\r\n";
	std::list<User>::iterator ite = userlist.begin();
	std::string users;
	while (ite != userlist.end())
	{
		users += (*ite).getNickname();
		users += " ";
		ite++;
	}
	ite = userlist.begin();
	std::string server = "IRCserv";
	Channel& ch = slav.getChanFromName(channel);
	while (ite != userlist.end())
	{
		
		std::string msg1 = ":" + server + " 353 " + (*ite).getNickname() + " = " + channel + " :" + users + "\r\n";
		std::string msg2 = ":" + server + " 366 " + (*ite).getNickname() + " " + channel + " :End of /NAMES list\r\n";
		slav.sendData((*ite).getSocket(), msg);
		slav.sendData((*ite).getSocket(), msg1);
		slav.sendData((*ite).getSocket(), msg2);
		ite++;
	}
	for (std::list<User>::iterator sbirr = ch.getSbirri().begin(); sbirr != ch.getSbirri().end(); sbirr++)
	{
		std::string msg3 = ":IRCserv MODE "  + channel + " +o " + (*sbirr).getNickname() + "\r\n";
		slav.ft_send_all_chan(slav, ch, msg3);
	}
}

void	ft_kick_list(Server slav, std::string channel, int j,std::list<User> *userlist, std::string nick)
{
	std::cout << "Canale trovato, rimosso l'utente!" << std::endl;
	std::string msg = ":" + slav.getList()[j].getNickname() + " KICK " + channel + " " + nick + "\r\n";
	std::list<User>::iterator it = userlist->begin();
	std::string users;
	while (it != userlist->end())
	{
		users += (*it).getNickname();
		users += " ";
		it++;
	}
	it = userlist->begin();
	std::string server = "IRCserv";
	while (it != userlist->end())
	{
		slav.sendData((*it).getSocket(), msg);
		it++;
	}
	it = userlist->begin();
	while (it != userlist->end())
	{
		if ((*it).getNickname() == nick)
			userlist->erase(it);
		std::cout << "Utente rimosso dal canale!" << std::endl;
		break;
		it++;
	}
}

void	ft_send_all(Server slav, std::string channel, int j, std::list<User> userlist, std::string msg)
{
	std::list<User>::iterator it = userlist.begin();
	while (it != userlist.end())
	{
		slav.sendData((*it).getSocket(), msg);
		it++;
	}
}

int	init(int ac, char **av, Server &slav)
{
	if (ac != 3)
	{
		std::cout << "Program usage ./ircserv <port> <password>" << std::endl;
		return(1);
	}
	try
	{
		std::stoi(av[1]);
		if (std::stoi(av[1]) < 1024 || std::stoi(av[1]) > 65535)
		{
			std::cerr << "Insert a numeric value between 1024 and 65535" << std::endl;
			return(1);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "Insert a numeric value" << std::endl;
	}
	slav.generateBestemmieFromFile();
	slav.setPort(std::stoi(av[1]));
	slav.setPassword(av[2]);
    if (slav.createSocket() == -1)
	{
		std::cout << "Socket non creato!" << std::endl;
		return (1);
	}
	std::cout << "Socket creato!" << std::endl;
	if (listen(slav.getServerSock(), 10) == -1)
	{
		std::cout << "Listen non creato!" << std::endl;
		return (1);
	}
	std::cout << "Listen creato!" << std::endl;
	return (0);
}

void	ft_send_channel(Server slav, std::string channel, int j)
{
	std::string server_name = "IRCserv";
	Channel ch = slav.getChanFromName(channel);
	if (ch.getEmpty() == true)
	{
		std::string msg = ":" + server_name + " 403 " + slav.getList()[j].getNickname() + " " + channel + " :No such channel\r\n";
		slav.sendData(slav.getList()[j].getSocket(), msg);
		return ;
	}
	std::string topic = ch.getTopic();


	//send JOIN msg
	std::string msg = ":" + slav.getList()[j].getNickname() + " JOIN " + channel + "\r\n";
	slav.sendData(j, msg);

	//send RPL_TOPIC
	msg = ":" + server_name + " 332 " + slav.getList()[j].getNickname() + " " + channel + " :" + topic + "\r\n";
	slav.sendData(j, msg);

	//send RPL_NAMREPLY
	std::list<User>::iterator ite = ch.getListUsers().begin();
	std::string users;
	while (ite != ch.getListUsers().end())
	{
		users += (*ite).getNickname();
		users += " ";
		ite++;
	}
	msg = ":" + server_name + " 353 " + slav.getList()[j].getNickname() + " = " + channel + " :" + users + "\r\n";
	slav.sendData(j, msg);

	//send RPL_ENDOFNAMES
	msg = ":" + server_name + " 366 " + slav.getList()[j].getNickname() + " " + channel + " :End of /NAMES list\r\n";
	slav.sendData(j, msg);
}