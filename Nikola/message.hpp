#ifndef MESSAGE_HPP
# define MESSAGE_HPP
# include <iostream>

class Message
{
	private:
	Message ();
	~Message ();
	Message (Message const &src);
	Message &operator=(Message const &src);
	Message (std::string const &message_text, std::string const &message_author);

	public:
	std::string _message_text;
	std::string _message_timestamp;
	std::string _message_author;
	int _message_type;

	
};

#endif