#pragma once

typedef enum _MsgType
{
	UNKNOWN,

	// System
	SYS,

	// Request
	REQ,

	// Response
	RSP,

	// Callback
	CALLBK

} MSG_TYPE;