#ifndef __YUV2RGB_H
#define __YUV2RGB_H

#include "vislib.h"

#define limit(x) ( (x > 255) ? 255 : ( (x < 0) ? 0 : x ) )

#define CLIP        320
#define RED_NULL    128
#define BLUE_NULL   128
#define LUN_MUL     256
#define RED_MUL     512
#define BLUE_MUL    512

#define GREEN1_MUL  (-RED_MUL/2)
#define GREEN2_MUL  (-BLUE_MUL/6)
#define RED_ADD     (-RED_NULL  * RED_MUL)
#define BLUE_ADD    (-BLUE_NULL * BLUE_MUL)
#define GREEN1_ADD  (-RED_ADD/2)
#define GREEN2_ADD  (-BLUE_ADD/6)



#define Gray(val)               ng_yuv_gray[val]
#define Red(gray,red)           ng_clip[ CLIP + gray + ng_yuv_red[red] ]
#define Green(gray,red,blue)    ng_clip[ CLIP + gray + ng_yuv_g1[red] + \
                                                      ng_yuv_g2[blue] ]
#define Blue(gray,blue)         ng_clip[ CLIP + gray + ng_yuv_blue[blue] ]


void yuv2rgb_init(void);
void yuv2rgb(unsigned char*, vlPixel*);

#endif

