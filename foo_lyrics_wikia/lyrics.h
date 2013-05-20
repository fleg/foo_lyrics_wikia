#define CURLPP_STATICLIB
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <sstream>
#include <string>


class lyrics {
public:
	static std::string getLyrics(const std::string &artist, const std::string &song) {
		curlpp::Cleanup cleanup;
		curlpp::Easy req;
		std::ostringstream res;

		try {
			req.setOpt(new curlpp::options::Url(std::string("http://lyrics.wikia.com/api.php?artist=") + curlpp::escape(artist) + "&song=" + curlpp::escape(song) + "&fmt=xml"));
			req.setOpt(new curlpp::options::WriteStream(&res));
			req.perform();


			std::string lyricsUrl =  getTagValue("<url>", "</url>", res.str());
			if(lyricsUrl.empty()) {
				return "can't get lyrics URL";
			}
			res.clear();
			req.setOpt(new curlpp::options::Url(lyricsUrl + "?action=edit"));
			req.perform();

			std::string lyrics = getTagValue("&lt;lyrics>", "&lt;/lyrics>", res.str());
			if(lyrics.empty()) {
				return "not found";
			}

			return lyrics;
		}
		catch(const std::exception &e) {
			return e.what();
		}
	}

private:
	static std::string getTagValue(const std::string &begin, std::string end, const std::string &data) {
		size_t pos0 = data.find(begin);
		size_t pos1 = data.find(end);
		if(pos0 == std::string::npos || pos1 == std::string::npos) {
			return "";
		}
		return data.substr(pos0 + begin.size(), pos1 - pos0 - begin.size()); 
	}
};