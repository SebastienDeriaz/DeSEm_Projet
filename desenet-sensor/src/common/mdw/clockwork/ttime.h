#pragma once

#include <stdint.h>

class Clockwork;

/**
 * @brief Holds time information (hours, minutes, seconds, milliseconds).
 */
class TTime
{
public:

	static const uint32_t MDS_MAX = 86400000;	// 1000 * 60 * 60 * 24 = 86400000

	TTime();
	~TTime();

	TTime addSecs(short seconds) const;			///< Adds seconds to stored time.
	static TTime currentTime();					///< Returns actual time (delegates to clockwork).

	/**
	 * @brief Returns time difference in millisecond.
	 * Value is positive if stored time is bigger then the time given by parameter, otherwise zero or negative.
	 */
	void getDiffinMSec(const TTime & time, long & timeDiff) const;
	/**
	 * @brief Returns time difference in seconds (Milliseconds are ignored).
	 * Value is positive if stored time is bigger then the time given by parameter, otherwise zero or negative.
	 */
	void getDiffinSec(const TTime & time, long & timeDiff) const;

	void getHMS(unsigned char & hours,
				unsigned char & minutes,
				unsigned char & seconds,
				unsigned short & milliseconds) const;	///< Returns stored time as part of hours, minutes, seconds and milliseconds.

	unsigned long getMds() const;						///< Returns stored time in milliseconds.
	bool isNull() const;

	inline unsigned char hour() const					///< Returns hour part of the stored time.
	{ return mTHours; }
	inline unsigned char minute() const					///< Returns minute part of the stored time.
	{ return mTMinutes; }
	inline unsigned char second() const					///< Returns second part of the stored time.
	{ return mTSeconds; }
	inline unsigned short msec() const					///< Returns millisecond part of the stored time.
	{ return mTMilliseconds; }

	bool operator <(const TTime & time) const;			///< Returns true if this time is earlier than time given by parameter, otherwise returns false.
	bool operator >=(const TTime & time) const;			///< Returns true if this time is equal or greater than time given by parameter, otherwise returns false.
	bool operator ==(const TTime & time) const;			///< Checks if the two objects are equal.
	bool operator !=(const TTime & time) const;			///< Checks if the two objects are not equal.
	const TTime & operator =(const TTime & t);			///< Time assignment.

	void setHMS(uint8_t hours,
				uint8_t minutes,
				uint8_t seconds,
				uint16_t milliseconds = 0);				///< Sets time using given parameters.
	void setMds(unsigned long mds);						///< Sets time in milliseconds.

protected:
	uint8_t mTHours;
	uint8_t mTMinutes;
	uint8_t mTSeconds;
	uint16_t mTMilliseconds;
};
