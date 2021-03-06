/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_REAL
#define BEE_UTIL_REAL 1

// Real number functions

#include <random>
#include <time.h>

#include "real.hpp" // Include the function definitions

/*
* random_internal() - Return a random value seeded by the given seed and bounded by the given min and max
* ! This function should probably never be called directly by the user because there are wrapper functions for it
* ! This function contains two static variables to ensure that the engine stays seeded with the correct value
* @new_seed: the integer to seed the random number engine with
* @minimum: the minimum value to return
* @maximum: the maximum value to return
*/
unsigned int random_internal(unsigned int new_seed, unsigned int minimum, unsigned int maximum) {
	static unsigned int seed = DEFAULT_RANDOM_SEED; // Define the seed for the random number engine if it hasn't been defined yet
	static std::mt19937 engine; // Declare the random number engine if it hasn't been declared yet

	if (new_seed != DEFAULT_RANDOM_SEED) { // If the new given seed is not the default seed then use it to seed the engine
		seed = new_seed;
		engine.seed(seed);
	} else if (seed == DEFAULT_RANDOM_SEED) { // If the current seed is the terrible default seed then seed the engine with the current time
		seed = static_cast<unsigned int>(time(nullptr));
		engine.seed(seed);
	}

	if (minimum == 0) { // If the minimum is the lowest possible
		if (maximum == 0) { // If the maximum is equal to the minimum
			return engine(); // Return any number
		}
		return engine() % maximum; // Return a number below the maximum value
	} else { // If the minimum exists
		if (maximum < minimum) { // If the maximum is less then the minimum, that signals that we should return the seed
			return seed; // Return the static seed
		}
		return (engine() % (maximum - minimum)) + minimum; // Return a number bounded by the given minimum and maximum
	}
}
/*
* random() - Return a random number between 0 and the given number
* ! Note that if the given number is 0 then an unbounded random number will be returned
* @x: the maximum value to return
*/
unsigned int random(int x) {
	return random_internal(DEFAULT_RANDOM_SEED, 0, x);
}
/*
* random_range() - Return a random number between the two given values
* @x1: the minimum value to return
* @x2: the maximum value to return
*/
unsigned int random_range(unsigned int x1, unsigned int x2) {
	return random_internal(DEFAULT_RANDOM_SEED, x1, x2);
}
/*
* random_get_seed() - Return the current seed used in the random number engine
* ! Calling random_internal() with a larger minimum than the maximum signals it to return the seed
*/
unsigned int random_get_seed() {
	return random_internal(DEFAULT_RANDOM_SEED, 1, 0);
}
/*
* random_set_seed() - Set and return the seed used in the random number engine
* @new_seed: The new seed to generate random numbers with
*/
unsigned int random_set_seed(unsigned int new_seed) {
	return random_internal(new_seed, 1, 0);
}
/*
* random_reset_seed() - Set the seed of the random number engine to the current time
*/
unsigned int random_reset_seed() {
	return random_set_seed(static_cast<unsigned int>(time(nullptr)));
}
/*
* randomize() - Set the seed of the random number engine to a random number
*/
unsigned int randomize() {
	return random_set_seed(random_internal(DEFAULT_RANDOM_SEED, 0, 0));
}

/*
* sign() - Return -1, 0, or 1 based on the sign of the given number
* @x: the number to return the sign of
*/
template <typename T>
int sign(T x) {
	if (x > 0) {
		return 1; // Return 1 if the given number is greater than 0
	} else if (x < 0) {
		return -1; // Return -1 if the given number is less than 0
	}
	return 0; // Return 0 if the given number is equal to 0
}
template int sign<int>(int);
template int sign<long>(long);
template int sign<float>(float);
template int sign<double>(double);
#ifdef _WIN32
	template int sign<time_t>(time_t); // On Windows, time_t is __int64 but on Linux, time_t is long
