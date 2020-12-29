#pragma once
#include <string>
namespace RT
{
	inline std::string getBaseDir(const std::string& str)
    {
        size_t found = str.find_last_of("/\\");
        return str.substr(0, found);
    }

	inline std::string getFileName(const std::string& str)
	{
        std::string out = str;
		// Remove directory if present.
		// Do this before extension removal incase directory has a period character.
        const size_t last_slash_idx = out.find_last_of("\\/");
        if (std::string::npos != last_slash_idx)
        {
            out.erase(0, last_slash_idx + 1);
        }

        // Remove extension if present.
        const size_t period_idx = out.rfind('.');
        if (std::string::npos != period_idx)
        {
            out.erase(period_idx);
        }
        return out;
	}
}
