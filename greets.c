#include <ogcsys.h>
#include <gccore.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "timing.h"
#include "greets.h"
#include "screenspace.h"

#include "images/font.h"
#include "font_tpl.h"

static TPLFile fontTPL;

greets_data greets_data_0;

#define TILES_W 64
#define TILES_H 32
#define TILES_FMT GX_TF_IA4

static void
init_tile_shader (void *dummy)
{
  #include "tile-grid.inc"
}

static unsigned char grid_ascii_equiv[] =
  {
    ' ', '!', '?', '0', '1', '2', '3', '4',
    '5', '6', '7', '8', '9', 'A', 'B', 'C',
    'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',
    'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '3'
  };

static unsigned char ascii_to_char[256];

static void
init_ascii_to_char (void)
{
  int i;
  
  memset (ascii_to_char, 255, sizeof (ascii_to_char));
  
  for (i = 0; i < sizeof (grid_ascii_equiv); i++)
    {
      int s_part = i & 7;
      int t_part = (i >> 3) & 7;
      ascii_to_char[grid_ascii_equiv[i]] = s_part * 16 + t_part;
    }
}

static void
put_text (void *tileidx, int row, int startcol, unsigned char *text)
{
  unsigned char *c = tileidx;
  int i;
  
  for (i = 0; text[i] != '\0'; i++)
    {
      int idx = tex_index (startcol + i, row, TILES_W, 8);
      c[idx] = ascii_to_char[text[i]];
    }

  DCFlushRange (c, GX_GetTexBufferSize (TILES_W, TILES_H, TILES_FMT, GX_FALSE,
					0));
}

static void
greets_init_effect (void *params, backbuffer_info *bbuf)
{
  greets_data *gdata = (greets_data *) params;
  
  gdata->world = create_world (0);
  world_set_perspective (gdata->world, 60.0, 1.33f, 10.0f, 500.0f);
  world_set_pos_lookat_up (gdata->world,
			   (guVector) { 0, 0, -30 },
			   (guVector) { 0, 0, 0 },
			   (guVector) { 0, 1, 0 });

  TPL_OpenTPLFromMemory (&fontTPL, (void *) font_tpl, font_tpl_size);
  TPL_GetTexture (&fontTPL, font, &gdata->fontobj);
  GX_InitTexObjFilterMode (&gdata->fontobj, GX_LINEAR, GX_LINEAR);
  GX_InitTexObjWrapMode (&gdata->fontobj, GX_REPEAT, GX_REPEAT);
  
  gdata->tileidx = memalign (32, GX_GetTexBufferSize (TILES_W, TILES_H,
						      TILES_FMT, GX_FALSE, 0));
  GX_InitTexObj (&gdata->tileidxobj, gdata->tileidx, TILES_W, TILES_H,
		 TILES_FMT, GX_CLAMP, GX_CLAMP, GX_FALSE);
  GX_InitTexObjFilterMode (&gdata->tileidxobj, GX_NEAR, GX_NEAR);

  gdata->tile_shader = create_shader (&init_tile_shader, NULL);
  shader_append_texmap (gdata->tile_shader, &gdata->fontobj, GX_TEXMAP0);
  shader_append_texmap (gdata->tile_shader, &gdata->tileidxobj, GX_TEXMAP1);
  shader_append_texcoordgen (gdata->tile_shader, GX_TEXCOORD0, GX_TG_MTX2x4,
			     GX_TG_TEX0, GX_IDENTITY);
  memset (gdata->tileidx, 0, GX_GetTexBufferSize (TILES_W, TILES_H,
						  TILES_FMT, GX_FALSE, 0));
  init_ascii_to_char ();
  put_text (gdata->tileidx, 2, 1, (unsigned char *) "HELLO!");
}

static void
greets_uninit_effect (void *params, backbuffer_info *bbuf)
{
  greets_data *gdata = (greets_data *) params;

  free_world (gdata->world);
  free (gdata->tileidx);
  free_shader (gdata->tile_shader);
}

static void
greets_display_effect (uint32_t time_offset, void *params, int iparam)
{
  greets_data *gdata = (greets_data *) params;
  f32 indmtx[2][3] = { { 0.5, 0.0, 0.0 },
		       { 0.0, 0.5, 0.0 } };

  GX_SetIndTexMatrix (GX_ITM_0, indmtx, 5);

  world_display (gdata->world);
  //shader_load (gdata->tile_shader);
  GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);
  screenspace_rect (gdata->tile_shader, GX_VTXFMT0, 0);
}

effect_methods greets_methods =
{
  .preinit_assets = NULL,
  .init_effect = &greets_init_effect,
  .prepare_frame = NULL,
  .display_effect = &greets_display_effect,
  .uninit_effect = &greets_uninit_effect,
  .finalize = NULL
};
