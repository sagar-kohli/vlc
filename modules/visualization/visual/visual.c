/*****************************************************************************
 * visual.c : Visualisation system
 *****************************************************************************
 * Copyright (C) 2002 VideoLAN
 * $Id: visual.c,v 1.5 2003/09/09 23:35:28 hartman Exp $
 *
 * Authors: Cl�ment Stenac <zorglub@via.ecp.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 *****************************************************************************/

/*****************************************************************************
 * Preamble
 *****************************************************************************/
#include "visual.h"

#include <vlc/vlc.h>

/*****************************************************************************
 * Local prototypes
 *****************************************************************************/
static int  Open         ( vlc_object_t * );             
static void Close        ( vlc_object_t * );                   

static void DoWork    ( aout_instance_t *, aout_filter_t *, aout_buffer_t *,
                                        aout_buffer_t * );

/*****************************************************************************
 * Module descriptor
 *****************************************************************************/
#define ELIST_TEXT N_( "Effects list" )
#define ELIST_LONGTEXT N_( \
      "A list of visual effect, separated by commas.\n
      "Current effects include: dummy, random, scope, spectrum" )

#define WIDTH_TEXT N_( "Video width" )
#define HEIGHT_LONGTEXT N_( \
      "The width of the effects video window, in pixels." )

#define HEIGHT_TEXT N_( "Video width" )
#define WIDTH_LONGTEXT N_( \
      "The width of the effects video window, in pixels." )

#define NBBANDS_TEXT N_( "Number of bands" )
#define NBBANDS_LONGTEXT N_( \
      "Number of bands used by spectrum analizer, should be 20 or 80" )

#define SEPAR_TEXT N_( "Band separator" )
#define SEPAR_LONGTEXT N_( \
        "Number of blank pixels between bands")
      
#define AMP_TEXT N_( "Amplification" )
#define AMP_LONGTEXT N_( \
        "This is a coefficient that modifies the height of the bands")
      
#define PEAKS_TEXT N_( "Enable peaks" )
#define PEAKS_LONGTEXT N_( \
        "Defines whether to draw peaks" )

#define STARS_TEXT N_( "Number of stars" )
#define STARS_LONGTEXT N_( \
        "Defines the number of stars to draw with random effect" )

//static char *effect_list[] = { "dummy", "random", "scope", "spectrum", NULL };

vlc_module_begin();
    add_category_hint( N_("visualizer") , NULL , VLC_FALSE);
    set_description( _("visualizer filter") ); 
    add_string("effect-list", "dummy", NULL,
            ELIST_TEXT, ELIST_LONGTEXT, VLC_TRUE );
    add_integer("effect-width",VOUT_WIDTH,NULL,
             WIDTH_TEXT, WIDTH_LONGTEXT, VLC_FALSE );
    add_integer("effect-height" , VOUT_HEIGHT , NULL,
             HEIGHT_TEXT, HEIGHT_LONGTEXT, VLC_FALSE );
    add_integer("visual-nbbands", 80, NULL,
             NBBANDS_TEXT, NBBANDS_LONGTEXT, VLC_FALSE );
    add_integer("visual-separ", 1, NULL,
             SEPAR_TEXT, SEPAR_LONGTEXT, VLC_FALSE );
    add_integer("visual-amp", 3, NULL,
             AMP_TEXT, AMP_LONGTEXT, VLC_FALSE );
    add_bool("visual-peaks", VLC_TRUE, NULL,
             PEAKS_TEXT, PEAKS_LONGTEXT, VLC_FALSE );
    add_integer("visual-stars", 200, NULL,
             STARS_TEXT, STARS_LONGTEXT, VLC_FALSE );
    set_capability( "audio filter", 0 );
    set_callbacks( Open, Close );
    add_shortcut( "visualizer");
vlc_module_end();

