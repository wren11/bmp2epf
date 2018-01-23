#include "BmpFile.h"
#include <assert.h>
#include "arguments.h"
#include <iostream>
#include "exceptions.h"
#include "GdiFIle.h"
#include <algorithm>

using namespace std;

void process(const Arguments &args)
{
	int min_width = 0;
	int min_height = 0;

	EpfFile epf;
	for (int i = 0; i < args.input_count(); ++i)
	{
		BmpData bmp_data;
		gdi_extract(args.filename(i), &bmp_data);
		Rect r = { 0, bmp_data.m_width, 0,  bmp_data.m_height };
		epf.append_frame(r, bmp_data.m_data);
		min_width = max(min_width, bmp_data.m_width);
		min_height = max(min_height, bmp_data.m_height);
	}

	if (!args.height().empty() && !args.width().empty())
	{
		uint32_t width = atoi(args.width().c_str());
		uint32_t height = atoi(args.height().c_str());
		if (height >> 0xFFFF)
			throw usage_exception("Invalid height");
		if (width >> 0xFFFF)
			throw usage_exception("Invalid width");
		epf.set_size(width, height);
	}
	else
		epf.set_size(min_width, min_height);
	
	if (!args.pallete_file().empty())
	{
		std::vector<Pixel> pallete;
		load_pallete(args.pallete_file(), &pallete);
		epf.set_pallete(pallete);
	}

	std::vector<uint8_t> out_data;
	assert(epf.create_raw_data(&out_data));

	std::string outfile = args.outname();
	if (outfile.empty())
	{
		outfile = args.filename(0);
		outfile.resize(outfile.size() - 3);
		outfile += "epf";
	}

	FILE* f = fopen(outfile.c_str(), "wb");
	assert(f);
	fwrite(out_data.data(), out_data.size(), 1, f);
	fclose(f);

	if (args.pallete_file().empty())
	{
		string palfile = outfile;
		palfile.resize(palfile.size() - 3);
		palfile += "pal";

		vector<Pixel> pallete;
		epf.get_pallete(&pallete);
		save_pallete_to_file(palfile, pallete);
	}


	//tbl file
	vector<uint8_t> tbl_data;
	epf.get_tbl_data(&tbl_data);
	string tbl_name = outfile;
	tbl_name.resize(tbl_name.size() - 3);
	tbl_name += "tbl";

	FILE* tblf = fopen(tbl_name.c_str(), "wb");
	if (!tblf)
		throw exception((string("cant open tbl file ") + tbl_name + "for writing").c_str());
	fwrite(tbl_data.data(), tbl_data.size(), 1, tblf);
	fclose(tblf);


}

int main(int argc, const char** argv )
{
	
	try
	{
		Arguments args;
		args.parse(argv, argc);

		if (args.help())
		{
			args.print_usage();
			exit(0);
		}

		if (args.input_count() == 0)
			throw usage_exception("Input filename is not set");

		process(args);



	}
	catch (const usage_exception& e)
	{
		std::cout << e.what() << endl;
		Arguments().print_usage();
		system("pause");
		return -1;
	}
	catch (const exception& e)
	{
		std::cout << e.what() << endl;
		system("pause");
		return -1;
	}

	



    return 0;
}

