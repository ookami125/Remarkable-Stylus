#pragma once
#include <ctype.h>
#include <string>
#include <vector>

inline unsigned constexpr const_hash(char const* input) {
	return *input ?
		static_cast<unsigned int>(*input) + 33 * const_hash(input + 1) :
		5381;
}

inline unsigned constexpr operator "" _h(char const* input) {
	return const_hash(input);
}

inline unsigned hash(char const* input) {
	return *input ?
		static_cast<unsigned int>(*input) + 33 * const_hash(input + 1) :
		5381;
}

inline unsigned hash_lower(char const* input) {
	return *input ?
		static_cast<unsigned int>(tolower(*input)) + 33 * const_hash(input + 1) :
		5381;
}

template<typename T>
inline bool toNumber(T& ret, const char* arg, int base = 10)
{
	for (const char* c = arg; *c != '\0'; c++) {
		if (isdigit(*c) == 0) return false;
		ret = (ret * 10) + (*c - '0');
	}
	return true;
}

inline bool isTruthy(char* arg, bool* ret)
{
	switch (hash_lower(arg))
	{
	case const_hash("true"):
	case const_hash("1"):
		*ret = true;
		return true;

	case const_hash("false"):
	case const_hash("0"):
		*ret = false;
		return true;
	}

	return false;
}

inline std::vector<std::string> splitString(const char* arg, const std::string delimiter)
{
	std::string s = arg;
	std::vector<std::string> output = {};
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		output.push_back(token);
		s.erase(0, pos + delimiter.length());
	};
	output.push_back(s);
	return output;
}

enum Orientation
{
	Landscape,
	Portrait,
	LandscapeFlipped,
	PortraitFlipped
};

struct Args
{
	std::string hostname = "10.11.99.1";
	std::string username = "root";
	std::string password = "";
	Orientation orientation = Landscape;
	int32_t xSize = 0;
	int32_t ySize = 0;
	int32_t xOffset = 0;
	int32_t yOffset = 0;

};

inline const char* ParseArgs(Args& output, int argc, char** argv)
{
	for (int i = 1; i < argc;)
	{
		switch (hash(argv[i]))
		{
		case const_hash("-h"):
		case const_hash("--hostname"): {
			output.hostname = argv[i + 1];
			i += 2;
		} break;
		case const_hash("-u"):
		case const_hash("--username"): {
			output.username = argv[i + 1];
			i += 2;
		} break;
		case const_hash("-p"):
		case const_hash("--password"): {
			output.password = argv[i + 1];
			i += 2;
		} break;
		case const_hash("-s"):
		case const_hash("--scale"): {
			auto points = splitString(argv[i + 1], ":");
			auto tl = splitString(points[0].c_str(), ",");
			auto br = splitString(points[1].c_str(), ",");
			toNumber(output.xOffset, tl[0].c_str());
			toNumber(output.yOffset, tl[1].c_str());
			toNumber(output.xSize, br[0].c_str());
			toNumber(output.ySize, br[1].c_str());
			output.xSize -= output.xOffset;
			output.ySize -= output.yOffset;
			i += 2;
		} break;
		case const_hash("-o"):
		case const_hash("--orientation"): {
			std::string arg = argv[i + 1];
			unsigned harg = hash(argv[i + 1]);
			output.orientation =
				(harg == const_hash("Landscape")) ? Landscape :
				(harg == const_hash("Portrait")) ? Portrait :
				(harg == const_hash("LandscapeFlipped")) ? LandscapeFlipped :
				(harg == const_hash("PortraitFlipped")) ? PortraitFlipped :
				Orientation(atoi(arg.c_str()));
			i += 2;
		}
		}
	}
	return nullptr;
}