
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <gem.h>
#include <ldg.h>
#include <gif_lib.h>

#define STRINGIFY(x) #x
#define VERSION_LIB(A,B,C) STRINGIFY(A) "." STRINGIFY(B) "." STRINGIFY(C)
#define VERSION_LDG(A,B,C) "GIF decoder from The GIFLib Project (" STRINGIFY(A) "." STRINGIFY(B) "." STRINGIFY(C) ")"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

/* structures */

typedef struct gif_mem_file {
  uint8_t *data;
  int size;
  int offset;
} gif_mem_file;

/* global variables */

static unsigned char palette[768] = { 0 };

static gif_mem_file gif_mf;

GifFileType *gif_read = NULL;

/* internal functions */

static int gifldg_read(GifFileType* gif_ptr, GifByteType* data, int count)
{
  gif_mem_file *mf = (gif_mem_file *) gif_ptr->UserData;
    
  count = MIN(count, mf->size - mf->offset);
  
  if (count < 1) { return 0; }

  if (mf->offset + count <= mf->size)
  {
    memcpy(data, mf->data + mf->offset, count);
    mf->offset += count;
    return count;
  }
  
  return 0;
}

/* functions */

const char * CDECL gifdec_get_lib_version() { return VERSION_LIB(GIFLIB_MAJOR, GIFLIB_MINOR, GIFLIB_RELEASE); }

int32_t CDECL gifdec_close()
{
  int err = 0;
  
  if (gif_read != NULL) { DGifCloseFile(gif_read, &err); }

  gif_read = NULL;
 
  gif_mf.data = NULL;
  gif_mf.size = 0;
  gif_mf.offset = 0;
  
  return GIF_OK;
}
int32_t CDECL gifdec_open(uint8_t *data, const int size)
{
  if (gif_read != NULL) { gifdec_close(); }
  
  gif_mf.data = data;
  gif_mf.size = size;
  gif_mf.offset = 0;

  int err = 0;

  gif_read = DGifOpen(&gif_mf, gifldg_read, &err);
  
  if (gif_read) { return GIF_OK; }
  
  return GIF_ERROR;
}

int32_t CDECL gifdec_read()
{
  if (gif_read == NULL) { return GIF_ERROR; }
  
  return (int32_t)DGifSlurp(gif_read);
}
int32_t CDECL gifdec_get_width()
{
  if (gif_read == NULL) { return 0; }
  return (int32_t)gif_read->SWidth;
}
int32_t CDECL gifdec_get_height()
{
  if (gif_read == NULL) { return 0; }
  return (int32_t)gif_read->SHeight;
}
int32_t CDECL gifdec_get_bckgrnd_index()
{
  if (gif_read == NULL) { return -1; }
  return (int32_t)gif_read->SBackGroundColor;
}

int32_t CDECL gifdec_get_frames_count()
{
  if (gif_read == NULL) { return 0; }
  return (int32_t)gif_read->ImageCount;
}
int32_t CDECL gifdec_get_frame_left(int idx)
{
  if (gif_read == NULL) { return 0; }
  return (idx < 0 || idx >= gif_read->ImageCount) ? 0 : (int32_t)gif_read->SavedImages[idx].ImageDesc.Left;
}
int32_t CDECL gifdec_get_frame_top(int idx)
{
  if (gif_read == NULL) { return 0; }
  return (idx < 0 || idx >= gif_read->ImageCount) ? 0 : (int32_t)gif_read->SavedImages[idx].ImageDesc.Top;
}
int32_t CDECL gifdec_get_frame_width(int idx)
{
  if (gif_read == NULL) { return 0; }
  return (idx < 0 || idx >= gif_read->ImageCount) ? (int32_t)gif_read->SWidth : (int32_t)gif_read->SavedImages[idx].ImageDesc.Width;
}
int32_t CDECL gifdec_get_frame_height(int idx)
{
  if (gif_read == NULL) { return 0; }
  return (idx < 0 || idx >= gif_read->ImageCount) ? (int32_t)gif_read->SHeight : (int32_t)gif_read->SavedImages[idx].ImageDesc.Height;
}

int32_t CDECL gifdec_get_colors_count( int idx)
{
  if (gif_read == NULL) { return 0; }
  if (idx < 0 || idx >= gif_read->ImageCount) { return GIF_ERROR; }
  
  GifImageDesc *dsc = &gif_read->SavedImages[idx].ImageDesc;
  ColorMapObject *map = dsc->ColorMap ? dsc->ColorMap : gif_read->SColorMap;
  
  return (int32_t)map->ColorCount;
}
uint8_t* CDECL gifdec_get_colors_table(int idx)
{
  if (gif_read == NULL) { return NULL; }
  
  GifColorType rgb;
  uint8_t *p;
  
  if (idx < 0 || idx >= gif_read->ImageCount) { return NULL; }
  
  GifImageDesc *dsc = &gif_read->SavedImages[idx].ImageDesc;
  ColorMapObject *map = dsc->ColorMap ? dsc->ColorMap : gif_read->SColorMap;
  
  memset(palette, 0, 768);

  p = palette;
  for (int c = 0; c < map->ColorCount; c++)
  {
    rgb = map->Colors[c];
    *p = rgb.Red; p++;
    *p = rgb.Green; p++;
    *p = rgb.Blue; p++;
  }
  
  return palette;
}

