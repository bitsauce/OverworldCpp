#ifndef TIME_OF_DAY_H
#define TIME_OF_DAY_H

#include "Config.h"
#include "Entities/Entity.h"

class World;

class TimeOfDay : public Entity
{
public:
	TimeOfDay(World *world);

	void setTime(const float time)
	{
		this->time = time;
	}

	float getTime() const
	{
		return time;
	}
	
	int getHour() const
	{
		return int(time/60.0f);
	}
	
	int getMinute() const
	{
		return int(time-getHour()*60.0f);
	}
	
	bool isDay() const
	{
		int hour = getHour();
		return hour >= 6 && hour < 18;
	}
	
	bool isNight() const
	{
		return !isDay();
	}

	void onTick(TickEvent *e)
	{
		// Apply time
		time += e->getDelta();

		// Make sure time loops around to 00:00
		if(time >= 1440.0f)
		{
			time = 0.0f;
		}

		// Make sure time loops back to 24:59
		if(time < 0.0f)
		{
			time = 1440.0f;
		}
	}

private:
	float time;
};

#endif // TIME_OF_DAY_H