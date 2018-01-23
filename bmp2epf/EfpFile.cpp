#include "EfpFile.h"
#include "BitStack.h"
#include <assert.h>
#include <stdio.h>
#include <algorithm>
#include "exceptions.h"


EpfFile::EpfFile():m_min_width(0),m_min_height(0),
m_width(0),m_height(0)
{
}

bool EpfFile::create_raw_data(std::vector<uint8_t>* out)
{
	if (m_min_width > m_width || m_min_height > m_height)
		return false;

	m_raw_data.clear();
	m_raw_data.reserve(0x10000);

	if (m_pixel_index.size() == 0)
		create_pallete_from_frame_data();

	push(uint16_t(m_frames.size()));

	push(m_width);   
	push(m_height);  
	push(uint16_t(0));

	std::vector<uint32_t> pixel_data_offset;
	std::vector<uint32_t> stencil_data_offset;

	uint32_t offs = 0;
	for (Rect& r : m_frame_coords)
	{
		uint32_t square = r.height() * r.width();
		pixel_data_offset.push_back(offs);
		offs += square;
		stencil_data_offset.push_back(offs);
		offs += (square + 3) / 4; 
	}
	push(offs); 

	for (size_t i = 0; i < m_frames.size(); ++i)
	{
		assert(12 + pixel_data_offset[i] == m_raw_data.size());
		
		FrameData& frame =  m_frames[i];
		for (Pixel pixel : frame)
		{
			auto itr = m_pixel_index.find(pixel);
			if (itr == m_pixel_index.end())
				throw std::exception("Pallete colors count is not enought for image");

			push(itr->second);
		}

		assert(12 + stencil_data_offset[i] == m_raw_data.size());
	
		BitStack b;
		for (Pixel pixel : frame)
		{
			b.push(false); 
			b.push(false); 
		}
		b.flush();
		m_raw_data.insert(m_raw_data.end(), b.data().begin(), b.data().end());
	}

	for (size_t i = 0; i < m_frame_coords.size(); ++i)
	{
		push(m_frame_coords[i].top);
		push(m_frame_coords[i].left);
		push(m_frame_coords[i].bottom);
		push(m_frame_coords[i].right);
		push(pixel_data_offset[i]);
		push(stencil_data_offset[i]);
	}

	push(uint16_t(0));
	push(uint16_t(0));
	push(uint16_t(0));
	push(uint16_t(0));
	push(offs);
	push(uint32_t(0));

	create_tbl_data();
	*out = m_raw_data;
	return true;
}

void EpfFile::set_size(uint16_t width, uint16_t height)
{
	m_width = width;
	m_height = height;
}

void EpfFile::append_frame(const Rect& coords, const FrameData &data)
{
	m_min_width = std::max(m_min_width,coords.right);
	m_min_height = std::max(m_min_height, coords.bottom);

	assert(coords.height()
		* coords.width() == data.size());

	m_frame_coords.push_back(coords);
	m_frames.push_back(data);
}

template <class T>
void EpfFile::push(const T& var)
{
	m_raw_data.resize(m_raw_data.size() + sizeof(var));
	memcpy(m_raw_data.data() + m_raw_data.size() - sizeof(var), &var, sizeof(var));
}

void EpfFile::create_pixel_index()
{
	int idx = 0;
	for (Pixel p : m_pallete)
		m_pixel_index[p] = idx++;
}

void EpfFile::create_pallete_from_frame_data()
{
	m_pallete.clear();
	std::set<Pixel> new_pallete;
	
	m_pixel_index.clear();

	for (FrameData& fr : m_frames)
	{
		for (Pixel& p : fr)
			new_pallete.insert(p);
	}
	m_pallete.insert(m_pallete.end(), new_pallete.begin(), new_pallete.end());

	if (m_pallete.size() > 256)
		throw std::exception("pallete size is bigger than 256 colors");

	create_pixel_index();
	
	m_pallete.resize(256, Pixel(255, 255, 255));
}

void EpfFile::create_tbl_data()
{
	m_tbl.clear();
	for (int i = 0; i < m_frames.size(); ++i)
	{
		m_tbl.push_back(0x37);
		m_tbl.push_back(0x46); 
	}
}

void EpfFile::set_pallete(const std::vector<Pixel>& pallete)
{
	m_pallete = pallete;
	create_pixel_index();
}

void EpfFile::get_pallete(std::vector<Pixel>* pallete)
{
	*pallete = m_pallete;
}


void EpfFile::get_tbl_data(std::vector<uint8_t> *tbl)
{
	tbl->resize(m_tbl.size() * 2);
	memcpy(tbl->data(), m_tbl.data(), tbl->size());
}

bool Pixel::operator<(const Pixel& p) const
{
	if (R != p.R)
		return R < p.R;
	if (G != p.G)
		return G < p.G;
	return B < p.B;
}

void load_pallete(const std::vector<uint8_t> &palfile, std::vector<Pixel> *pallete)
{
	assert(!(palfile.size() % 3));
	for (size_t i = 0; i < palfile.size(); i += 3)
		pallete->push_back(Pixel(palfile[i], palfile[i + 1], palfile[i + 2]));
}

void load_pallete(std::string filename, std::vector<Pixel> *pallete)
{

	FILE* palf = fopen(filename.c_str(), "rb");
	assert(palf);
	struct stat s;
	fstat(fileno(palf), &s);
	std::vector<uint8_t> pal_data(s.st_size);
	fread(pal_data.data(), s.st_size, 1, palf);
	fclose(palf);
	load_pallete(pal_data, pallete);
	
}

void save_pallete_to_file(std::string filename, const std::vector<Pixel> &pallete)
{
	FILE* palf = fopen(filename.c_str(), "wb");
	if (!palf)
		throw std::exception((std::string("Cant open ") + filename + " to save pallete").c_str());
	assert(pallete.size() == 256);

	for (Pixel p : pallete)
	{
		fwrite(&p.R, sizeof(p.R), 1, palf);
		fwrite(&p.G, sizeof(p.R), 1, palf);
		fwrite(&p.B, sizeof(p.R), 1, palf);
	}
	fclose(palf);
}

