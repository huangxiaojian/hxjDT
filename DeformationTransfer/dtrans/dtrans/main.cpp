#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "transformer.h"
#include "mesh_seg.h"
#include "../common/triangle_corr_dict.h"


#define N_MAXCORRS 3


int main(int argc, char *argv[])
{
    dtTransformer trans;
    dtMeshModel source_deformed;
#ifdef _DEBUG
	const char 
		*source_ref = "horse_ref.obj",  /* source reference model */
		*target_ref = "camel_ref.obj",  /* target reference model */
		*tricorrs = "out.tricorrs";  /* vertex constraints specified with Corres!
											 */
	//char str[] = "horse-0.obj";
	char src_deformed[][FILENAME_MAX] = {"horse-0.obj"};
#else
    const char 
        *source_ref = argv[1],  /* filename of source reference model */
        *target_ref = argv[2],  /* filename of target reference model */
        *tricorrs   = argv[3];  /* filename of triangle correspondence */

	char **src_deformed = &argv[4]; /* deformed source mesh filenames */
#endif
    

    /* number of deformed source model files specified in command line */
#ifdef _DEBUG
	dt_size_type n_deformed_source = 1;
#else
    dt_size_type n_deformed_source = argc - 4;
#endif
    dt_index_type i_source = 0;

    char deformed_mesh_name[FILENAME_MAX];  /* deformed target mesh filename */
#ifdef _DEBUG
	if(1)
#else
    if (argc > 3)
#endif
    {
        __dt_CHOLMOD_start();

        /* Create a transformer object for deforming the target mesh using 
           source mesh deformations */
        printf("reading data...\n");
        CreateDeformationTransformer(
            source_ref, target_ref, tricorrs, N_MAXCORRS, &trans);

        /* Transfer the deformation of each deformed source mesh to the target
           mesh, so that the target mesh would deform like the source mesh  */
        for ( ; i_source < n_deformed_source; i_source++)
        {
            /* read deformed source model */
            printf("loading source deformed meshes...\n");
            __dt_ReadObjFile_commit_or_crash(
                src_deformed[i_source], &source_deformed);

            /* deform the target model like source_ref=>source_deformed */
            printf("deforming...\n");
            Transform2TargetMeshModel(&source_deformed, &trans);

            /* save deformed target mesh to file: out_##.obj */
            printf("deformation complete, save deformed mesh to file\n");
            /*snprintf(
                deformed_mesh_name, sizeof(deformed_mesh_name), 
                "out_%d.obj", i_source);*/
			int result = _snprintf_s(
				deformed_mesh_name, sizeof(deformed_mesh_name), 
				"out_%d.obj", i_source);
			if(result == sizeof(deformed_mesh_name) || result < 0)
				deformed_mesh_name[sizeof(deformed_mesh_name)-1] = 0;
            SaveObjFile(deformed_mesh_name, &(trans.target));

            /* complete */
            DestroyMeshModel(&source_deformed);
        }

        DestroyDeformationTransformer(&trans);
        __dt_CHOLMOD_finish();

    }
    else {
        printf(
            "usage: %s source_ref target_ref tricorres"
            " <one or more deformed source model>\n", argv[0]);
    }

	getchar();
    return 0;
}