uint8_t* CDECL gifdec_get_chunky_raster(int idx)
{
  if (gif_read == NULL) { return NULL; }
  
  return (idx < 0 || idx >= gif_read->ImageCount) ? NULL : (gif_read->SavedImages[idx]).RasterBits;
}

int32_t CDECL gifdec_get_image_disposal(int idx)
{
  if (gif_read == NULL) { return 0; }
  if (idx < 0 || idx >= gif_read->ImageCount) { return 0; }
  
  GraphicsControlBlock gcb = {0};
  DGifSavedExtensionToGCB(gif_read, idx, &gcb);
  return gcb.DisposalMode;
}
int32_t CDECL gifdec_get_trnsprnt_index(int idx)
{
  if (gif_read == NULL) { return -1; }
  if (idx < 0 || idx >= gif_read->ImageCount) { return -1; }
  
  GraphicsControlBlock gcb = {0};
  DGifSavedExtensionToGCB(gif_read, idx, &gcb);
  return gcb.TransparentColor;
}
int32_t CDECL gifdec_get_image_delay(int idx)
{
  if (gif_read == NULL) { return 0; }
  if (idx < 0 || idx >= gif_read->ImageCount) { return 0; }
  
  GraphicsControlBlock gcb = {0};
  DGifSavedExtensionToGCB(gif_read, idx, &gcb);
  return gcb.DelayTime;
}

const char * CDECL gifdec_get_last_error(GifFileType *gif) { return GifErrorString(gif->Error); }

/* populate functions list and info for the LDG */

PROC LibFunc[] =
{
  {"gifdec_get_lib_version", "const char* gifdec_get_lib_version();\n", gifdec_get_lib_version},

  {"gifdec_open", "int32_t gifdec_open(uint8_t *data, const int size);\n", gifdec_open},
  {"gifdec_read", "int32_t gifdec_read();\n", gifdec_read},

  {"gifdec_get_width", "int32_t gifdec_get_width();\n", gifdec_get_width},
  {"gifdec_get_height", "int32_t gifdec_get_height();\n", gifdec_get_height},
  {"gifdec_get_bckgrnd_index", "int32_t gifdec_get_bckgrnd_index();\n", gifdec_get_bckgrnd_index},

  {"gifdec_get_frames_count", "int32_t gifdec_get_images_count();\n", gifdec_get_frames_count},
  {"gifdec_get_frame_left", "int32_t gifdec_get_image_left(int idx);\n", gifdec_get_frame_left},
  {"gifdec_get_frame_top", "int32_t gifdec_get_image_top(int idx);\n", gifdec_get_frame_top},
  {"gifdec_get_frame_width", "int32_t gifdec_get_image_width(int idx);\n", gifdec_get_frame_width},
  {"gifdec_get_frame_height", "int32_t gifdec_get_image_height(int idx);\n", gifdec_get_frame_height},
  {"gifdec_get_colors_count", "int32_t gifdec_get_colors_count(int idx);\n", gifdec_get_colors_count},
  {"gifdec_get_colors_table", "uint8_t* gifdec_get_colors_table(int idx);\n", gifdec_get_colors_table},
  {"gifdec_get_chunky_raster", "uint8_t* gifdec_get_chunky_raster(int idx);\n", gifdec_get_chunky_raster},
  
  {"gifdec_get_image_disposal", "int32_t gifdec_get_image_disposal(int idx);\n", gifdec_get_image_disposal},
  {"gifdec_get_trnsprnt_index", "int32_t gifdec_get_trnsprnt_index(int idx);\n", gifdec_get_trnsprnt_index},
  {"gifdec_get_image_delay", "int32_t gifdec_get_image_delay(int idx);\n", gifdec_get_image_delay},

  {"gifdec_close", "int32_t gifdec_close();\n", gifdec_close},

  {"gifdec_get_last_error", "const char* gifdec_get_last_error();\n", gifdec_get_last_error},
};

LDGLIB LibLdg[] = { { 0x0004, 19, LibFunc, VERSION_LDG(GIFLIB_MAJOR, GIFLIB_MINOR, GIFLIB_RELEASE), 1} };

/*  */

int main(void)
{
  ldg_init(LibLdg);
  return 0;
}
