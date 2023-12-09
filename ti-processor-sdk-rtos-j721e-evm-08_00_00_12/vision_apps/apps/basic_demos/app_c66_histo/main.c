#include <TI/tivx.h>
#include <TI/tivx_ow_algos.h>
#include <tivx_utils_file_rd_wr.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_C66_CORES 2

#define NUM_RUN_CNT 10

#define IN_FILE_NAME       "/opt/vision_apps/colors.bmp"
//#define IN_OW_FILE_NAME    "/opt/vision_apps/test_2720x1450_10bit_bayer.y"
#define IN_OW_FILE_NAME    "/opt/vision_apps/csix_raw_output_ch_0_fr_5038_exp0_2720x1450_10bit_bayer.y"
//#define IN_OW_FILE_NAME    "/opt/vision_apps/csix_raw_output_ch_0_fr_1048_exp0_2720x1450_10bit_bayer.y"
#define MAX_ATTRIBUTE_NAME (32u)
#define APP_MAX_FILE_PATH           (256u)
#define APP_ASSERT(x)               assert((x))
#define APP_ASSERT_VALID_REF(ref)   (APP_ASSERT(vxGetStatus((vx_reference)(ref))==VX_SUCCESS));

typedef struct {
    uint32_t statusFlag;
} tivxHistoParams;

typedef struct {

    /* config options */
    vx_user_data_object config;

    /* OpenVX references */
    vx_context context;
    vx_graph graph[NUM_C66_CORES];
    vx_node  node[NUM_C66_CORES];
    vx_image input_img;
    vx_distribution output_histo;
    tivx_raw_image raw_img;
    vx_scalar mean;
    vx_scalar sd;

} AppObj;

const char *targetCore[NUM_C66_CORES] = {TIVX_TARGET_DSP1, TIVX_TARGET_DSP2};
static int core_ping_pong = 0;

AppObj gAppObj;
uint32_t g_num_bins = 1024;

void show_image_attributes(vx_image image);
void show_node_attributes(vx_node node);

static vx_status app_init(AppObj *obj);
static vx_status app_deinit(AppObj *obj);
static vx_status app_create_graph(AppObj *obj);
static vx_status app_run_graph(AppObj *obj, int instance);
static void app_delete_graph(AppObj *obj);

int app_c66_histo_main(int argc, char* argv[])
{
    AppObj *obj = &gAppObj;
    vx_status status = VX_SUCCESS;
    printf("Executing app_c66_histo_main\n");

    app_init(obj);
    if(status == VX_SUCCESS)
    {
        status = app_create_graph(obj);
    }

    int i = 0;
    core_ping_pong = 0;

    for( i = 0; i < NUM_RUN_CNT; i++ )
    {
        if(status == VX_SUCCESS)
        {
            status = app_run_graph(obj, i);
            core_ping_pong = (core_ping_pong == 0 ? 1 : 0);
        }
    }

    app_delete_graph(obj);
    if(status == VX_SUCCESS)
    {
        status = app_deinit(obj);
    }

    return status;
}


/**
 * \brief Show attributes of previously created node
 *
 * This function queries the vx_node node for its status, number of parameters,
 * performance, reference name and reference count then prints this information.
 *
 * \param node [in] Previouly created graph object
 *
 */
