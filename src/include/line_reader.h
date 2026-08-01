// Fast line reader for (optionally gzip-compressed) input files.
// Reads large chunks via gzread() and splits lines with memchr(),
// avoiding the per-byte virtual-call and tiny-syscall overhead of
// istream::getline() on a 300-byte streambuf.
// Semantics are byte-identical to std::getline(std::istream&, std::string&)
// with delimiter '\n': the delimiter is consumed but not stored, and a
// trailing line without a final '\n' is still returned.
#ifndef LINE_READER_H
#define LINE_READER_H

#include <cstddef>
#include <cstring>
#include <string>
#include <zlib.h>

class LineReader
{
public:
	explicit LineReader(const char* path)
		: file(NULL), buf(NULL), pos(0), end(0)
	{
		file = gzopen(path, "rb");
		if (file)
		{
			buf = new char[BufferSize];
		}
	}

	~LineReader() { close(); }

	bool is_open() const { return file != NULL; }

	void close()
	{
		if (file) { gzclose(file); file = NULL; }
		if (buf) { delete[] buf; buf = NULL; }
		pos = end = 0;
	}

	bool getline(std::string& out)
	{
		out.clear();
		if (!file) { return false; }
		for (;;)
		{
			if (pos == end)
			{
				if (!refill()) { return !out.empty(); }
			}
			const char* nl = static_cast<const char*>(memchr(buf + pos, '\n', end - pos));
			if (nl)
			{
				out.append(buf + pos, static_cast<size_t>(nl - (buf + pos)));
				pos = static_cast<size_t>(nl - buf) + 1;
				return true;
			}
			out.append(buf + pos, end - pos);
			pos = end;
		}
	}

private:
	enum { BufferSize = 1 << 20 };

	bool refill()
	{
		pos = 0;
		end = 0;
		int n = gzread(file, buf, BufferSize);
		if (n <= 0) { return false; }
		end = static_cast<size_t>(n);
		return true;
	}

	gzFile file;
	char* buf;
	size_t pos;
	size_t end;
};

#endif // LINE_READER_H
