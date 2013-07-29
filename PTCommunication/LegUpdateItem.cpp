#include "StdAfx.h"
#include "LegUpdateItem.h"

namespace PTEntity {

LegUpdateItem::LegUpdateItem(const entity::LegUpdateItem* pEntity)
{
	_symbol = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->symbol().c_str());
	_status = static_cast<LegStatus>(pEntity->status());
	_last = pEntity->last();
	_ask = pEntity->ask();
	_askSize = pEntity->asksize();
	_bid = pEntity->bid();
	_bidSize = pEntity->bidsize();
}

}
