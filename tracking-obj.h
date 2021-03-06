#ifndef TRACKING_OBJ_H
#define TRACKING_OBJ_H 1

#include <ogcsys.h>
#include <gccore.h>

#include "spline.h"

typedef enum
{
  TRACKING_OBJ_STATIC,
  TRACKING_OBJ_PATH
} tracking_obj_type;

typedef struct
{
  tracking_obj_type type;
} tracking_obj_base;

typedef struct
{
  tracking_obj_base base;
  float pos[3];
} static_tracking_obj;

typedef struct
{
  tracking_obj_base base;
  spline_info spline;
  float *anim_data;
  int anim_frames;
  float anim_start;
  float anim_end;
} spline_tracking_obj;

extern void tracking_obj_pos (guVector *, tracking_obj_base *, float param);

#endif
