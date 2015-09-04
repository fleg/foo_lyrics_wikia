#define CURLPP_STATICLIB
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <sstream>
#include <string>


class lyrics {
public:
	static std::string getLyrics(const std::string &artist, const std::string &song) {
		try {
			std::string lyricsXml = httpGet("http://lyrics.wikia.com/api.php?action=lyrics&artist=" + curlpp::escape(artist) + "&song=" + curlpp::escape(song) + "&fmt=xml");
			std::string lyricsUrl =  getTagValue("<url>", "</url>", lyricsXml);
			
			if(lyricsUrl.empty()) {
				return "can't get lyrics URL";
			}
			
			std::string lyricsPage = httpGet(normalizeUrl(lyricsUrl) + "?action=edit");
			std::string lyrics = getTagValue("&lt;lyrics>", "&lt;/lyrics>", lyricsPage);
			
			if(lyrics.empty()) {
				return "not found";
			}

			return trim(lyrics);
		}
		catch(const std::exception &e) {
			return e.what();
		}
	}

private:
	static std::string httpGet(const std::string &url) {
		curlpp::Cleanup cleanup;
		curlpp::Easy req;
		std::ostringstream res;

		req.setOpt(new curlpp::options::Url(url));
		req.setOpt(new curlpp::options::UserAgent(std::string("Mozilla/5.0 (Windows NT 6.1; WOW64; rv:40.0) Gecko/20100101 Firefox/40.0")));
		req.setOpt(new curlpp::options::WriteStream(&res));
		req.perform();

		return res.str();
	}

	static std::string normalizeUrl(const std::string &url) {
		size_t lastDash = url.find_last_of('/');
		std::string name = url.substr(lastDash + 1, url.length() - lastDash - 1);
		return "http://lyrics.wikia.com/wiki/" + name;
	}

	static std::string trim(std::string &str) {
		size_t first = str.find_first_not_of(' ');
		size_t last = str.find_last_not_of(' ');
		return str.substr(first, (last - first + 1));
	}

	static std::string getTagValue(const std::string &begin, std::string end, const std::string &data) {
		size_t pos0 = data.find(begin);
		size_t pos1 = data.find(end);
		if(pos0 == std::string::npos || pos1 == std::string::npos) {
			return "";
		}
		return data.substr(pos0 + begin.size(), pos1 - pos0 - begin.size()); 
	}
};