void show_node_attributes(vx_node node)
{
    vx_uint32 num_params=0, ref_count=0;
    vx_status status=(vx_status)VX_FAILURE;
    vx_perf_t perf={0};
    vx_char *ref_name=NULL;
    char ref_name_invalid[MAX_ATTRIBUTE_NAME];
    char status_name[MAX_ATTRIBUTE_NAME];
    
    printf("show_node_attributes\n");

    /** - Query node attributes.
     *
     *  Queries node for number of status, number of parameters and performance
     *
     * \code
     */
    vxQueryNode(node, (vx_enum)VX_NODE_STATUS, &status, sizeof(vx_status));
    vxQueryNode(node, (vx_enum)VX_NODE_PARAMETERS, &num_params, sizeof(vx_uint32));
    vxQueryNode(node, (vx_enum)VX_NODE_PERFORMANCE, &perf, sizeof(vx_perf_t));
    /** \endcode */

    vxQueryReference((vx_reference)node, (vx_enum)VX_REFERENCE_NAME, &ref_name, sizeof(vx_char*));
    vxQueryReference((vx_reference)node, (vx_enum)VX_REFERENCE_COUNT, &ref_count, sizeof(vx_uint32));

    switch(status)
    {
        case (vx_status)VX_SUCCESS:
            strncpy(status_name, "VX_SUCCESS", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_status)VX_FAILURE:
            strncpy(status_name, "VX_FAILURE", MAX_ATTRIBUTE_NAME);
            break;
        default:
            strncpy(status_name, "VX_FAILURE_OTHER", MAX_ATTRIBUTE_NAME);
            break;
    }

    if(ref_name==NULL)
    {
        strncpy(ref_name_invalid, "INVALID_REF_NAME", MAX_ATTRIBUTE_NAME);
        ref_name = &ref_name_invalid[0];
    }

    printf(" VX_TYPE_NODE: %s, %d params, avg perf %9.6fs, %s, %d refs\n",
        ref_name,
        num_params,
        perf.avg/1000000000.0,
        status_name,
        ref_count
        );
}

/**
 * \brief Show attributes of previously created image
 *
 * This function queries the vx_image image for its number of nodes, number of parameters,
 * state, performance, reference name and reference count then prints this information.
 *
 * \param image [in] Previouly created image object
 *
 */
