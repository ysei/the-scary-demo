#ifndef PUMPKIN_H
#define PUMPKIN_H 1

#include "shader.h"
#include "object.h"

typedef struct
{
  shader_info *pumpkin_lighting_shader;
  GXTexObj pumpkin_tex_obj;

  shader_info *beam_z_render_shader;
  void *beams_texture_gb;
  void *beams_texture_r;
  void *beams_z_texture;
  GXTexObj beams_gb_tex_obj;
  GXTexObj beams_r_tex_obj;
  GXTexObj beams_z_tex_obj;
  GXTexObj gradient_tex_obj;
  
  Mtx modelview;
  object_loc pumpkin_loc;
  object_loc beam_loc;
  
  shader_info *beam_lighting_shader;

  float vpos[3];
  float spd[3];
  
  float central_pumpkin_rotation;
} pumpkin_data;

extern effect_methods pumpkin_methods;
extern pumpkin_data pumpkin_data_0;

#endif
