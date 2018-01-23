#include "arguments.h"
#include <string.h>
#include <exception>
#include <map>
#include <exception>
#include <iostream>
#include "exceptions.h"

using namespace std;

void Arguments::parse(const char** argv, int argc)
{
	for (int i = 1; i < argc; ++i)
	{
		const char* arg = argv[i];
		if (strlen(arg) < 2)
			throw usage_exception( string("Invalid argument: ") + arg );

		if (arg[0] == '-' && arg[1] == '-')
		{
			if (!parse_named_tag(&arg[2]))
			{
				if (i + 1 == argc)
					throw usage_exception(string("No value for argument: ") + &arg[2]);
				parse_named_field(&arg[2], argv[i + 1]);
				++i;
			}
		}
		else
		{
			parse_ordered_field(arg);
		}

	}
}

std::string Arguments::filename(int i) const
{
	return (m_frames.size() <= i) ? string() : m_frames[i];
}

void Arguments::print_usage() const
{
	cout << "Usage:" << endl;
	cout << "bmp2epf.exe --frame filename1  --frame filename2 outfilename" << endl;

	cout << "optional arguments:" << endl;

	for (auto itr = m_named_fields.begin(); itr != m_named_fields.end(); ++itr)
	{
		auto itr2 = m_field_descrption.find(itr->first);
		cout << "\t--" << itr->first << ":" << itr2->second << endl;
	}
	

}

Arguments::Arguments() :m_order(0), m_help(false)
{

	m_named_tags =
	{
	{ "help", &m_help }
	};

	m_named_fields =
	{
		{ "width", &m_width },
		{ "height", &m_height },
		{ "outfilename", &m_outname },
		{ "pallete", &m_pallete_file },
	};

	m_field_descrption = 
	{
		{ "frame", "Input filename in bmp format(24bit)" },
		{ "width",  "Width of the efp file"},
		{ "height", "Height of the efp file" },
		{ "outfilename", "Name of the output efp file. Efp suffix is not needed" },
		{ "pallete", "Name of the pallete file if you are going to use external pallete"},
		{ "help", "Prints this help"}
	};



	m_ordered_fields = { &m_outname };
}

void Arguments::parse_ordered_field(std::string val)
{

	*m_ordered_fields[m_order++] = val;
}

void Arguments::parse_named_field(std::string name, std::string val)
{

	if (name == "frame")
	{
		m_frames.push_back(val);
		return;
	}
	auto itr = m_named_fields.find(name);
	if (itr == m_named_fields.end())
		throw std::exception((std::string("Invalid argument: " + name).data()));
	*(itr->second) = val;

}

bool Arguments::parse_named_tag(const char* arg)
{
	auto itr = m_named_tags.find(arg);
	if (itr != m_named_tags.end())
	{
		*(itr->second) = true;
		return true;
	}
	return false;
}