#endif
/*
* sqr() - Return the square of the given number
* @x: the number to square
*/
template <typename T>
T sqr(T x) {
	return x*x; // Supposedly x*x is faster than pow(x, 2) but I doubt that it actually matters
}
template int sqr<int>(int);
template long sqr<long>(long);
template float sqr<float>(float);
template double sqr<double>(double);
/*
* logn() - Return the logarithm of the given number for any given base
* @n: the base of the logarithm
* @x: the number to log
*/
double logn(double n, double x) {
	return log(x)/log(n);
}
/*
* degtorad() - Convert the given number from degrees to radians
* @a: the number to convert
*/
double degtorad(double a) {
	return a*PI/180.0;
}
/*
* radtodeg() - Convert the given number from radians to degrees
* @a: the number to convert
*/
double radtodeg(double a) {
	return a*180.0/PI;
}
/*
* opposite_angle() - Return the angle (in degrees) which is opposite to the given one on the unit circle
* @a: the angle to find the opposite of
*/
double opposite_angle(double a) {
	a = absolute_angle(a); // Make sure that the angle is between 0.0 and 360.0
	if (a < 180.0) { // If the angle is on the top side of the unit circle then return the angle 180 degrees in front of it
		return a+180.0;
	}
	return a-180.0; // Otherwise, return the angle 180 degrees behind it
}
/*
* absolute_angle() - Return the angle absolute to the unit circle
* ! The primary use case is to correctly determine negative angles
* @a: the angle to find
*/
double absolute_angle(double a) {
	a = fmod(a, 360.0); // Make sure that the angle is between -360.0 and 360.0
	if (a < 0) {
		return a+360.0; // Return the equivalent angle if the given one is negative
	}
	return a; // Return the given angle
}

/*
* direction_of() - Return the direction in degrees from (x1, y1) to (x2, y2)
* @x1: the x-coordinate of the first point
* @y1: the y-coordinate of the first point
* @x2: the x-coordinate of the second point
* @y2: the y-coordinate of the second point
*/
double direction_of(double x1, double y1, double x2, double y2) {
	return absolute_angle(radtodeg(atan2(y2-y1, x2-x1))); // Calculate and return the angle of the vector
}
/*
* direction_of() - Return the direction as a unit vector from (x1, y1, z1) to (x2, y2, z2)
* @x1: the x-coordinate of the first point
* @y1: the y-coordinate of the first point
* @z1: the z-coordinate of the first point
* @x2: the x-coordinate of the second point
* @y2: the y-coordinate of the second point
* @z2: the z-coordinate of the second point
*/
btVector3 direction_of(double x1, double y1, double z1, double x2, double y2, double z2) {
	btVector3 v1 = btVector3(
		btScalar(x1),
		btScalar(y1),
		btScalar(z1)
	);
	btVector3 v2 = btVector3(
		btScalar(x2),
		btScalar(y2),
		btScalar(z2)
	);
	return btVector3(
		v2.x()-v1.x(),
		v2.y()-v1.y(),
		v2.z()-v1.z()
	).normalized();
}
/*
* dist_sqr() - Return the square of the distance from (x1, y1, z1) to (x2, y2, z2) in order to avoid costly square roots
* @x1: the x-coordinate of the first point
* @y1: the y-coordinate of the first point
* @z1: the z-coordinate of the first point
* @x2: the x-coordinate of the second point
* @y2: the y-coordinate of the second point
* @z2: the z-coordinate of the second point
*/
double dist_sqr(double x1, double y1, double z1, double x2, double y2, double z2) {
	return sqr(x1-x2) + sqr(y1-y2) + sqr(z1-z2);
}
/*
* dist_sqr() - Return the square of the distance from (x1, y1, 0.0) to (x2, y2, 0.0) in order to avoid costly square roots
* ! When the function is called without the z-coordinates, simply call it with them set to 0.0
*/
double dist_sqr(double x1, double y1, double x2, double y2) {
	return dist_sqr(x1, y1, 0.0, x2, y2, 0.0);
}
/*
* distance() - Return the distance from (x1, y1, z1) to (x2, y2, z2)
* @x1: the x-coordinate of the first point
* @y1: the y-coordinate of the first point
* @z1: the z-coordinate of the first point
* @x2: the x-coordinate of the second point
* @y2: the y-coordinate of the second point
* @z2: the z-coordinate of the second point
*/
double distance(double x1, double y1, double z1, double x2, double y2, double z2) {
	return sqrt(sqr(x1-x2) + sqr(y1-y2) + sqr(z1-z2));
}
/*
* distance() - Return the distance from (x1, y1) to (x2, y2)
* ! When the function is called without the z-coordinates, simply call it with them set to 0.0
*/
double distance(double x1, double y1, double x2, double y2) {
	return distance(x1, y1, 0.0, x2, y2, 0.0);
}
/*
* distance() - Return the distance from (0.0, 0.0, 0.0) to (x1, y1, z1)
* ! When the function is called with only one coordinate, simply call it with the other set to (0.0, 0.0, 0.0)
*/
double distance(double x, double y, double z) {
	return distance(x, y, z, 0.0, 0.0, 0.0);
}
/*
* distance() - Return the distance from (0.0, 0.0, 0.0) to (x1, y1, 0.0)
* ! When the function is called with only a coordinate pair, simply call it with everything else set to 0.0
*/
double distance(double x, double y) {
	return distance(x, y, 0.0, 0.0, 0.0, 0.0);
}
/*
* coord_approach() - Return a pair of coordinates which is closer to (x2, y2) from (x1, y1) by a certain amount
* @x1: the x-coordinate of the original point
* @y1: the y-coordinate of the original point
* @x2: the x-coordinate of the desired destination
* @y2: the y-coordinate of the desired destination
* @speed: the factor by which to move towards the destination
*/
std::pair<double,double> coord_approach(double x1, double y1, double x2, double y2, double speed, double dt) {
	double d = distance(x1, y1, x2, y2);
	if (d <= speed) { // If the distance between the points is less than the speed, simply return the destination point
		return std::make_pair(x2, y2);
	}
 	double ratio = speed/d;

	double x3 = x1 + (x2-x1)*ratio*dt;
	double y3 = y1 + (y2-y1)*ratio*dt;

	return std::make_pair(x3, y3); // Return a point somewhere in between the given points based on the given speed
}
/*
* bt_to_glm3() - Convert the given btVector3 to glm::vec3
* @v: the vector to convert
*/
glm::vec3 bt_to_glm3(const btVector3& v) {
	return glm::vec3(v.x(), v.y(), v.z());
}
/*
* glm_to_bt3() - Convert the given glm::vec3 to btVector3
* @v: the vector to convert
*/
btVector3 glm_to_bt3(const glm::vec3& v) {
	return btVector3(v.x, v.y, v.z);
}

