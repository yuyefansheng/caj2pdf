/*
  Copyright 2021 (c) Hin-Tak Leung <htl10@users.sourceforge.net>
  See The FreeType Project LICENSE for license terms.

  This is a small wrapper around libjbig2dec to provide a python
  interface to decode JBIG2 stream.

  To build:

      cc -Wall `pkg-config --cflags jbig2dec` -fPIC -shared -o libjbig2codec.so decode_jbig2data_x.cc `pkg-config --libs jbig2dec`
*/

#include <cstdint>
#include <cstring>
#include "jbig2.h"

int decode_jbig2data(char*, int, char*, int, int, int, int);

extern "C" {
int decode_jbig2data_c(char* inbuf, int bufsize, char* outptr, int width, int height, int width_in_padded_4bytes, int width_in_padded_bytes)
{
  return decode_jbig2data(inbuf, bufsize, outptr, width, height, width_in_padded_4bytes, width_in_padded_bytes);
}
}

int decode_jbig2data(char* inbuf, int bufsize, char* outptr, int width, int height, int width_in_padded_4bytes, int width_in_padded_bytes)
{
  int v12;

  Jbig2Ctx *ctx = jbig2_ctx_new(NULL, JBIG2_OPTIONS_EMBEDDED, NULL, NULL, NULL);
  jbig2_data_in(ctx, (const unsigned char*)inbuf, bufsize);
  jbig2_complete_page(ctx); // Required, apparently this is "work around broken CVision embedded streams",
                            // "simulating an end-of-page segment (for broken streams)"
  Jbig2Image *image = jbig2_page_out(ctx);
  if ( height > 0 )
  {
    v12 = 0;
    char* v13 = outptr + (height - 1) * width_in_padded_4bytes;
    unsigned char *data = image->data;
    do
    {
      ++v12;
      memcpy(v13, data, width_in_padded_bytes);
      v13 -= width_in_padded_4bytes;
      data += image->stride;
    }
    while ( v12 != height );
  }
  return 0;
}
