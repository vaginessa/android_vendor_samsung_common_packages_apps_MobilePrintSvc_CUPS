/* Copyright (C) 2001-2006 Artifex Software, Inc.
   All Rights Reserved.
  
  This file is part of GNU ghostscript

  GNU ghostscript is free software; you can redistribute it and/or
  modify it under the terms of the version 2 of the GNU General Public
  License as published by the Free Software Foundation.

  GNU ghostscript is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  ghostscript; see the file COPYING. If not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

*/

/* $Id: gxcspace.h,v 1.11 2009/04/19 13:54:28 Arabidopsis Exp $ */
/* Implementation of color spaces */
/* Requires gsstruct.h */

#ifndef gxcspace_INCLUDED
#  define gxcspace_INCLUDED

#include "gscspace.h"		/* client interface */
#include "gsccolor.h"
#include "gscsel.h"
#include "gxfrac.h"		/* for concrete colors */
#include "gxcindex.h"           /* for gx_color_index  */

/* Define opaque types. */

#ifndef gx_device_color_DEFINED
#  define gx_device_color_DEFINED
typedef struct gx_device_color_s gx_device_color;
#endif

#ifndef gx_device_DEFINED
#  define gx_device_DEFINED
typedef struct gx_device_s gx_device;
#endif

#ifndef stream_DEFINED
#  define stream_DEFINED
typedef struct stream_s stream;
#endif

/* Color space types (classes): */
/*typedef struct gs_color_space_type_s gs_color_space_type; */
struct gs_color_space_type_s {

    gs_color_space_index index;

    /*
     * Define whether the space can be the base space for an Indexed
     * color space or the alternate space for a Separation or DeviceN
     * color space.
     */

    bool can_be_base_space;
    bool can_be_alt_space;

    /*
     * Define the true structure type for this variant of the color
     * space union.
     */

    gs_memory_type_ptr_t stype;

    /* ------ Procedures ------ */

    /*
     * Define the number of components in a color of this space.  For
     * Pattern spaces, where the number of components depends on the
     * underlying space, this value is -1 for colored Patterns,
     * -N-1 for uncolored Patterns, where N is the number of components
     * in the base space.
     */

#define cs_proc_num_components(proc)\
  int proc(const gs_color_space *)
#define cs_num_components(pcs)\
  (*(pcs)->type->num_components)(pcs)
	cs_proc_num_components((*num_components));

    /* Construct the initial color value for this space. */

#define cs_proc_init_color(proc)\
  void proc(gs_client_color *, const gs_color_space *)
#define cs_init_color(pcc, pcs)\
  (*(pcs)->type->init_color)(pcc, pcs)
#define cs_full_init_color(pcc, pcs)\
  ((pcc)->pattern = 0, cs_init_color(pcc, pcs))
	cs_proc_init_color((*init_color));

    /* Force a client color into its legal range. */

#define cs_proc_restrict_color(proc)\
  void proc(gs_client_color *, const gs_color_space *)
#define cs_restrict_color(pcc, pcs)\
  ((pcs)->type->restrict_color(pcc, pcs))
	cs_proc_restrict_color((*restrict_color));

    /* Return the concrete color space underlying this one. */
    /* (Not defined for Pattern spaces.) */

#define cs_proc_concrete_space(proc)\
  const gs_color_space *proc(const gs_color_space *,\
				const gs_imager_state *)
#define cs_concrete_space(pcs, pis)\
  (*(pcs)->type->concrete_space)(pcs, pis)
	cs_proc_concrete_space((*concrete_space));

    /*
     * Reduce a color to a concrete color.  A concrete color is one
     * that the device can handle directly (possibly with halftoning):
     * a DeviceGray/RGB/CMYK/Pixel color, or a Separation or DeviceN
     * color that does not use the alternate space.
     * (Not defined for Pattern spaces.)
     */

#define cs_proc_concretize_color(proc)\
  int proc(const gs_client_color *, const gs_color_space *,\
    frac *, const gs_imager_state *)
#define cs_concretize_color(pcc, pcs, values, pis)\
  (*(pcs)->type->concretize_color)(pcc, pcs, values, pis)
	cs_proc_concretize_color((*concretize_color));

    /* Map a concrete color to a device color. */
    /* (Only defined for concrete color spaces.) */

#define cs_proc_remap_concrete_color(proc)\
  int proc(const frac *, const gs_color_space * pcs, gx_device_color *,\
	const gs_imager_state *, gx_device *, gs_color_select_t)
	cs_proc_remap_concrete_color((*remap_concrete_color));

    /* Map a color directly to a device color. */

#define cs_proc_remap_color(proc)\
  int proc(const gs_client_color *, const gs_color_space *,\
    gx_device_color *, const gs_imager_state *, gx_device *,\
    gs_color_select_t)
	cs_proc_remap_color((*remap_color));

    /* Install the color space in a graphics state. */

#define cs_proc_install_cspace(proc)\
  int proc(gs_color_space *, gs_state *)
	cs_proc_install_cspace((*install_cspace));