/*
* dot_product() - Return the dot product of two vectors given as individual elements
* @x1: the x-element of the first vector
* @y1: the y-element of the first vector
* @x2: the x-element of the second vector
* @y2: the y-element of the second vector
*/
template <typename T>
T dot_product(T x1, T y1, T x2, T y2) {
	return x1*x2 + y1*y2;
}
template int dot_product<int>(int, int, int, int);
template long dot_product<long>(long, long, long, long);
template double dot_product<double>(double, double, double, double);
/*
* dot_product() - Return the dot product of two vectors given as pairs of elements
* @p1: the first vector
* @p2: the second vector
*/
template <typename T>
T dot_product(const std::pair<T,T>& p1, const std::pair<T,T>& p2) {
	return dot_product(p1.first, p1.second, p2.first, p2.second);
}
template int dot_product<int>(const std::pair<int,int>&, const std::pair<int,int>&);
template long dot_product<long>(const std::pair<long,long>&, const std::pair<long,long>&);
template float dot_product<float>(const std::pair<float,float>&, const std::pair<float,float>&);
template double dot_product<double>(const std::pair<double,double>&, const std::pair<double,double>&);

/*
* is_between() - Return whether the given number is between or equal to the given bounds
* @x: the number to check
* @a: one of the bounds
* @b: the other bound
*/
template <typename T>
bool is_between(T x, T a, T b) {
	if (a < b) { // Handle the case where the minimum is less than the maximum
		if ((x >= a)&&(x <= b)) {
			return true; // Return true if x is in between the given bounds
		}
		return false;
	} else if (a == b) {
		if (x == a) {
			return true;
		}
		return false;
	} else {
		return is_between<T>(x, b, a);
	}
}
template bool is_between<int>(int, int, int);
template bool is_between<long>(long, long, long);
template bool is_between<float>(float, float, float);
template bool is_between<double>(double, double, double);
/*
* is_angle_between() - Return whether the given angle (in degrees) is between or equal to the given bounds
* @x: the angle to check
* @a: one of the bounds
* @b: the other bound
*/
template <typename T>
bool is_angle_between(T x, T a, T b) {
	x = static_cast<T>(absolute_angle(x)); // Make sure the angle is between 0.0 and 360.0 degrees
	if (a < b) { // If the bounds are normal
		return is_between(x, a, b);
	} else { // If the bounds are at the top of the unit circle e.g. from 315 to 45
		return is_between(x, a, static_cast<T>(360)) || is_between(x, static_cast<T>(0), b);
	}
}
template bool is_angle_between<int>(int, int, int);
template bool is_angle_between<long>(long, long, long);
template bool is_angle_between<float>(float, float, float);
template bool is_angle_between<double>(double, double, double);
/*
* fit_bounds() - Return a value in between the given bounds, either x or one of the bounds
* @x: the number to check whether it is between the bounds
* @a: one of the bounds
* @b: the other bound
*/
template <typename T>
T fit_bounds(T x, T a, T b) {
	if (a <= b) { // Handle the case where the minimum is less than the maximum
		if (x < a) { // If x is less than the minimum then return the minimum
			return a;
		}
		if (x > b) { // If x is greater than the maximum then return the maximum
			return b;
		}
		return x; // Otherwise, return x
	} else {
		return fit_bounds(x, b, a);
	}
}
template int fit_bounds<int>(int, int, int);
template long fit_bounds<long>(long, long, long);
template float fit_bounds<float>(float, float, float);
template double fit_bounds<double>(double, double, double);
/*
* qmod() - Return the result of a "quick" modulo
* ! This is used to avoid unnecessary floating point divisions
* ! This is effectively: x - truncated(x/m) * m
* @x: the quotient numerator
* @m: the quotient denominator
*/
template <typename T>
T qmod(T x, unsigned int m) {
	if ((m == 0)||(m == 1)) {
		return m;
	}

	if (x < 0) { // Handle negative modulo
		while (x < 0) {
			x += m;
		}
		return x;
	}

	while (x >= 0) {
		x -= m;
	}
	return x+m;
}
template int qmod<int>(int, unsigned int);
template long qmod<long>(long, unsigned int);
template float qmod<float>(float, unsigned int);
template double qmod<double>(double, unsigned int);

