//
//  scene.c
//  c-ray
//
//  Created by Valtteri Koskivuori on 28/02/2015.
//  Copyright © 2015-2025 Valtteri Koskivuori. All rights reserved.
//

#include "../../includes.h"
#include "scene.h"

#include <accelerators/bvh.h>
#include <common/hashtable.h>
#include <common/textbuffer.h>
#include <common/dyn_array.h>
#include <common/node_parse.h>
#include <common/texture.h>
#include "camera.h"
#include "tile.h"
#include "mesh.h"
#include "poly.h"

void tex_asset_free(struct texture_asset *a) {
	if (a->path) free(a->path);
	if (a->t) tex_destroy(a->t);
}

void scene_destroy(struct world *scene) {
	if (scene) {
		scene->textures.elem_free = tex_asset_free;
		texture_asset_arr_free(&scene->textures);
		camera_arr_free(&scene->cameras);
		scene->meshes.elem_free = mesh_free;
		mesh_arr_free(&scene->meshes);

		thread_rwlock_wrlock(&scene->bvh_lock);
		destroy_bvh(scene->topLevel);
		thread_rwlock_unlock(&scene->bvh_lock);

		destroyHashtable(scene->storage.node_table);
		destroyBlocks(scene->storage.node_pool);

		// TODO: find out a nicer way to bind elem_free to the array init
		scene->shader_buffers.elem_free = bsdf_buffer_free;
		bsdf_buffer_arr_free(&scene->shader_buffers);

		cr_shader_node_free(scene->bg_desc);

		instance_arr_free(&scene->instances);
		sphere_arr_free(&scene->spheres);
		if (scene->asset_path) free(scene->asset_path);
		free(scene);
	}
}
