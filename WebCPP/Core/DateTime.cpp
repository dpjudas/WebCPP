
#include "DateTime.h"
#include <stdexcept>

DateTime::DateTime() : handle(JSValue::global("Date").new_())
{
}

DateTime::DateTime(const DateTime& datetime) : handle(JSValue::global("Date").new_(datetime.getTime()))
{
}

DateTime::DateTime(int64_t value) : handle(JSValue::global("Date").new_((double)value))
{
}

static std::string checkForUTC(const std::string& dateString)
{
	if (!dateString.empty() && dateString.back() != 'Z')
		return dateString + "Z";
	else
		return dateString;
}

DateTime::DateTime(const std::string& dateString, Timezone timezone) : handle(JSValue::global("Date").new_(timezone == Timezone::utc ? checkForUTC(dateString) : dateString))
{
}

DateTime::DateTime(int year, int month) : handle(JSValue::global("Date").new_(year, month - 1))
{
}

DateTime::DateTime(int year, int month, int day) : handle(JSValue::global("Date").new_(year, month - 1, day))
{
}

DateTime::DateTime(int year, int month, int day, int hours) : handle(JSValue::global("Date").new_(year, month - 1, day, hours))
{
}

DateTime::DateTime(int year, int month, int day, int hours, int minutes) : handle(JSValue::global("Date").new_(year, month - 1, day, hours, minutes))
{
}

DateTime::DateTime(int year, int month, int day, int hours, int minutes, int seconds) : handle(JSValue::global("Date").new_(year, month - 1, day, hours, minutes, seconds))
{
}

DateTime::DateTime(int year, int month, int day, int hours, int minutes, int seconds, int milliseconds) : handle(JSValue::global("Date").new_(year, month - 1, day, hours, minutes, seconds, milliseconds))
{
}

DateTime DateTime::now()
{
	return DateTime();
}

int64_t DateTime::getTime() const
{
	return (int64_t)handle.call<double>("getTime");
}

void DateTime::setTime(int64_t value)
{
	handle.call<void>("setTime", (double)value);
}

int DateTime::getYear() const
{
	return handle.call<int>("getFullYear");
}

int DateTime::getMonth() const
{
	return handle.call<int>("getMonth") + 1;
}

int DateTime::getDay() const
{
	return handle.call<int>("getDate");
}

int DateTime::getDayOfWeek() const
{
	return handle.call<int>("getDay");
}

int DateTime::getHours() const
{
	return handle.call<int>("getHours");
}

int DateTime::getSeconds() const
{
	return handle.call<int>("getSeconds");
}

int DateTime::getMinutes() const
{
	return handle.call<int>("getMinutes");
}

int DateTime::getMilliseconds() const
{
	return handle.call<int>("getMilliseconds");
}

int DateTime::getTimezoneOffset() const
{
	return handle.call<int>("getTimezoneOffset");
}

int DateTime::getUTCYear() const
{
	return handle.call<int>("getUTCFullYear");
}

int DateTime::getUTCMonth() const
{
	return handle.call<int>("getUTCMonth") + 1;
}

int DateTime::getUTCDay() const
{
	return handle.call<int>("getUTCDate");
}

int DateTime::getUTCDayOfWeek() const
{
	return handle.call<int>("getUTCDay");
}

int DateTime::getUTCHours() const
{
	return handle.call<int>("getUTCHours");
}

int DateTime::getUTCMinutes() const
{
	return handle.call<int>("getUTCMinutes");
}

int DateTime::getUTCSeconds() const
{
	return handle.call<int>("getUTCSeconds");
}

int DateTime::getUTCMilliseconds() const
{
	return handle.call<int>("getUTCMilliseconds");
}

void DateTime::setYear(int year)
{
	handle.call<void>("setFullYear", year);
}

void DateTime::setMonth(int month)
{
	handle.call<void>("setMonth", month - 1);
}

void DateTime::setDay(int date)
{
	handle.call<void>("setDate", date);
}

void DateTime::setHours(int hours)
{
	handle.call<void>("setHours", hours);
}

void DateTime::setSeconds(int seconds)
{
	handle.call<void>("setSeconds", seconds);
}

void DateTime::setMinutes(int minutes)
{
	handle.call<void>("setMinutes", minutes);
}

void DateTime::setMilliseconds(int milliseconds)
{
	handle.call<void>("setMilliseconds", milliseconds);
}

