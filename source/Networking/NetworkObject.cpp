#include "NetworkObject.h"
#include "Connection.h"
#include "Server.h"
#include "Client.h"

NetworkObject::NetworkObject()
{
	//SetNetworkIDManager(Connection::getNetworkIDManager());
	//if(Client::getInstance()) Client::getInstance()->addNetworkObject(this);
}

NetworkObject::~NetworkObject()
{
	//Connection::getInstance()->removeNetworkObject(this);
}