void show_image_attributes(vx_image image)
{
    vx_uint32 width=0, height=0, ref_count=0;
    vx_df_image df=0;
    vx_size num_planes=0, size=0;
    vx_enum color_space=0, channel_range=0, memory_type=0;
    vx_char *ref_name=NULL;
    char df_name[MAX_ATTRIBUTE_NAME];
    char color_space_name[MAX_ATTRIBUTE_NAME];
    char channel_range_name[MAX_ATTRIBUTE_NAME];
    char memory_type_name[MAX_ATTRIBUTE_NAME];
    char ref_name_invalid[MAX_ATTRIBUTE_NAME];

    printf("show_image_attributes\n");

    /** - Query image attributes.
     *
     *  Queries image for width, height, format, planes, size, space, range,
     *  range and memory type.
     *
     * \code
     */
    vxQueryImage(image, (vx_enum)VX_IMAGE_WIDTH, &width, sizeof(vx_uint32));
    vxQueryImage(image, (vx_enum)VX_IMAGE_HEIGHT, &height, sizeof(vx_uint32));
    vxQueryImage(image, (vx_enum)VX_IMAGE_FORMAT, &df, sizeof(vx_df_image));
    vxQueryImage(image, (vx_enum)VX_IMAGE_PLANES, &num_planes, sizeof(vx_size));
    vxQueryImage(image, (vx_enum)VX_IMAGE_SIZE, &size, sizeof(vx_size));
    vxQueryImage(image, (vx_enum)VX_IMAGE_SPACE, &color_space, sizeof(vx_enum));
    vxQueryImage(image, (vx_enum)VX_IMAGE_RANGE, &channel_range, sizeof(vx_enum));
    vxQueryImage(image, (vx_enum)VX_IMAGE_MEMORY_TYPE, &memory_type, sizeof(vx_enum));
    /** \endcode */

    vxQueryReference((vx_reference)image, (vx_enum)VX_REFERENCE_NAME, &ref_name, sizeof(vx_char*));
    vxQueryReference((vx_reference)image, (vx_enum)VX_REFERENCE_COUNT, &ref_count, sizeof(vx_uint32));

    switch(df)
    {
        case (vx_df_image)VX_DF_IMAGE_VIRT:
            strncpy(df_name, "VX_DF_IMAGE_VIRT", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_RGB:
            strncpy(df_name, "VX_DF_IMAGE_RGB", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_RGBX:
            strncpy(df_name, "VX_DF_IMAGE_RGBX", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_NV12:
            strncpy(df_name, "VX_DF_IMAGE_NV12", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_NV21:
            strncpy(df_name, "VX_DF_IMAGE_NV21", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_UYVY:
            strncpy(df_name, "VX_DF_IMAGE_UYVY", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_YUYV:
            strncpy(df_name, "VX_DF_IMAGE_YUYV", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_IYUV:
            strncpy(df_name, "VX_DF_IMAGE_IYUV", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_YUV4:
            strncpy(df_name, "VX_DF_IMAGE_YUV4", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_U8:
            strncpy(df_name, "VX_DF_IMAGE_U8", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_U16:
            strncpy(df_name, "VX_DF_IMAGE_U16", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_S16:
            strncpy(df_name, "VX_DF_IMAGE_S16", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_U32:
            strncpy(df_name, "VX_DF_IMAGE_U32", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_df_image)VX_DF_IMAGE_S32:
            strncpy(df_name, "VX_DF_IMAGE_S32", MAX_ATTRIBUTE_NAME);
            break;
        default:
            strncpy(df_name, "VX_DF_IMAGE_UNKNOWN", MAX_ATTRIBUTE_NAME);
            break;
    }

    switch(color_space)
    {
        case (vx_enum)VX_COLOR_SPACE_NONE:
            strncpy(color_space_name, "VX_COLOR_SPACE_NONE", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_enum)VX_COLOR_SPACE_BT601_525:
            strncpy(color_space_name, "VX_COLOR_SPACE_BT601_525", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_enum)VX_COLOR_SPACE_BT601_625:
            strncpy(color_space_name, "VX_COLOR_SPACE_BT601_625", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_enum)VX_COLOR_SPACE_BT709:
            strncpy(color_space_name, "VX_COLOR_SPACE_BT709", MAX_ATTRIBUTE_NAME);
            break;
        default:
            strncpy(color_space_name, "VX_COLOR_SPACE_UNKNOWN", MAX_ATTRIBUTE_NAME);
            break;
    }

    switch(channel_range)
    {
        case (vx_enum)VX_CHANNEL_RANGE_FULL:
            strncpy(channel_range_name, "VX_CHANNEL_RANGE_FULL", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_enum)VX_CHANNEL_RANGE_RESTRICTED:
            strncpy(channel_range_name, "VX_CHANNEL_RANGE_RESTRICTED", MAX_ATTRIBUTE_NAME);
            break;
        default:
            strncpy(channel_range_name, "VX_CHANNEL_RANGE_UNKNOWN", MAX_ATTRIBUTE_NAME);
            break;
    }

    switch(memory_type)
    {
        case (vx_enum)(vx_enum)VX_MEMORY_TYPE_NONE:
            strncpy(memory_type_name, "VX_MEMORY_TYPE_NONE", MAX_ATTRIBUTE_NAME);
            break;
        case (vx_enum)VX_MEMORY_TYPE_HOST:
            strncpy(memory_type_name, "VX_MEMORY_TYPE_HOST", MAX_ATTRIBUTE_NAME);
            break;
        default:
            strncpy(memory_type_name, "VX_MEMORY_TYPE_UNKNOWN", MAX_ATTRIBUTE_NAME);
            break;
    }

    if(ref_name==NULL)
    {
        strncpy(ref_name_invalid, "INVALID_REF_NAME", MAX_ATTRIBUTE_NAME);
        ref_name = &ref_name_invalid[0];
    }

    printf(" VX_TYPE_IMAGE: %s, %d x %d, %d plane(s), %d B, %s %s %s %s, %d refs\n",
        ref_name,
        width,
        height,
        (uint32_t)num_planes,
        (uint32_t)size,
        df_name,
        color_space_name,
        channel_range_name,
        memory_type_name,
        ref_count
        );
}

static vx_status app_c66_kernels_load(vx_context context)
{
    vx_status status = VX_SUCCESS;
    #ifdef x86_64
    {
    }
    #endif
    
    tivxOwAlgosLoadKernels(context);
    return status;
}

static vx_status app_c66_kernels_unload(vx_context context)
{
    vx_status status = VX_SUCCESS;
    #ifdef x86_64
    
    #endif
    if(status == VX_SUCCESS)
    {
    }
    tivxOwAlgosUnLoadKernels(context);
    return status;
}

static vx_status app_init(AppObj *obj)
{
    vx_status status = VX_SUCCESS;
    obj->context = vxCreateContext();
    status = vxGetStatus((vx_reference)obj->context);

    if(status == VX_SUCCESS)
    {
        status = app_c66_kernels_load(obj->context);
    }

    return status;
}

static vx_status app_deinit(AppObj *obj)
{
    vx_status status = VX_SUCCESS;
    status = app_c66_kernels_unload(obj->context);

    if (status == VX_SUCCESS)
    {
        status = vxReleaseContext(&obj->context);
    }
    return status;
}

static vx_status app_create_graph(AppObj *obj)
{
    vx_status status = VX_SUCCESS;

    tivx_raw_image_create_params_t params;

    tivxHistoParams histParams = {0};

    params.width = 2720;
    params.height = 1450;
    params.num_exposures = 1;
    params.line_interleaved = vx_false_e;
    params.format[0].pixel_container = TIVX_RAW_IMAGE_16_BIT;
    params.format[0].msb = 9;
    //params.format[1].pixel_container = TIVX_RAW_IMAGE_8_BIT;
    //params.format[1].msb = 7;
    //params.format[2].pixel_container = TIVX_RAW_IMAGE_P12_BIT;
    //params.format[2].msb = 11;
    params.meta_height_before = 0;
    params.meta_height_after = 0;

    //g_range = (1 << (params.format[0].msb + 1));

    int i;
    char refName[32] = {0};

    for( i = 0; i < NUM_C66_CORES; i++ )
    {
        obj->graph[i] = vxCreateGraph(obj->context);
        if(status == VX_SUCCESS)
        {
            status = vxGetStatus((vx_reference)obj->graph[i]);
        }

        if(status == VX_SUCCESS)
        {
            sprintf(refName, "MY_HISTO_GRAPH_C66_%d", i);
            vxSetReferenceName((vx_reference)obj->graph[i], "MY_HISTO_GRAPH");
        }
        else
        {
            break;
        }
    }

    if( status == VX_SUCCESS )
    {
        obj->config = vxCreateUserDataObject(obj->context, "histoConfig", sizeof(tivxHistoParams), NULL);
        status = vxGetStatus((vx_reference)obj->config);

        if (status == VX_SUCCESS)
        {
            status = vxCopyUserDataObject(obj->config, 0, sizeof(tivxHistoParams),
                                        &histParams, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
        }
        
    }

    if(status == VX_SUCCESS)
    {
        //printf("create vx image object\n");
        //obj->input_img = vxCreateImage(obj->context, 2720, 1450, VX_DF_IMAGE_U16);
        //status = vxGetStatus((vx_reference)obj->input_img);

        printf("create tivx raw image object\n");
        obj->raw_img = tivxCreateRawImage(obj->context, &params);
        status = vxGetStatus((vx_reference)obj->raw_img);
    }

    if(status == VX_SUCCESS)
    {
        printf("vxSetReferenceName for INPUT image\n");
        //status = vxSetReferenceName((vx_reference)obj->input_img, "INPUT");
        status = vxSetReferenceName((vx_reference)obj->raw_img, "INPUT");
    }

    if(status == VX_SUCCESS)
    {
        printf("vxCreateDistribution\n");
        obj->output_histo = vxCreateDistribution(obj->context, g_num_bins, 0, 1024);
    }

    if(status == VX_SUCCESS)
    {
        printf("vxSetReferenceName for HISTOGRAM output\n");
        status = vxSetReferenceName((vx_reference)obj->output_histo, "OUTPUT");
    }

    vx_uint32 fmean = 0;
    vx_uint32 fsd = 0;
    obj->mean = vxCreateScalar(obj->context, VX_TYPE_UINT32, &fmean);
    status = vxGetStatus((vx_reference)obj->mean);
    if(status == VX_SUCCESS)
    {
        VX_PRINT(VX_ZONE_INFO, "vxSetReferenceName for MEAN output\n");
        status = vxSetReferenceName((vx_reference)obj->mean, "OUT_MEAN");
    }

    if( status == VX_SUCCESS)
    {
        obj->sd = vxCreateScalar(obj->context, VX_TYPE_UINT32, &fsd);
        status = vxGetStatus((vx_reference)obj->sd);
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR,"create scalar object for mean failed!");
    }
        
    if(status == VX_SUCCESS)
    {
        VX_PRINT(VX_ZONE_INFO, "vxSetReferenceName for SD output\n");
        status = vxSetReferenceName((vx_reference)obj->sd, "OUT_SD");
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR,"create scalar object for SD failed!");
    }

    if(status == VX_SUCCESS)
    {
        int i;
        char refName[32] = {0};

        for( i = 0; i < NUM_C66_CORES; i++ )
        {
            // vx_reference refs[] = {(vx_reference)obj->input_img, (vx_reference)obj->output_histo};
            obj->node[i] = tivxOwCalcHistoNode(obj->graph[i], obj->config,
                                        obj->raw_img, obj->output_histo,
                                        obj->mean, obj->sd);
            sprintf(refName, "HISTOGRAM_C66_%d", i);
            status = vxSetReferenceName((vx_reference)obj->node[i], refName);
            if( status != VX_SUCCESS )
            {
                VX_PRINT(VX_ZONE_ERROR,"failed to create kernel node[%d] ......", i);
                break;
            }
            else
            {
                vxSetNodeTarget(obj->node[i], (vx_enum)VX_TARGET_STRING, targetCore[i]);
            }
        }
    }

    if(status == VX_SUCCESS)
    {
        status = vxVerifyGraph(obj->graph[0]);
        status |= vxVerifyGraph(obj->graph[1]);
    }

    if(status == VX_SUCCESS)
    {
        /* Do not set status here because this step is not critical */
        tivxExportGraphToDot(obj->graph[0],".", "vx_app_c66_1_histo");
        tivxExportGraphToDot(obj->graph[1],".", "vx_app_c66_2_histo");
    }

    if( status == VX_SUCCESS )
    {
        show_node_attributes(obj->node[0]);
        show_node_attributes(obj->node[1]);
    }

    return status;

}

static vx_status app_run_graph(AppObj *obj, int instance)
{
    vx_status status = VX_SUCCESS;

    printf(" Loading [%s] ...\n", IN_OW_FILE_NAME);
    // status = tivx_utils_load_vximage_from_bmpfile(obj->input_img, IN_FILE_NAME, vx_true_e);
    uint16_t* raw_data = NULL;
    uint16_t* a64_raw_data = NULL;
    FILE* fp = fopen(IN_OW_FILE_NAME, "rb");
    if (!fp) 
    {
        printf("Error: could not open file %s\n", IN_OW_FILE_NAME);
        status = VX_FAILURE;
    }
    else
    {
        raw_data = malloc((2720 * 1450 * sizeof(uint16_t)) + (2 * 64));
        a64_raw_data = raw_data;    //(uint16_t *)((uintptr_t)((unsigned long)((uintptr_t)raw_data) - ((unsigned long)((uintptr_t)raw_data) % 16) + 16u));
        printf("raw_data: %p, a64_raw_data: %p\n", raw_data, a64_raw_data);
        status = VX_SUCCESS;
    }
    
    if(status == VX_SUCCESS)
    {
        vx_uint32 width, height;
        vx_imagepatch_addressing_t image_addr;
        vx_rectangle_t rect;
        vx_map_id map_id;
        void *data_ptr;
        vx_uint32 num_bytes = 1;
        tivx_raw_image_format_t format[3];

        tivxQueryRawImage(obj->raw_img, TIVX_RAW_IMAGE_WIDTH, &width, sizeof(vx_uint32));
        tivxQueryRawImage(obj->raw_img, TIVX_RAW_IMAGE_HEIGHT, &height, sizeof(vx_uint32));
        tivxQueryRawImage(obj->raw_img, TIVX_RAW_IMAGE_FORMAT, &format, sizeof(format));

        if( format[0].pixel_container == TIVX_RAW_IMAGE_16_BIT )
        {
            num_bytes = 2;
        }
        else if( format[0].pixel_container == TIVX_RAW_IMAGE_8_BIT )
        {
            num_bytes = 1;
        }
        else if( format[0].pixel_container == TIVX_RAW_IMAGE_P12_BIT )
        {
            num_bytes = 0;
        }

        int x = 0;
        // read data in
        fread(a64_raw_data, sizeof(uint16_t), 2720 * 1450, fp);
        fclose(fp);

        // Copy raw data to image object
        image_addr.dim_x = width;
        image_addr.dim_y = height;
        image_addr.stride_x = sizeof(uint16_t);
        image_addr.stride_y = 2720 * sizeof(uint16_t);

        rect.start_x = 0;
        rect.start_y = 0;
        rect.end_x = width;
        rect.end_y = height;

        for(x = 0; x < 5; x++){
            printf("RAW Pixel: 0x%x\n", a64_raw_data[x]);        
        }

        //vxCopyImagePatch(obj->input_img, &rect, 0, &image_addr, a64_raw_data, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

        tivxMapRawImagePatch(obj->raw_img,
                    &rect,
                    0,
                    &map_id,
                    &image_addr,
                    &data_ptr,
                    VX_WRITE_ONLY,
                    VX_MEMORY_TYPE_HOST,
                    TIVX_RAW_IMAGE_PIXEL_BUFFER
                    );

        
        /* flatten the input image - row wise! */
        uint8_t *dst = data_ptr;
        uint8_t *src = (uint8_t*)a64_raw_data;
        for(int i = 0; i < height; i++)
        {
            memcpy((void *)&dst[image_addr.stride_y * i], (void *)&src[width * num_bytes * i], width * num_bytes);
        }

        tivxUnmapRawImagePatch(obj->raw_img, map_id);

        for(x = 0; x < 5; x++){
            printf("RAW Image Patch Pixel: 0x%x\n", ((uint16_t *)dst)[x]);
        }

        free(raw_data);     // free w.r.t allocated memory pointer - NOT the aligned memory pointer 
    }
    
    if(status == VX_SUCCESS)
    {
        printf(" Showing Attributes ...\n");
        //show_image_attributes(obj->input_img);
    }
    

    printf(" Running graph ...\n");
    if(status == VX_SUCCESS)
    {
        status = vxScheduleGraph(obj->graph[core_ping_pong]);
    }
    if(status == VX_SUCCESS)
    {
        status = vxWaitGraph(obj->graph[core_ping_pong]);
    }
    else
    {
        printf("vxScheduleGraph() failed!\n");
        return status;
    }
    
    // Print the histogram values
    printf("Histogram values:\n");
    vx_map_id map_id3;
    uint32_t *ptr3;
    vx_size hist_num_bins;

    status = vxQueryDistribution(obj->output_histo, VX_DISTRIBUTION_BINS, &hist_num_bins, sizeof(hist_num_bins));
    if( status != VX_SUCCESS )
    {
        printf("Distribution query for num bins failed ......\n");
    }
    else
    {
        status = vxMapDistribution(obj->output_histo, &map_id3, (void *)&ptr3, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0 /*VX_NOGAP_X*/);
        if( status == VX_SUCCESS )
        {
            char ofname[128];
            char sbuf[64];
            sprintf(ofname, "/run/media/nvme0n1/histogram_%03d", instance);
            FILE *hfp = fopen(ofname, "w");
            if( hfp != NULL )
            {
                for (vx_uint32 i = 0; i < hist_num_bins; i++) {
                    printf("%d: %d\n", i, ptr3[i]);
                    sprintf(sbuf, "%d: %d\n", i, ptr3[i]);
                    fputs(sbuf, hfp);
                }
                fclose(hfp);
            }
            vxUnmapDistribution(obj->output_histo, map_id3);
        }
        else
        {
            printf("Failed to map distribution, error: %d\n", status);
        }
    }
    

    if( status == VX_SUCCESS )
    {
        vx_float32 dm = 0.0f;

        status = vxCopyScalar(obj->mean, (void *)&dm, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
        if( status == VX_SUCCESS )
        {
            printf("Dark Rows Mean Value: %f\n", dm);
        }
        else
        {
            printf("Failed to get dark mean value ......\n");
            status = VX_FAILURE;
        }
    }

    return status;
}

static void app_delete_graph(AppObj *obj)
{
    vxReleaseNode(&obj->node[0]);
    vxReleaseNode(&obj->node[1]);
    vxReleaseGraph(&obj->graph[0]);
    vxReleaseGraph(&obj->graph[1]);
    vxReleaseUserDataObject(&obj->config);
    //vxReleaseImage(&obj->input_img);
    tivxReleaseRawImage(&obj->raw_img);
    vxReleaseDistribution(&obj->output_histo);
    vxReleaseScalar(&obj->mean);
    vxReleaseScalar(&obj->sd);
}