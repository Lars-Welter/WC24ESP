#pragma once

// Position of the sun for the brightness that follows sunrise and sunset.
// The SP803E has no light sensor, so the clock dims by the sun instead.

#include <math.h>
#include <stdint.h>

namespace wc24h {

constexpr double SUN_DEG = M_PI / 180.0;

// Sun elevation above the horizon in degrees for a UTC unix time. Low
// precision formulas of the Astronomical Almanac, good to about 0.1 degrees.
inline double sunElevation(int64_t utc, double latitude, double longitude) {
    const double days = (utc - 946728000LL) / 86400.0; // since J2000.0
    const double anomaly = fmod(357.529 + 0.98560028 * days, 360.0) * SUN_DEG;
    const double meanLongitude = fmod(280.459 + 0.98564736 * days, 360.0);
    const double eclipticLongitude =
        (meanLongitude + 1.915 * sin(anomaly) + 0.020 * sin(2 * anomaly)) *
        SUN_DEG;
    const double obliquity = (23.439 - 0.00000036 * days) * SUN_DEG;
    const double rightAscension =
        atan2(cos(obliquity) * sin(eclipticLongitude), cos(eclipticLongitude));
    const double declination = asin(sin(obliquity) * sin(eclipticLongitude));
    const double siderealDeg =
        fmod(280.46061837 + 360.98564736629 * days, 360.0);
    const double hourAngle =
        (siderealDeg + longitude) * SUN_DEG - rightAscension;
    const double lat = latitude * SUN_DEG;
    return asin(sin(lat) * sin(declination) +
                cos(lat) * cos(declination) * cos(hourAngle)) /
           SUN_DEG;
}

// Brightness in percent: night until the sun is 6 degrees below the horizon
// (end of civil twilight), day from 6 degrees above, a smooth ramp between.
// In Germany the ramp takes about 70 minutes around sunrise and sunset.
inline uint8_t sunBrightness(double elevation, uint8_t day, uint8_t night) {
    double share = (elevation + 6.0) / 12.0;
    if (share < 0) {
        share = 0;
    } else if (share > 1) {
        share = 1;
    }
    share = share * share * (3 - 2 * share); // smoothstep
    return static_cast<uint8_t>(lround(night + (day - night) * share));
}

// Sunrise and sunset (upper limb at the horizon, -0.833 degrees) within the
// 24 hours after dayStart. -1 when the sun does not rise or set that day.
inline void sunTimes(int64_t dayStart, double latitude, double longitude,
                     int64_t &sunrise, int64_t &sunset) {
    constexpr double HORIZON = -0.833;
    constexpr int64_t STEP = 600;
    sunrise = sunset = -1;
    double previous = sunElevation(dayStart, latitude, longitude) - HORIZON;
    for (int64_t t = dayStart + STEP; t <= dayStart + 86400; t += STEP) {
        const double now = sunElevation(t, latitude, longitude) - HORIZON;
        if ((previous < 0) != (now < 0)) {
            // the elevation is nearly linear over ten minutes
            const int64_t crossing =
                t - STEP + static_cast<int64_t>(STEP * previous /
                                                (previous - now));
            if (now >= 0 && sunrise < 0) {
                sunrise = crossing;
            } else if (now < 0 && sunset < 0) {
                sunset = crossing;
            }
        }
        previous = now;
    }
}

} // namespace wc24h
