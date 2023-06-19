#include "Channel.hpp"

Channel::Channel(std::string &name) : _name(name)
{
}

Channel::~Channel()
{
}

std::string& Channel::getName()
{
	return (_name);
}

std::list<User>& Channel::getListUsers()
{
	return (_users);
}

std::list<User>& Channel::getSbirri()
{
	return (_sbirri);
}

std::list<User>& Channel::getInvited()
{
	return (_invited);
}

std::string &Channel::getUserNick(int socket)
{
	static std::string empty = "";
	for (std::list<User>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		if ((*it).getSocket() == socket)
			return ((*it).getNickname());
	}
	return (empty);
}

User &Channel::getUser(int socket)
{
	static User empty;
	for (std::list<User>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		if ((*it).getSocket() == socket)
			return (*it);
	}
	return (empty);
}

void Channel::setName(std::string &name)
{
	_name = name;
}

void Channel::setListUsers(User &user)
{
	_users.push_back(user);
}

void Channel::setInvited(User &invited)
{
	_invited.push_back(invited);
}

void Channel::setSbirri(User &sbirri)
{
	_sbirri.push_back(sbirri);
}

void Channel::setTopic(std::string& topic)
{
    _topic = topic;
}

std::string& Channel::getTopic()
{
    return (_topic);
}

void Channel::setEmpty(int empty)
{
    _empty = empty;
}

int Channel::getEmpty()
{
    return (_empty);
}

void Channel::setPw(std::string &pw)
{
	_pwOn = 1;
	_pw = pw;
}

int	Channel::isPwOn()
{
	return(_pwOn);
}

std::string Channel::getPw()
{
	return(_pw);
}

void Channel::removePw()
{
	_pwOn = 0;
	_pw = "";
}


int	Channel::isUserIn(std::string user)
{
	std::list<User>::iterator finder = _users.begin();
	for (; finder != _users.end(); finder++)
	{
		if ((*finder).getNickname() == user)
			return (1);
	}
	return (0);
}

int	Channel::isUserSbirro(std::string user)
{
    for (std::list<User>::iterator it = _sbirri.begin(); it != _sbirri.end(); it++)
    {
                if (it->getNickname() == user)
                    return (1);
    }
    return (0);
}

int	Channel::isUserInvited(std::string user)
{
    for (std::list<User>::iterator it = _invited.begin(); it != _invited.end(); it++)
    {
                if (it->getNickname() == user)
                    return (1);
    }
    return (0);
}

void Channel::remove_User(Channel ch, std::string user)
{
	std::list<User>::iterator finder = ch.getListUsers().begin();
	for(; finder != ch.getListUsers().end(); finder++)
	{
		if (finder->getNickname() == user)
		{
			ch.getListUsers().erase(finder);
			std::cout << "Utente rimosso dal canale!" << std::endl;
			break ;
		}
	}
	if (finder == ch.getListUsers().end())
		std::cout << "L'utente non esiste nel canale!" << std::endl;
}

void Channel::removeSbirro(std::string sbirro)
{
	std::list<User>::iterator finder = _sbirri.begin();
	for(; finder != _sbirri.end(); finder++)
	{
		if (finder->getNickname() == sbirro)
		{
			_sbirri.erase(finder);
			std::cout << "Utente rimosso dagli sbirri del canale!" << std::endl;
			break ;
		}
	}
	if (finder == _sbirri.end())
	 	std::cout << "L'utente è già un comune cittadino del canale!" << std::endl;
}

void Channel::removeInvited(std::string invited)
{
	std::list<User>::iterator finder = _invited.begin();
	for(; finder != _invited.end(); finder++)
	{
		if (finder->getNickname() == invited)
		{
			_invited.erase(finder);
			std::cout << "Utente rimosso dagli invitati del canale!" << std::endl;
			break ;
		}
	}
	if (finder == _invited.end())
	 	std::cout << "L'utente non è stato invitato nel canale!" << std::endl;
}

int	Channel::isInvOn()
{
	return (_invOn);
}

void Channel::setInvOn()
{
	_invOn = 1;
}

void Channel::removeInvOn()
{
	_invOn = 0;
}

int	Channel::istopicOn()
{
	return(_topicOn);
}
void Channel::setopicOn()
{
	_topicOn = 1;
}

void Channel::removetopicOn()
{
	_topicOn = 0;
}

int	Channel::isLimitOn()
{
	return(_limitOn);
}

int Channel::checkFull()
{
	if (!_limitOn)
		return (0);
	if (_stop)
		return (1);
	if (_limitOn)
	{
		if (_limit == _users.size())
		{
			_stop = 1;
			return (1);
		}
		else if (_limit < _users.size())
		{
			_stop = 0;
			return (0);
		}
	}
	return (0);
}

void Channel::setLimitOn(Server &serv,int limit)
{
	_limitOn = 1;
	_limit = limit;
	
	if (_limit > _users.size())
	{
		_stop = 0;
		return ;
	}
	else if (_limit == _users.size())
	{
		_stop = 1;
		return ;
	}
	int i = 0;
	for (std::list<User>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		if (i >= _limit)
		{
			std::string server = "IrcServer";
			std::string reason = "Channel is full";
			std::string msg = ":" + server + " KICK " + getName() + " " + (*it).getNickname() + " " + reason + "\r\n";
			std::string msg1 = ":" + server +  "!" + " KICK " + getName() + " " + (*it).getNickname() + " " + reason + "\n";
			serv.ft_send_all_chan(serv, (*this), msg1);
			serv.sendData((*it).getSocket(), msg);
			_users.erase(it);
			it--;
		}
		i++;
	}
	_stop = 1;

}

void Channel::removeLimitOn()
{
	_limitOn = 0;

}

int	Channel::getLimit()
{
	return (_limit);
}
