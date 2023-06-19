#include "Server.hpp"


int    commander(std::string buffer, int j, int mes, Server &slav)
{
    std::string msg(buffer);
    std::istringstream ss(msg);
    std::string command;
    std::string whatIveDone;
    ss >> command;
    while (ss)
    {
        std::string tmp;
        ss >> tmp;
        whatIveDone += " " + tmp;
    }
    std::cout << "Command: " << whatIveDone << std::endl;
    if (mes == 2 && command != "PASS")
    {
        std::cout << "Errore: password non inserita!" << std::endl;
        std::string msg = "Password non inserita, connessione chiusa!";
        send(j, msg.c_str(), msg.length(), 0);
        close(j);
        slav.getList().erase(j);
        return (1) ;
    }
    if (command == "quit" || command == "QUIT")
        slav.quit(whatIveDone, j);
    else if (command == "NICK" && mes == 3)
        slav.nick(whatIveDone, j, 3);
    else if (command == "USER"  && mes == 4)
        slav.user(whatIveDone, j, 4);
    else if (command == "PASS" && mes == 2)
        slav.pass(whatIveDone, j, 2);
    else if (command == "PING")
        slav.ping(whatIveDone, j);
    else if (command == "WHO")
        slav.who(whatIveDone, j);
    else if (command == "PRIVMSG")
        slav.privmsg(whatIveDone, j);
    else if (command == "JOIN")
        slav.join(whatIveDone, j);
    else if (command == "PART")
        slav.part(whatIveDone, j);
    else if (command == "KICK")
        slav.kick(whatIveDone, j);
    else if (command == "TOPIC")
        slav.topic(whatIveDone, j);
    else if (command == "INVITE")
        slav.invite(whatIveDone, j);
    else if (command == "INSULTA" || command == "insulta" || command == "INSULTAMI" || command == "insultami")
        slav.insulta(whatIveDone, j);
    else if (command == "SECRET" || command == "secret")
        slav.secret(whatIveDone, j);
    else if (command == "channels" || command == "CHANNELS")
        slav.channel(whatIveDone, j);
    else if (command == "MODE")
        slav.mode(whatIveDone, j);
    return (0);
}

int cycle(Server &slav)
{
	int flags = fcntl(slav.getServerSock(), F_GETFL, 0);
	fcntl(slav.getServerSock(), F_SETFL, flags | O_NONBLOCK);
    struct pollfd fds[512];
	fds[0].events = POLLIN;
	fds[0].fd = slav.getServerSock();
	fds[0].revents = 0;
	int ret = 0;
	while(1)
	{
		ret = poll(fds, slav.getList().size() + 1, -1);
		if (ret < 0)
		{
			perror("poll failed");
			break;
		}
		//poll revent to check new client
		if (fds[0].revents && POLLIN)
		{
			int clientsockfd = accept(slav.getServerSock(), NULL, NULL);
			if (clientsockfd < 0)
			{
				std::cout << "Accept fallito!" << std::endl;
				if(errno == EAGAIN || errno == EWOULDBLOCK)
				{
					std::cout << "Errore: continue." << errno << std::endl;
					continue;
				}
				else
				{
					std::cout << "Errore: break." << errno << std::endl;
					exit(0);
				}
			}
			std::map<int, User>::iterator it = slav.getList().begin();
			slav.getList().insert(std::pair<int, User>(clientsockfd, User()));

			//poll client init
			int n = slav.getList().size();
			fds[n].fd = clientsockfd;
			fds[n].events = POLLIN;
			fds[n].revents = 0;
			slav.getList()[clientsockfd].setSocket(clientsockfd);
			slav.getList()[clientsockfd].setMessages(0);
			slav.getList()[clientsockfd].setNickname("");
			slav.getList()[clientsockfd].setUsername("");
			slav.getList()[clientsockfd].setPassword("");
			
			std::cout << "Accept creato!" << std::endl;
		}

		//poll revents
		for (int j = 1; j <= (slav.getList().size()); j++)
		{
			if (fds[j].revents && POLLIN)
			{

				char buffer[1024];
				memset(buffer, 0, sizeof(buffer));
				ret = recv(fds[j].fd, buffer, sizeof(buffer), 0);
				if (ret == -1)
				{
					std::cout << "Recv fallito!" << std::endl;
					if(errno == EAGAIN || errno == EWOULDBLOCK)
					{
						std::cout << "Errore: continue." << errno << std::endl;
						continue;
					}
					else
					{
						close(fds[j].fd);
						slav.getList().erase(fds[j].fd);
						std::cout << errno << std::endl;
						return 0;
					}
				}
				else if (ret == 0)
				{
					std::cout << "Client disconnesso!" << std::endl;
					slav.ft_delete_user(fds[j].fd);
					slav.getList().erase(fds[j].fd);
					close(fds[j].fd);
				}
				else
				{
					buffer[ret] = '\0';
					std::cout << "Received: " << buffer << std::endl;
					slav.getList()[fds[j].fd].setMessages(slav.getList()[fds[j].fd].getMessages() + 1);
				}
                if (commander(buffer, fds[j].fd, slav.getList()[fds[j].fd].getMessages() ,slav))
                    break;
			}
		}
	}
    return 0;
}