/*****************************************************************************
 * Open: open the visualizer
 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    aout_filter_t *p_filter = (aout_filter_t *)p_this;

    char *psz_effects;
    char  *psz_eof; /* Used to parse effect list */
    char *psz_eoa, *psz_boa;     /* Used to parse arg lists */
     
    vlc_bool_t b_end = VLC_FALSE;
    
    visual_effect_t *p_current_effect = NULL;

    if(  (p_filter->input.i_format != VLC_FOURCC('f','l','3','2')
          && p_filter->input.i_format != VLC_FOURCC('f','i','3','2')) )
    {
            return -1;
    }

    p_filter->p_sys = malloc(sizeof(struct aout_filter_sys_t));
    if ( p_filter->p_sys == NULL )
    {
        msg_Err( p_filter, "out of memory" );
        return -1;
    }

    p_filter->p_sys->i_height = config_GetInt( p_filter , "effect-height");
    p_filter->p_sys->i_width  = config_GetInt( p_filter , "effect-width");

    if ( p_filter->p_sys->i_height < 20 ) 
        p_filter->p_sys->i_height =  20;
    if ( p_filter->p_sys->i_width < 20 ) 
        p_filter->p_sys->i_width =  20;
    
    if( (p_filter->p_sys->i_height % 2 ) != 0 )
        p_filter->p_sys->i_height --;
   
    if( (p_filter->p_sys->i_width % 2 ) != 0 )
        p_filter->p_sys->i_width --;
    
    /* Parse the effect list */
    psz_effects = config_GetPsz( p_filter, "effect-list" );
    msg_Dbg( p_filter , "Building list of effects" );
    
    p_filter->p_sys->p_first_effect = 
            (visual_effect_t *)malloc( sizeof( visual_effect_t ) );
    if( !p_filter->p_sys->p_first_effect ) 
    {
        msg_Err( p_filter, "Out of memory" );
        return -1;
    }

    p_current_effect = p_filter->p_sys->p_first_effect;
    p_current_effect->p_next = NULL;
    while(1)
    {
        psz_eof = strchr( psz_effects , ','  );
        if( !psz_eof )
        {
            b_end = VLC_TRUE;
            psz_eof = strchr( psz_effects,'\0');
        }
        if( psz_eof )
        {
            *psz_eof = '\0';
        }

        p_current_effect->p_next =
                (visual_effect_t *)malloc( sizeof( visual_effect_t ) );
        if( !p_current_effect )
        {
            msg_Err( p_filter, "Out of memory" );
        }
        p_current_effect = p_current_effect -> p_next;
        p_current_effect->pf_run = NULL;
        p_current_effect->p_next = NULL;
        p_current_effect->i_width = p_filter->p_sys->i_width;
        p_current_effect->i_height = p_filter->p_sys->i_height;
        p_current_effect->p_data = NULL;

        if(! strncasecmp(psz_effects,"dummy",5))
            p_current_effect->pf_run = dummy_Run;
        else if(! strncasecmp(psz_effects,"scope",5) )
            p_current_effect->pf_run = scope_Run;
        else if(! strncasecmp(psz_effects,"spectrum",8) )
            p_current_effect->pf_run = spectrum_Run;
        else if(! strncasecmp(psz_effects,"random",6) )
            p_current_effect->pf_run = random_Run;
#if 0
        else if(! strncasecmp(psz_effects,"blur",4) )
            p_current_effect->pf_run = blur_Run;
#endif
        p_current_effect->psz_args  = NULL;
        p_current_effect->i_nb_chans =
                aout_FormatNbChannels( &p_filter->input);
        psz_boa = strchr( psz_effects, '{' );
        if( psz_boa )
        {
            psz_eoa = strchr( psz_effects, '}');
            if( ! psz_eoa )
            {
               msg_Err( p_filter, "Unable to parse effect list. Aborting");
               return -1;
            }
            *psz_eoa = '\0';
            p_current_effect->psz_args = strdup(++psz_boa);
            psz_effects = psz_eoa;
        }
        psz_effects =  psz_eof;
        psz_effects ++;

        if( b_end == VLC_TRUE )
            break;
    }
                
    p_filter->pf_do_work = DoWork;
    p_filter->b_in_place= 1;

    /* Open the video output */
    p_filter->p_sys->p_vout =
         vout_Request( p_filter, NULL,
                         p_filter->p_sys->i_width, 
                         p_filter->p_sys->i_height,
                  VLC_FOURCC('I','4','2','0'), 
                  VOUT_ASPECT_FACTOR * p_filter->p_sys->i_width/
                  p_filter->p_sys->i_height  );        
    
    if( p_filter->p_sys->p_vout == NULL )
    {
        msg_Err( p_filter, "no suitable vout module" );
        if(p_filter->p_sys)
            free( p_filter->p_sys );
        return -1;
    }
            
    msg_Dbg(p_filter,"Visualizer initialized");
    return 0 ;
}

/******************************************************************************
 * SparseCopy: trivially downmix or upmix a buffer
 ******************************************************************************
 * Pasted from trivial.c                                                      *
 *****************************************************************************/
static void SparseCopy( s32 * p_dest, const s32 * p_src, size_t i_len,
                        int i_output_stride, int i_input_stride )
{
    int i;
    for ( i = i_len; i--; )
    {
        int j;
        for ( j = 0; j < i_output_stride; j++ )
        {
            p_dest[j] = p_src[j % i_input_stride];
        }
        p_src += i_input_stride;
        p_dest += i_output_stride;
    }
}
/*****************************************************************************
 * DoWork: convert a buffer
 *****************************************************************************
 * Audio part pasted from trivial.c
 ****************************************************************************/
