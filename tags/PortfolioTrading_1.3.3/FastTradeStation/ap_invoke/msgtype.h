#pragma once

typedef enum _MsgType
{
	UNKNOWN,

	// Connect
	CONN,

	// Connect Ack
	CONN_ACK,

	// Request
	REQ,

	// Response
	RSP,

	// Callback Req
	CALLBK_REQ,

	// Callback Rsp
	CALLBK_RSP,

	// Heartbeat
	HEARTBEAT

} MSG_TYPE;