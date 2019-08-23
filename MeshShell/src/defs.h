#pragma once

#ifdef _WIN64
	constexpr auto DEFAULT_DATA_DIR = "D:\\data";
#elif __linux
	constexpr auto DEFAULT_DATA_DIR = "/usr/local";
#endif