    /*
     * Push the appropriate overprint compositor onto the current device.
     * This is distinct from install_cspace as it may need to be called
     * when the overprint parameter is changed.
     *
     * This routine need only be called if:
     *   1. The color space or color model has changed, and overprint
     *      is true.
     *   2. The overprint mode setting has changed, and overprint is true.
     *   3. The overprint mode setting has changed.
     */

#define cs_proc_set_overprint(proc)\
  int proc(const gs_color_space *, gs_state *)
	cs_proc_set_overprint((*set_overprint));

    /* Free contents of composite colorspace objects. */

#define cs_proc_final(proc)\
  void proc(const gs_color_space *)
	cs_proc_final((*final));

    /* Adjust reference counts of indirect color components. */
    /*
     * Note: the color space argument may be NULL, which indicates that the
     * caller warrants that any subsidiary colors don't have allocation
     * issues.  This is a hack for an application that needs to be able to
     * release Pattern colors.
     */

#define cs_proc_adjust_color_count(proc)\
  void proc(const gs_client_color *, const gs_color_space *, int)
#define cs_adjust_color_count(pgs, delta)\
  (*(pgs)->color_space->type->adjust_color_count)\
    ((pgs)->ccolor, (pgs)->color_space, delta)
	cs_proc_adjust_color_count((*adjust_color_count));

/* Adjust both reference counts. */
#define cs_adjust_counts(pgs, delta)\
    cs_adjust_color_count(pgs, delta);					\
	rc_adjust_const(pgs->color_space, delta, "cs_adjust_counts")

    /* Serialization. */
    /*
     * Note : We don't include *(pcs)->type into serialization,
     * because we assume it is a static constant to be processed separately.
     */

#define cs_proc_serialize(proc)\
  int proc(const gs_color_space *, stream *)
#define cs_serialize(pcs, s)\
  (*(pcs)->type->serialize)(pcs, s)
	cs_proc_serialize((*serialize));

    /* A color mapping linearity check. */

#define cs_proc_is_linear(proc)\
  int proc(const gs_color_space *cs, const gs_imager_state * pis,\
		gx_device *dev,\
		const gs_client_color *c0, const gs_client_color *c1,\
		const gs_client_color *c2, const gs_client_color *c3,\
		float smoothness)
#define cs_is_linear(pcs, pis, dev, c0, c1, c2, c3, smoothness)\
  (*(pcs)->type->is_linear)(pcs, pis, dev, c0, c1, c2, c3, smoothness)
	cs_proc_is_linear((*is_linear));
};

extern_st(st_base_color_space);
#define public_st_base_color_space()	/* in gscspace.c */\
  gs_public_st_simple(st_base_color_space, gs_color_space,\
    "gs_base_color_space")

/* Standard color space procedures */
cs_proc_num_components(gx_num_components_1);
cs_proc_num_components(gx_num_components_3);
cs_proc_num_components(gx_num_components_4);
cs_proc_init_color(gx_init_paint_1);
cs_proc_init_color(gx_init_paint_3);
cs_proc_init_color(gx_init_paint_4);
cs_proc_restrict_color(gx_restrict01_paint_1);
cs_proc_restrict_color(gx_restrict01_paint_3);
cs_proc_restrict_color(gx_restrict01_paint_4);
cs_proc_concrete_space(gx_no_concrete_space);
cs_proc_concrete_space(gx_same_concrete_space);
cs_proc_concretize_color(gx_no_concretize_color);
cs_proc_remap_color(gx_default_remap_color);
cs_proc_install_cspace(gx_no_install_cspace);
cs_proc_set_overprint(gx_spot_colors_set_overprint);
cs_proc_adjust_color_count(gx_no_adjust_color_count);
cs_proc_serialize(gx_serialize_cspace_type);
cs_proc_is_linear(gx_cspace_no_linear);
cs_proc_is_linear(gx_cspace_is_linear_default);

/*
 * Define the implementation procedures for the standard device color
 * spaces.  These are implemented in gxcmap.c.
 */
cs_proc_remap_color(gx_remap_DeviceGray);
cs_proc_concretize_color(gx_concretize_DeviceGray);
cs_proc_remap_concrete_color(gx_remap_concrete_DGray);
cs_proc_remap_color(gx_remap_DeviceRGB);
cs_proc_concretize_color(gx_concretize_DeviceRGB);
cs_proc_remap_concrete_color(gx_remap_concrete_DRGB);
cs_proc_remap_color(gx_remap_DeviceCMYK);
cs_proc_concretize_color(gx_concretize_DeviceCMYK);
cs_proc_remap_concrete_color(gx_remap_concrete_DCMYK);

/* Define the allocator type for color spaces. */
extern_st(st_color_space);

/* Allocate a new color space object. Used by color space implementations. */
gs_color_space *
gs_cspace_alloc(gs_memory_t *mem, const gs_color_space_type *pcstype);

/* Determine if the current color model is a "DeviceCMYK" color model, and */
/* if so what are its process color components. */
gx_color_index check_cmyk_color_model_comps(gx_device * dev);

#endif /* gxcspace_INCLUDED */