/*
* checksum_internal_table() - Return a value from the CRC lookup table
* @index: the index of the value to return
*/
unsigned int checksum_internal_table(size_t index) {
	static std::vector<unsigned int> table;

	static bool is_initialized = false;
	if (!is_initialized) {
		unsigned int polynomial = 0x04C11DB7; // Use the official polynomial used by most implementations

		table.resize(256);
		for (unsigned int i=0; i<256; ++i) {
			table[i] = checksum_internal_reflect(i, 8) << 24;

			for (unsigned int j=0; j<8; ++j) {
				table[i] =
					(table[i] << 1)
					^ (
						(table[i] & (1 << 31)) ? polynomial : 0
					);
			}

			table[i] = checksum_internal_reflect(table[i], 32);
		}

		is_initialized = true;
	}

	return table[index];
}
/*
* checksum_internal_reflect() - Reflect the CRC table value to conform to the CRC standard
* @reflect: the value to be reflected
* @bits: the number of bits to reflect
*/
unsigned int checksum_internal_reflect(unsigned int reflect, const char bits) {
	unsigned int value = 0;

	for (int i=0; i<(bits+1); ++i) { // Swap bits
		if (reflect & 1) {
			value |= (1 << (bits-i));
		}
		reflect >>= 1;
	}

	return value;
}
/*
* get_checksum() - Return the CRC32 checksum for the given data
* @data: the data vector to generate a checksum for
*/
unsigned int get_checksum(const std::vector<unsigned char>& data) {
	unsigned int crc = 0xffffffff; // Initialize the checksum

	for (auto& d : data) {
		crc = (crc >> 8) ^ checksum_internal_table((crc & 0xff) ^ d);
	}

	return (crc ^ 0xffffffff); // Finalize and return the checksum
}
/*
* verify_checksum() - Return whether the data matches the checksum
* @data: the data to check
* @crc: the checksum to verify against
*/
bool verify_checksum(const std::vector<unsigned char>& data, unsigned int crc) {
	return (get_checksum(data) == crc);
}

#endif // BEE_UTIL_REAL
