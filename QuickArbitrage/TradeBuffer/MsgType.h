#pragma once

typedef enum _MsgType
{
	// System
	SYS_HEARTBEAT,
	SYS_HEARTBEAT_ACK,
	
	// Request
	REQ_LOGIN,

	REQ_SUBSCRIBE,
	REQ_UNSUBSCRIBE,

	// Response
	RSP_LOGIN,

	RSP_SUBSCRIBE,
	RSP_UNSUBSCRIBE,

	// Quote
	QUOTE

} MSG_TYPE;