#include "AboutRequest.h"
#include "socket.h"
#include "sstream"

BEGIN_REMOTE_CLASS(AboutRequest)

	REQ_FUNCTION(AboutRequest,getMapInfo); // 
	REQ_FUNCTION(AboutRequest,reqLogin); // ×¢²áµÇÂ¼
	REQ_FUNCTION(AboutRequest,retMapInfo); //

END_REMOTE_CLASS