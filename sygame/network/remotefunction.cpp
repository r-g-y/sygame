#include "remotefunction.h"
#include "socket.h"

void RemoteFunction::doSendObject(Socket *socket,CmdObject *object)
{
	if (socket) socket->sendObject(object);
}
void RemoteFunction::doSendPack(Socket *socket,void* cmd,unsigned len,eRemoteCallType tag)
{
	if (localDelegate && socket && (socket->useLocalServer || tag == LOCAL_CALL_FUNCTION) )
	{
		localDelegate->doLocalCmd(socket,cmd,len);
	}
	else if (socket) socket->sendPacket(cmd,len);
}