//
// Created by 26372 on 2022/4/17.
//

#include "protocol.h"
#include <sys/types.h>
#include <sys/socket.h>

static const std::unordered_map<std::string, std::string> file_suffix_mapped {
		{"html",  "text/html"},
		{"htm",   "text/html"},
		{"css",   "text/css"},
		{"xml",   "application/xml"},
		{"json",  "application/json"},
		{"js",    "application/x-javascript"},
		{"mpa",   "video/mpeg"},
		{"png",   "image/png"},
		{"jpeg",  "image/jpeg"},
		{"woff2", "application/font-woff2"},
		{"txt",   "text/plain"},
		{"ppt",   "application/vnd.ms-powerpoint"},
		{"pic",   "image/pict"},
		{"body",  "text/html"},
		{"tif",   "image/tiff"},
		{"wbmp",  "image/vnd.wap.wbmp"},
		{"svgz",  "image/svg+xml"},
		{"woff",  "application/font-woff"},
		{"gif",   "image/gif"},
		{"jpg",   "image/jpeg"},
		{"ttf",   "application/x-font-ttf"},
		{"mp4",   "video/mp4"},
		{"mp3",   "audio/mpeg"},
		{"pdf",   "application/pdf"},
		{"xls",   "application/vnd.ms-excel"},
};

std::string file_type(const std::string &suffix) {
	auto iter = file_suffix_mapped.find(suffix);
	return iter == file_suffix_mapped.end() ? "text/plain" : iter->second;
}
