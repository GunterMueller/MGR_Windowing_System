#include "term.h"
#include "dump.h"
/* bitmap icon 68x55x1*/
char iconbits[] = {
	0x00, 0x7f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x90, 0x00, 0x00,
	0x00, 0x7f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x60, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
	0x00, 0x60, 0x00, 0x7d, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x60, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00,
	0x00, 0x60, 0x00, 0x60, 0x10, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00,
	0x00, 0x7f, 0xff, 0xe4, 0x44, 0x44, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x7f, 0xff, 0xe0, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x09, 0x00, 0x00, 0x10, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xbf, 0xff, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x09, 0x20, 0x00, 0x00, 0x00, 0x40, 0x01, 0xc0, 0x02, 0x00, 0x00,
	0x00, 0x08, 0x20, 0x00, 0x10, 0x00, 0x00, 0x01, 0xa0, 0x20, 0x00, 0x00,
	0x00, 0x09, 0x20, 0x00, 0x10, 0x00, 0x40, 0x01, 0xd0, 0xd4, 0x00, 0x00,
	0x00, 0x00, 0x20, 0x00, 0x10, 0x00, 0x00, 0x01, 0xa0, 0x49, 0x00, 0x00,
	0x00, 0x09, 0x20, 0x00, 0x00, 0x00, 0x40, 0x01, 0xd0, 0x00, 0x00, 0x00,
	0x00, 0x08, 0x20, 0x00, 0x10, 0x00, 0x00, 0x01, 0xa9, 0x50, 0x00, 0x00,
	0x00, 0x0e, 0xfb, 0xbb, 0xb0, 0x00, 0x40, 0x01, 0xd0, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa0, 0x28, 0x00, 0x00,
	0x00, 0x01, 0x20, 0x00, 0x00, 0x00, 0x40, 0x01, 0xd0, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa0, 0x60, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x01, 0xdf, 0xff, 0x00, 0x00,
	0x80, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa0, 0x28, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x01, 0xd0, 0xe2, 0x00, 0x00,
	0x80, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa0, 0x00, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x01, 0xd0, 0x44, 0x00, 0x00,
	0x80, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa0, 0x14, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x01, 0xd0, 0x00, 0x00, 0x00,
	0x80, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa0, 0x14, 0x00, 0x00,
	0x80, 0x00, 0x44, 0x44, 0x44, 0x44, 0x40, 0x01, 0xd0, 0x80, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa0, 0x14, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xd0, 0x88, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa0, 0x14, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xd0, 0x80, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa0, 0x8c, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xd0, 0x00, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa0, 0x00, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xd0, 0x61, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa0, 0x64, 0x00, 0x00,
	0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0xd0, 0xd6, 0x00, 0x00,
	0x80, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xa0, 0x61, 0x00, 0x00,
	0x80, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd6, 0x6f, 0x00, 0x00,
	0x80, 0x00, 0x0a, 0xaa, 0xaa, 0xba, 0xaa, 0xaa, 0xa0, 0x00, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x55, 0x55, 0x56, 0x03, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xaa, 0xaa, 0xa0, 0x2a, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x03, 0x75, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0xa1, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x09, 0x30, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x04, 0x02, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x0c, 0x65, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x0a, 0x61, 0x00, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x00,
	0x7f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00
};
loadicon(n, w, h)
     int n, *w, *h;
{
    int s, old_mode, new_mode;

    m_flush();
    ioctl(fileno(m_termout),TIOCLGET,&old_mode);
    new_mode = old_mode | LLITOUT;
    ioctl(fileno(m_termout),TIOCLSET,&new_mode);

    m_bitdestroy(n);
    m_bitldto(*w=68,*h=55,0,0,n,s=B_SIZE32(68,55,1));
    fwrite(iconbits,s,1,m_termout);
    m_flush();

    ioctl(fileno(m_termout),TIOCLSET,&old_mode);
}