#include <platform-config.h>
#include <trace/trace.h>
#include "clockwork.h"
#include "ttime.h"

TTime::TTime()
 : mTHours(0),
   mTMinutes(0),
   mTSeconds(0),
   mTMilliseconds(0)
{
}

TTime::~TTime()
{
}

TTime TTime::addSecs(short seconds) const
{
	TTime time;

	time.setMds(getMds() + seconds * 1000);

	return time;
}

// static
TTime TTime::currentTime()
{
	return Clockwork::instance().currentTime();
}

void TTime::getDiffinMSec(const TTime & time, long & timeDiff) const
{
	timeDiff = 0;

	timeDiff += (mTHours - time.hour()) * 3600000;
	timeDiff += (mTMinutes - time.minute()) * 60000;
	timeDiff += (mTSeconds - time.second()) * 1000;
	timeDiff += mTMilliseconds - time.msec();
}

void TTime::getDiffinSec(const TTime & time, long & timeDiff) const
{
	timeDiff = 0;

	timeDiff += (mTHours - time.hour()) * 3600;
	timeDiff += (mTMinutes - time.minute()) * 60;
	timeDiff += (mTSeconds - time.second());
}

void TTime::getHMS(unsigned char & hours, unsigned char & minutes, unsigned char & seconds, unsigned short & milliseconds) const
{
	hours = mTHours;
	minutes = mTMinutes;
	seconds = mTSeconds;
	milliseconds = mTMilliseconds;
}

unsigned long TTime::getMds() const
{
	unsigned long mds;

	mds = (unsigned long)mTHours * 3600000;
	mds += (unsigned long)mTMinutes * 60000;
	mds += (unsigned long)mTSeconds * 1000;
	mds += (unsigned long)mTMilliseconds;

	return mds;
}

bool TTime::isNull() const
{
	return (mTHours == 0 &&
			mTMinutes == 0 &&
			mTSeconds == 0 &&
			mTMilliseconds == 0) ? true : false;
}

bool TTime::operator <(const TTime & time) const
{
	if (mTHours == time.mTHours)
	{
		if (mTMinutes == time.mTMinutes)
		{
			if (mTSeconds == time.mTSeconds)
			{
				return (mTMilliseconds < time.mTMilliseconds);
			}
			else
			{
				return (mTSeconds < time.mTSeconds);
			}
		}
		else
		{
			return (mTMinutes < time.mTMinutes);
		}
	}
	else
	{
		return (mTHours < time.mTHours);
	}

// Does not work with 23:00:00 < 00:05:00
/*	return (mTHours < time.mTHours) ||
	(!(mTHours < time.mTHours) && mTMinutes < time.mTMinutes) ||
	(!(mTHours < time.mTHours) && !(mTMinutes < time.mTMinutes) && mTSeconds < time.mTSeconds) ||
	(!(mTHours < time.mTHours) && !(mTMinutes < time.mTMinutes) && !(mTSeconds < time.mTSeconds) && mTMilliseconds < time.mTMilliseconds);
*/
}

bool TTime::operator >=(const TTime & time) const
{
	return !(time < *this);
}

bool TTime::operator ==(const TTime & time) const
{
	return (mTHours == time.mTHours &&
			mTMinutes == time.mTMinutes &&
			mTSeconds == time.mTSeconds &&
			mTMilliseconds == time.mTMilliseconds);
}

bool TTime::operator !=(const TTime & time) const
{
	return !(*this == time);
}

const TTime & TTime::operator =(const TTime & t)
{
	mTHours = t.mTHours;
	mTMinutes = t.mTMinutes;
	mTSeconds = t.mTSeconds;
	mTMilliseconds = t.mTMilliseconds;

	return *this;
}

void TTime::setHMS(uint8_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds)
{
	if (hours < 24 &&
		minutes < 60 &&
		seconds < 60 &&
		milliseconds < 1000)
	{
		mTHours = hours;
		mTMinutes = minutes;
		mTSeconds = seconds;
		mTMilliseconds = milliseconds;
	}
	else
	{
//		Trace::outln("TTime: Received bad formatted time");
	}
}

void TTime::setMds(unsigned long mds)
{
	// Trim mds down to a valid value (between 0 and MDS_MAX-1)
	mds %= MDS_MAX;

	mTHours = mds / 3600000;
	mTHours %= 24;
	mds %= 3600000;

	mTMinutes = mds /60000;
	mds %= 60000;

	mTSeconds = mds / 1000;

	mTMilliseconds = mds % 1000;

	// Trace::outln("setMds: %02d:%02d:%02d.%03d",
	//	  mTHours,
	//	  mTMinutes,
	//	  mTSeconds,
	//	  mTMilliseconds);
}
