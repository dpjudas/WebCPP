#pragma once

#include <functional>
#include <string>

namespace web
{
	class Permissions
	{
	public:
		/*
		const permissions = [
			"accelerometer",
				"accessibility-events",
				"ambient-light-sensor",
				"background-sync",
				"camera",
				"clipboard-read",
				"clipboard-write",
				"geolocation",
				"gyroscope",
				"local-fonts",
				"magnetometer",
				"microphone",
				"midi",
				"notifications",
				"payment-handler",
				"persistent-storage",
				"push",
				"screen-wake-lock",
				"storage-access",
				"top-level-storage-access",
				"window-management",
		];
		*/

		// returns the state
		static void query(const std::string& permission, std::function<void(const std::string& state)> onThen);
	};
}
