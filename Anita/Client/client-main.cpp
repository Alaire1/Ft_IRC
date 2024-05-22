#include "Client.hpp"

int main (void)
{
	Client ourclient("127.0.0.1", "6667");
	
	if(ourclient.connectServer())
	{
		ourclient.sendMessage("Test01\n");
	}

	return 0;
}

