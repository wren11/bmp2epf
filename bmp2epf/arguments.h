#pragma once
#include <string>
#include <map>
#include <vector>

class Arguments
{
public:
	Arguments();
	void parse(const char** argv, int argc);
	size_t input_count() const { return m_frames.size(); };
	std::string filename(int i) const;
	std::string width() const { return m_width; }
	std::string height() const { return m_height; }
	std::string outname() const { return m_outname; }
	std::string pallete_file() const { return m_pallete_file; }

	void print_usage() const;
	bool help() const { return m_help; }
private:
	std::vector<std::string> m_frames;

	std::string m_width;
	std::string m_height;
	std::string m_outname;
	std::string m_pallete_file;
	bool m_help;

	void parse_ordered_field(std::string val);
	void parse_named_field(std::string name, std::string val);
	size_t m_order;

	std::map<std::string, std::string*> m_named_fields;
	std::vector<std::string*> m_ordered_fields;
	std::map<std::string, std::string> m_field_descrption;
	std::map<std::string, bool*> m_named_tags;
	bool parse_named_tag(const char* arg);
};

