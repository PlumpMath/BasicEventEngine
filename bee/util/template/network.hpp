/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_TEMPLATE_NETWORK_H
#define _BEE_UTIL_TEMPLATE_NETWORK_H 1

// Network template-requiring functions

#include <string> // Include the required library headers
#include <algorithm>
#include <sstream>
#include <map>

/*
* network_map_encode() - Return an array of Uint8's which represent a map
* ! Note that the first element in the array is the size of the array, thus the maximum length is 255
* @m: the map to encode
*/
template <typename A, typename B>
Uint8* network_map_encode(std::map<A,B> m) {
	std::stringstream s ("1"); // Create a string stream for temporary encoding
	for (auto& e : m) { // Iterate over the map and put the keys and values into the newline-separated stream
		s << e.first << " = " << e.second << "\n";
	}

	Uint8* d = orda(s.str()); // Convert the string into an array of Uint8's
	d[0] = s.str().length(); // Set the first element of the array to the array size
	return d; // Return the array on success
}
/*
* network_map_decode() - Decode the given array of Uint8's into the given map
* @data: the array to decode
* @mv: the map to store the values in
*/
template <typename A, typename B>
int network_map_decode(Uint8* data, std::map<A,B>* mv) {
	std::map<A,B> m; // Declare a temporary map
	std::string datastr = chra(data); // Convert the array to a string
	if (!datastr.empty()) { // If the string is not empty
		std::istringstream data_stream (datastr); // Create a stream from the string

		while (!data_stream.eof()) { // Operate on each line of the stream
			std::string tmp;
			getline(data_stream, tmp); // Fetch the line from the stream into a temporary string

			if ((tmp.empty())||(tmp[0] == '#')) { // If the line is empty or begins with a comment character, then go to the next
				continue;
			}

			std::string ks = tmp.substr(0, tmp.find(" = ")); // Get the key and value pair
			ks = trim(ks);
			std::string vs = tmp.substr(tmp.find(" = ")+3);
			vs = trim(vs);

			std::istringstream k (ks), v (vs);
			A key;
			B value;
			k >> key;
			v >> value;

			m.emplace(key, value); // Add the pair to the map
		}

	}
	*mv = m; // Set the given map to the temporary map on success
	return 0; // Return 0 on success
}

#endif // _BEE_UTIL_TEMPLATE_NETWORK_H
