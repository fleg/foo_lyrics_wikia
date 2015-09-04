#include "stdafx.h"
#include "lyrics.h"
#include <vector>

DECLARE_COMPONENT_VERSION("foo_lyrics_wikia", "0.0.2","Simple lyrics component");

static const GUID guid_lyrics_item = { 0xcf5f12bc, 0x588a, 0x4d39, { 0x84, 0x2c, 0xa7, 0x6a, 0xc5, 0x2a, 0x6d, 0x5a } };
static const GUID guid_get_lyrics = { 0x4f8f50ed, 0x3a19, 0x4a62, { 0x86, 0xc3, 0x8b, 0xab, 0x98, 0xd0, 0x22, 0xa1 } };
static contextmenu_group_popup_factory g_mygroup(guid_lyrics_item, contextmenu_groups::root, "Lyrics component", 0);

class lyrics_worker;
static std::vector<lyrics_worker*> workers_pool;

class lyrics_initquit : public initquit {
	void on_quit() {
		for(size_t i = 0; i < workers_pool.size(); i++) {
			delete workers_pool[i];
		}
	}
};

class lyrics_worker : public pfc::thread {
public:
	lyrics_worker(const std::string &artist, const std::string &title) :_artist(artist), _title(title) { }
	~lyrics_worker() {
		waitTillDone();
	}

	void threadProc() {
		std::string windowTitle = "Lyrics: " + _artist + " - " + _title;
		popup_message::g_show(lyrics::getLyrics(_artist, _title).c_str(), windowTitle.c_str());
	}

private:
	std::string _artist;
	std::string _title;
};

class lyrics_context_item : public contextmenu_item_simple {
public:
	enum {
		cmd_get_lyrics = 0,
		cmd_total
	};

	t_uint32 get_num_items() {
		return cmd_total;
	}

	GUID get_parent() {
		return guid_lyrics_item;
	}

	GUID get_item_guid(t_uint32 p_index) {
		switch(p_index) {
		case cmd_get_lyrics:
			return guid_get_lyrics;
		default: 
			uBugCheck(); 
		}
	}

	void get_item_name(t_uint32 p_index, pfc::string_base &p_out) {
		switch(p_index) {
		case cmd_get_lyrics: 
			p_out = "Get lyrics"; 
			break;
		default: 
			uBugCheck();
		}
	}

	bool get_item_description(t_uint32 p_index, pfc::string_base &p_out) {
		switch(p_index) {
		case cmd_get_lyrics: 
			p_out = "Get lyrics";
			return true;
		default: 
			uBugCheck();
		}
	}

	void context_command(t_uint32 p_index, metadb_handle_list_cref p_data, const GUID &p_caller) {
		switch(p_index) {
		case cmd_get_lyrics:
			get_lyrics_click();
			break;
		default:
			uBugCheck();
		}
	}

private:
	void get_lyrics_click() {
		metadb_handle_ptr track;
		static_api_ptr_t<playlist_manager> playlistManager;
		playlistManager->activeplaylist_get_focus_item_handle(track);
		file_info_impl fi;
		track->get_info(fi);

		std::string artist(fi.meta_enum_value(fi.meta_find("artist"), 0));
		std::string title(fi.meta_enum_value(fi.meta_find("title"), 0));

		if(artist.empty() || title.empty()) {
			popup_message::g_show("can't get track artist or title", "Lyrics: Undefined");
		}
		else {
			lyrics_worker *worker = new lyrics_worker(artist, title);
			worker->start();
			workers_pool.push_back(worker);
		}
	}
};


static contextmenu_item_factory_t<lyrics_context_item> g_lyrics_context_item_factory;
static initquit_factory_t<lyrics_initquit> g_lyrics_initquit_factory;