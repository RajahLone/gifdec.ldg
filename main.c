
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ldg.h>
#include <gif_lib.h>

#define STRINGIFY(x) #x
#define VERSION_LIB(A,B,C) STRINGIFY(A) "." STRINGIFY(B) "." STRINGIFY(C)
#define VERSION_LDG(A,B,C) "GIF decoder from The GIFLib Project (" STRINGIFY(A) "." STRINGIFY(B) "." STRINGIFY(C) ")"

/* global variables */

int error;
uint8_t *palette;

/* functions */

const char * CDECL gifdec_get_lib_version() { return VERSION_LIB(GIFLIB_MAJOR, GIFLIB_MINOR, GIFLIB_RELEASE); }

GifFileType * CDECL gifdec_open(const char *fileName) { error = 0; return DGifOpenFileName(fileName, &error); }
int32_t CDECL gifdec_read(GifFileType *gif) { return (int32_t)DGifSlurp(gif); }

const char * CDECL gifdec_get_gif_version(GifFileType *gif) { return DGifGetGifVersion(gif); }
int32_t CDECL gifdec_get_width(GifFileType *gif) { return (int32_t)gif->SWidth; }
int32_t CDECL gifdec_get_height(GifFileType *gif) { return (int32_t)gif->SHeight; }
int32_t CDECL gifdec_get_bckgrnd_index(GifFileType *gif) { return (int32_t)gif->SBackGroundColor; }

int32_t CDECL gifdec_get_images_count(GifFileType *gif) { return (int32_t)gif->ImageCount; }

int32_t CDECL gifdec_get_image_top(GifFileType *gif, int idx) { return (idx < 0 || idx >= gif->ImageCount) ? 0 : (int32_t)gif->SavedImages[idx].ImageDesc.Top; }
int32_t CDECL gifdec_get_image_left(GifFileType *gif, int idx) { return (idx < 0 || idx >= gif->ImageCount) ? 0 : (int32_t)gif->SavedImages[idx].ImageDesc.Left; }
int32_t CDECL gifdec_get_image_width(GifFileType *gif, int idx) { return (idx < 0 || idx >= gif->ImageCount) ? (int32_t)gif->SWidth : (int32_t)gif->SavedImages[idx].ImageDesc.Width; }
int32_t CDECL gifdec_get_image_height(GifFileType *gif, int idx) { return (idx < 0 || idx >= gif->ImageCount) ? (int32_t)gif->SHeight : (int32_t)gif->SavedImages[idx].ImageDesc.Height; }

