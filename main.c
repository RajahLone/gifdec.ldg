
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <ldg.h>

#include "gifdec.h"

/* structures sizes */

unsigned int CDECL get_sizeof_gd_gif_struct() { return (uint16_t)sizeof(gd_GIF); }

/* functions */

int CDECL gd_open_file(gd_GIF *gif, const char *fname)
{
	gd_GIF *tmp;
	
	tmp = gd_open_gif(fname);
	
	if (tmp)
	{
  	memcpy(gif, tmp, sizeof(gd_GIF));
    ldg_Free(tmp);
		return 1;
	}
	return 0;
}

int CDECL gd_get_width(gd_GIF *gif) { return gif->width; }
int CDECL gd_get_height(gd_GIF *gif) { return gif->height; }

int CDECL gd_get_depth(gd_GIF *gif) { return gif->depth; /*(gif->depth == 1) ? 1 : (gif->depth + 1);*/ } // TODO : à corriger, dans le GFA ?
uint8_t* CDECL gd_get_palette(gd_GIF *gif) { return &gif->gct.colors[0]; }
int CDECL gd_get_background_index(gd_GIF *gif) { return gif->bgindex; }

int CDECL gd_has_transparency(gd_GIF *gif) { return gif->gce.transparency; } // 1 for transparency, else 0
int CDECL gd_get_transparent_index(gd_GIF *gif) { return gif->gce.tindex; }

int CDECL gd_get_delay(gd_GIF *gif) { return gif->gce.delay; }

int CDECL gd_get_chunky_frame(gd_GIF *gif, uint8_t *buffer)
{
  int ret;
  
  ret = gd_get_frame(gif);
  
  if (ret == 1) { gd_render_frame(gif, buffer); }
  
  return ret; // 1: frame content is processed with possible next frame, 0: end = no next frame, -1: error
}

void CDECL gd_close_file(gd_GIF *gif)
{
  close(gif->fd);
  ldg_Free(gif->frame);
}

/* populate functions list and info for the LDG */

PROC LibFunc[] =
{
  {"get_sizeof_gd_gif_struct", "long get_sizeof_gd_gif_struct(void);\n", get_sizeof_gd_gif_struct},
	
  {"gd_open_file", "int gd_open_file(gd_GIF *gif, const char *fname);\n", gd_open_file},
  
  {"gd_get_width", "int gd_get_width(gd_GIF *gif);\n", gd_get_width},
  {"gd_get_height", "int gd_get_height(gd_GIF *gif);\n", gd_get_height},
  
  {"gd_get_depth", "int gd_get_depth(gd_GIF *gif);\n", gd_get_depth},
  {"gd_get_palette", "uint8_t* gd_get_palette(gd_GIF *gif);\n", gd_get_palette},
  {"gd_get_background_index", "int gd_get_background_index(gd_GIF *gif);\n", gd_get_background_index},
  
  {"gd_get_chunky_frame", "int gd_get_chunky_frame(gd_GIF *gif, uint8_t *buffer);\n", gd_get_chunky_frame},

  {"gd_has_transparency", "int gd_has_transparency(gd_GIF *gif);\n", gd_has_transparency},
  {"gd_get_transparent_index", "int gd_get_transparent_index(gd_GIF *gif);\n", gd_get_transparent_index},

  {"gd_get_delay", "int gd_get_delay(gd_GIF *gif);\n", gd_get_delay},

  {"gd_close_file", "void gd_close_file(gd_GIF *gif);\n", gd_close_file},
};

LDGLIB LibLdg[] = { { 0x0001,	9, LibFunc, "GIF decoder from https://github.com/lecram", 1} };

/*  */

int main(void)
{
	ldg_init(LibLdg);
	return 0;
}
