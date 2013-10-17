#ifndef GLOBAL_H
#define GLOBAL_H

#define WIDTH 320
#define HEIGHT 240

#define CAM_NUM 4
#define DIRECT_VIEW_FPS 30
#define ENCODE_FPS 10


#ifdef UNROLL_LOOPS
#define LOOP_UNROLL_LESS( CODE ) \
CODE \
CODE
#else
#define LOOP_UNROLL_LESS( CODE ) CODE
#endif



#ifdef UNROLL_LOOPS
#define LOOP_UNROLL_MORE( CODE ) \
CODE \
CODE \
CODE \
CODE \
CODE \
CODE \
CODE \
CODE

/* should always be 2 fewer unrolls than LOOP_UNROLL_MORE */
#define LOOP_UNROLL_MORE2( CODE ) \
CODE \
CODE \
CODE \
CODE \
CODE \
CODE

/* should always be 4 fewer unrolls than LOOP_UNROLL_MORE */
#define LOOP_UNROLL_MORE4( CODE ) \
CODE \
CODE \
CODE \
CODE
#else
#define LOOP_UNROLL_MORE( CODE ) CODE
#define LOOP_UNROLL_MORE2( CODE ) CODE
#define LOOP_UNROLL_MORE4( CODE ) CODE
#endif





#endif
