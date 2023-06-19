#include "Server.hpp"

Server slav(0, "");

void sig_handler(int signo)
{
	if (signo == SIGINT)
	{
		std::map<int, User>::iterator it = slav.getList().begin();
		std::map<int, User>::iterator ite = slav.getList().end();
		std::cout << std::endl;
		std::cout << "Server chiuso!" << std::endl;
		for(int i = 0; it != ite, i < slav.getList().size(); it++, i++)
		{
			slav.sendData((*it).first, "DISCONNECTED\r\n");
			close((*it).first);
		}
		close(slav.getServerSock());
		exit(0);
	}
}

int	main(int ac, char **av)
{
	signal(SIGINT, sig_handler);
	if (init(ac, av, slav))
		return (1);
	if (cycle(slav))
		return (1);
	return (0);
}