void DateTime::setUTCYear(int year)
{
	handle.call<void>("setUTCFullYear", year);
}

void DateTime::setUTCMonth(int month)
{
	handle.call<void>("setUTCMonth", month - 1);
}

void DateTime::setUTCDay(int date)
{
	handle.call<void>("setUTCDate", date);
}

void DateTime::setUTCHours(int hours)
{
	handle.call<void>("setUTCHours", hours);
}

void DateTime::setUTCMinutes(int minutes)
{
	handle.call<void>("setUTCMinutes", minutes);
}

void DateTime::setUTCSeconds(int seconds)
{
	handle.call<void>("setUTCSeconds", seconds);
}

void DateTime::setUTCMilliseconds(int milliseconds)
{
	handle.call<void>("setUTCMilliseconds", milliseconds);
}

std::string DateTime::toISOString() const
{
	return handle.call<std::string>("toISOString");
}

std::string DateTime::toDateString() const
{
	return handle.call<std::string>("toDateString");
}

std::string DateTime::toTimeString() const
{
	return handle.call<std::string>("toTimeString");
}

std::string DateTime::toUTCString() const
{
	return handle.call<std::string>("toUTCString");
}

std::string DateTime::toString() const
{
	return handle.call<std::string>("toString");
}

std::string DateTime::toLocaleString() const
{
	return handle.call<std::string>("toLocaleString", JSValue::global("navigator")["language"], JSValue::object());
}

std::string DateTime::toLocaleDateString() const
{
	return handle.call<std::string>("toLocaleDateString", JSValue::global("navigator")["language"], JSValue::object());
}

std::string DateTime::toLocaleTimeString() const
{
	return handle.call<std::string>("toLocaleTimeString", JSValue::global("navigator")["language"], JSValue::object());
}

void DateTime::addMilliSeconds(int milliSeconds)
{
	setTime(getTime() + milliSeconds);
}

void DateTime::addMilliSeconds(int64_t milliSeconds)
{
	setTime(getTime() + milliSeconds);
}

void DateTime::addSeconds(int seconds)
{
	addMilliSeconds(seconds * 1000);
}

void DateTime::addSeconds(int64_t seconds)
{
	addMilliSeconds(seconds * 1000);
}

void DateTime::addMinutes(int minutes)
{
	addSeconds(minutes * 60);
}

void DateTime::addMinutes(int64_t minutes)
{
	addSeconds(minutes * 60);
}

void DateTime::addHours(int hours)
{
	addMinutes(hours * 60);
}

void DateTime::addHours(int64_t hours)
{
	addMinutes(hours * 60);
}

void DateTime::addDays(int days)
{
	addHours((int64_t)days * 24);
}

void DateTime::addMonths(int months)
{
	int year = getYear();
	int month = getMonth();
	int day = getDay();

	int64_t days = -day;
	while (months > 0)
	{
		days += getDaysForMonth(month, year);
		month++;
		if (month == 13)
		{
			month = 1;
			year++;
		}
		months--;
	}

	while (months < 0)
	{
		month--;
		if (month == 0)
		{
			month = 12;
			year--;
		}
		months++;
		days += getDaysForMonth(month, year);
	}

	days += std::min(day, getDaysForMonth(month, year));
	addDays(days);
}

void DateTime::addYears(int years)
{
	addMonths(12 * years);
}

bool DateTime::operator >(const DateTime& other) const
{
	return getTime() > other.getTime();
}

bool DateTime::operator >=(const DateTime& other) const
{
	return getTime() >= other.getTime();
}

bool DateTime::operator <(const DateTime& other) const
{
	return getTime() < other.getTime();
}

bool DateTime::operator <=(const DateTime& other) const
{
	return getTime() <= other.getTime();
}

bool DateTime::operator ==(const DateTime& other) const
{
	return getTime() == other.getTime();
}

bool DateTime::operator !=(const DateTime& other) const
{
	return getTime() != other.getTime();
}

DateTime& DateTime::operator =(const DateTime& src)
{
	setTime(src.getTime());
	return *this;
}

int DateTime::getDaysForMonth(int month, int year)
{
	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	case 2:
		if (isLeapYear(year))
			return 29;
		else
			return 28;
	}
	throw std::runtime_error("Month out of range");
}

bool DateTime::isLeapYear(int year)
{
	if (year % 400 == 0)
		return true;
	else if (year % 100 == 0)
		return false;
	else if (year % 4 == 0)
		return true;
	else
		return false;
}
