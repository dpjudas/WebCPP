#pragma once

#include "JSValue.h"
#include <cstdint>

namespace web
{
	enum class Timezone
	{
		utc,
		local
	};

	class DateTime
	{
	public:
		DateTime();
		DateTime(int64_t value);
		DateTime(const std::string& dateString, Timezone = Timezone::utc);
		DateTime(int year, int month);
		DateTime(int year, int month, int day);
		DateTime(int year, int month, int day, int hours);
		DateTime(int year, int month, int day, int hours, int minutes);
		DateTime(int year, int month, int day, int hours, int minutes, int seconds);
		DateTime(int year, int month, int day, int hours, int minutes, int seconds, int milliseconds);

		static DateTime now();

		int64_t getTime() const;
		void setTime(int64_t value);

		int getYear() const;
		int getMonth() const;
		int getDay() const;
		int getDayOfWeek() const;
		int getHours() const;
		int getSeconds() const;
		int getMinutes() const;
		int getMilliseconds() const;
		int getTimezoneOffset() const;

		int getUTCYear() const;
		int getUTCMonth() const;
		int getUTCDay() const;
		int getUTCDayOfWeek() const;
		int getUTCHours() const;
		int getUTCMinutes() const;
		int getUTCSeconds() const;
		int getUTCMilliseconds() const;

		void setYear(int year);
		void setMonth(int month);
		void setDay(int date);
		void setHours(int hours);
		void setSeconds(int seconds);
		void setMinutes(int minutes);
		void setMilliseconds(int milliseconds);

		void setUTCYear(int year);
		void setUTCMonth(int month);
		void setUTCDay(int date);
		void setUTCHours(int hours);
		void setUTCMinutes(int minutes);
		void setUTCSeconds(int seconds);
		void setUTCMilliseconds(int milliseconds);

		std::string toISOString() const;

		std::string toDateString() const;
		std::string toTimeString() const;
		std::string toUTCString() const;
		std::string toString() const;

		std::string toLocaleString() const;
		std::string toLocaleDateString() const;
		std::string toLocaleTimeString() const;

		void addMilliSeconds(int milliSeconds);
		void addMilliSeconds(int64_t milliSeconds);
		void addSeconds(int seconds);
		void addSeconds(int64_t seconds);
		void addMinutes(int minutes);
		void addMinutes(int64_t minutes);
		void addHours(int hours);
		void addHours(int64_t hours);
		void addDays(int days);
		void addMonths(int months);
		void addYears(int years);

		bool operator >(const DateTime& other) const;
		bool operator >=(const DateTime& other) const;
		bool operator <(const DateTime& other) const;
		bool operator <=(const DateTime& other) const;
		bool operator ==(const DateTime& other) const;
		bool operator !=(const DateTime& other) const;

		DateTime(const DateTime& datetime);
		DateTime& operator =(const DateTime& src);

		DateTime(DateTime&& datetime) = default;
		DateTime& operator =(DateTime&& src) = default;

	private:
		static int getDaysForMonth(int month, int year);
		static bool isLeapYear(int year);

		JSValue handle;
	};
}