int32_t CDECL gifdec_get_colors_count(GifFileType *gif, int idx)
{
  if (idx < 0 || idx >= gif->ImageCount) { return 0; }
  
  GifImageDesc *dsc = &gif->SavedImages[idx].ImageDesc;
  ColorMapObject *map = dsc->ColorMap ? dsc->ColorMap : gif->SColorMap;
  
  return (int32_t)map->ColorCount;
}
uint8_t* CDECL gifdec_get_colors_table(GifFileType *gif, int idx)
{
  GifColorType rgb;
  uint8_t *p;
  
  if (idx < 0 || idx >= gif->ImageCount) { return NULL; }
  
  GifImageDesc *dsc = &gif->SavedImages[idx].ImageDesc;
  ColorMapObject *map = dsc->ColorMap ? dsc->ColorMap : gif->SColorMap;
  
  if (!palette) { palette = calloc(1, 256 * 3); } else { memset(palette, 0, 256 * 3); }
  
  if (!palette) { return NULL; }
  
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

GifByteType* CDECL gifdec_get_chunky_raster(GifFileType *gif, int idx)
{
  return (idx < 0 || idx >= gif->ImageCount) ? NULL : (gif->SavedImages[idx]).RasterBits;
}

int32_t CDECL gifdec_get_image_disposal(GifFileType* gif, int idx)
{
  if (idx < 0 || idx >= gif->ImageCount) { return 0; }
  
  GraphicsControlBlock gcb = {0};
  DGifSavedExtensionToGCB(gif, idx, &gcb);
  return gcb.DisposalMode;
}
int32_t CDECL gifdec_get_trnsprnt_index(GifFileType* gif, int idx)
{
  if (idx < 0 || idx >= gif->ImageCount) { return -1; }
  
  GraphicsControlBlock gcb = {0};
  DGifSavedExtensionToGCB(gif, idx, &gcb);
  return gcb.TransparentColor;
}
int32_t CDECL gifdec_get_image_delay(GifFileType* gif, int idx)
{
  if (idx < 0 || idx >= gif->ImageCount) { return 0; }
  
  GraphicsControlBlock gcb = {0};
  DGifSavedExtensionToGCB(gif, idx, &gcb);
  return gcb.DelayTime;
}

int32_t CDECL gifdec_close(GifFileType *gif)
{
  free(palette);
  palette = NULL;
  error = 0;
 
  if (DGifCloseFile(gif, &error) == GIF_ERROR) { return -error; }
  
  return GIF_OK;
}

const char * CDECL gifdec_get_last_error(GifFileType *gif) { return GifErrorString(gif->Error); }

/* populate functions list and info for the LDG */

PROC LibFunc[] =
{
  {"gifdec_get_lib_version", "const char* gifdec_get_lib_version();\n", gifdec_get_lib_version},

  {"gifdec_open", "GifFileType* gifdec_open(const char *fileName);\n", gifdec_open},
  {"gifdec_read", "int32_t gifdec_read(GifFileType *gif);\n", gifdec_read},

  {"gifdec_get_gif_version", "const char* gifdec_get_gif_version(GifFileType *gif);\n", gifdec_get_gif_version},
  {"gifdec_get_width", "int32_t gifdec_get_width(GifFileType *gif);\n", gifdec_get_width},
  {"gifdec_get_height", "int32_t gifdec_get_height(GifFileType *gif);\n", gifdec_get_height},
  {"gifdec_get_bckgrnd_index", "int32_t gifdec_get_bckgrnd_index(GifFileType *gif);\n", gifdec_get_bckgrnd_index},

  {"gifdec_get_images_count", "int32_t gifdec_get_images_count(GifFileType *gif);\n", gifdec_get_images_count},
  {"gifdec_get_image_top", "int32_t gifdec_get_image_top(GifFileType *gif, int idx);\n", gifdec_get_image_top},
  {"gifdec_get_image_left", "int32_t gifdec_get_image_left(GifFileType *gif, int idx);\n", gifdec_get_image_left},
  {"gifdec_get_image_width", "int32_t gifdec_get_image_width(GifFileType *gif, int idx);\n", gifdec_get_image_width},
  {"gifdec_get_image_height", "int32_t gifdec_get_image_height(GifFileType *gif, int idx);\n", gifdec_get_image_height},
  {"gifdec_get_colors_count", "int32_t gifdec_get_colors_count(GifFileType *gif, int idx);\n", gifdec_get_colors_count},
  {"gifdec_get_colors_table", "uint8_t* gifdec_get_colors_table(GifFileType *gif, int idx);\n", gifdec_get_colors_table},
  {"gifdec_get_chunky_raster", "uint8_t* gifdec_get_chunky_raster(GifFileType *gif, int idx);\n", gifdec_get_chunky_raster},
  
  {"gifdec_get_image_disposal", "int32_t gifdec_get_image_disposal(GifFileType* gif, int idx);\n", gifdec_get_image_disposal},
  {"gifdec_get_trnsprnt_index", "int32_t gifdec_get_trnsprnt_index(GifFileType* gif, int idx);\n", gifdec_get_trnsprnt_index},
  {"gifdec_get_image_delay", "int32_t gifdec_get_image_delay(GifFileType* gif, int idx);\n", gifdec_get_image_delay},

  {"gifdec_close", "int32_t gifdec_close(GifFileType *gif);\n", gifdec_close},

  {"gifdec_get_last_error", "const char* gifdec_get_last_error(GifFileType *gif);\n", gifdec_get_last_error},
};

LDGLIB LibLdg[] = { { 0x0001, 20, LibFunc, VERSION_LDG(GIFLIB_MAJOR, GIFLIB_MINOR, GIFLIB_RELEASE), 1} };

/*  */

int main(void)
{
  ldg_init(LibLdg);
  return 0;
}