static void DoWork( aout_instance_t *p_aout, aout_filter_t *p_filter,
                    aout_buffer_t *p_in_buf, aout_buffer_t *p_out_buf )
{
    picture_t *p_outpic;
       
    visual_effect_t *p_current_effect;
    
    int i_input_nb = aout_FormatNbChannels( &p_filter->input );
    int i_output_nb = aout_FormatNbChannels( &p_filter->output );
    s32 * p_dest = (s32 *)p_out_buf->p_buffer;
    s32 * p_src = (s32 *)p_in_buf->p_buffer;
    int i_index;
        
    p_out_buf->i_nb_samples = p_in_buf->i_nb_samples;
    p_out_buf->i_nb_bytes = p_in_buf->i_nb_bytes * i_output_nb / i_input_nb;

    /* First, output the sound */
    if ( (p_filter->output.i_original_channels & AOUT_CHAN_PHYSMASK)
         != (p_filter->input.i_original_channels & AOUT_CHAN_PHYSMASK)
         && (p_filter->input.i_original_channels & AOUT_CHAN_PHYSMASK)
           == (AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT) )
    {
        int i;
        /* This is a bit special. */
        if ( !(p_filter->output.i_original_channels & AOUT_CHAN_LEFT) )
        {
             p_src++;
        }
        if ( p_filter->output.i_physical_channels == AOUT_CHAN_CENTER )
        {
            /* Mono mode */
            for ( i = p_in_buf->i_nb_samples; i--; )
            {
                *p_dest = *p_src;
                p_dest++;
                p_src += 2;
            }
        }
        else
        {
            /* Fake-stereo mode */
            for ( i = p_in_buf->i_nb_samples; i--; )
            {
                *p_dest = *p_src;
                p_dest++;
                *p_dest = *p_src;
                p_dest++;
                p_src += 2;
            }
        }
    }
    else if ( p_filter->output.i_original_channels
             & AOUT_CHAN_REVERSESTEREO )
    {
        /* Reverse-stereo mode */
        int i;
        for ( i = p_in_buf->i_nb_samples; i--; )
        {
            *p_dest = p_src[1];
            p_dest++;
            *p_dest = p_src[0];
            p_dest++;
            p_src += 2;
        }
    }
    else
    {
        SparseCopy( p_dest, p_src, p_in_buf->i_nb_samples, i_output_nb,
                            i_input_nb );
    }

    /* Ok, the sound is gone, we can think about our effects */
    
    /* First, get a new picture */
    while( ( p_outpic = vout_CreatePicture( p_filter->p_sys->p_vout,
              VLC_FALSE, VLC_FALSE, 3  ) ) == NULL )
    {
            if(p_aout->b_die )
               return;
            msleep( 2 );
    }

    /* Blank the picture */
    for( i_index = 0 ; i_index < p_outpic->i_planes ; i_index++ )
    {
        memset( p_outpic->p[i_index].p_pixels, i_index ? 0x80 : 0x00,
                p_outpic->p[i_index].i_lines * p_outpic->p[i_index].i_pitch );
    }

    /* We can now call our visualization effects */
    p_current_effect = p_filter->p_sys->p_first_effect;

    while( p_current_effect )  
    {

#if 1
        /* FIXME: Find why it segfaults when we directly call
         * p_current_effect->pf_run(....)
         *  (segfault in errno()  ) */
        if( p_current_effect->pf_run == dummy_Run )
        {
            dummy_Run(p_current_effect, p_aout, p_out_buf , p_outpic );
        }
        else if (p_current_effect->pf_run == scope_Run )
        {
            scope_Run(p_current_effect, p_aout, p_out_buf , p_outpic );
        }
        else if (p_current_effect->pf_run == random_Run )
        {
            random_Run(p_current_effect, p_aout, p_out_buf , p_outpic );
        }
        else if (p_current_effect->pf_run == spectrum_Run )
        {
            spectrum_Run(p_current_effect, p_aout, p_out_buf , p_outpic );
        }
#if 0
        else if (p_current_effect->pf_run == blur_Run )
        {
            blur_Run(p_current_effect, p_aout, p_out_buf , p_outpic );
        }
#endif
#else
        p_current_effect->pf_run(p_current_effect, p_aout, p_out_buf , p_outpic );
#endif
        p_current_effect = p_current_effect->p_next;
    }
   
    vout_DatePicture ( p_filter->p_sys->p_vout, p_outpic,p_in_buf->start_date);
    
    
    vout_DisplayPicture ( p_filter->p_sys->p_vout, p_outpic );

}

/*****************************************************************************
 * Close: close the plugin
 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    aout_filter_t * p_filter = (aout_filter_t *)p_this;

    visual_effect_t *p_old;
    visual_effect_t *p_cur = p_filter->p_sys->p_first_effect;
    
    if( p_filter->p_sys->p_vout ) 
    {
        vlc_object_detach( p_filter->p_sys->p_vout) ;
        vout_Destroy( p_filter->p_sys->p_vout );
    }
   
    /* Free the list */ 
    while( p_cur )
    {
        p_old = p_cur;
        p_cur = p_cur->p_next;
        if( p_old ) free( p_old );
    }
                    
    if( p_filter->p_sys != NULL )
        free( p_filter->p_sys);
}
