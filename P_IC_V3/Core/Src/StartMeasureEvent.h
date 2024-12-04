#ifndef EVENT_H
#define EVENT_H

#include "Event.h"

class StartMeasureEvent: public Event
{
public:
	void handleEvent() override;
	virtual ~StartMeasureEvent();
};

#endif
