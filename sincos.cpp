#define _USE_MATH_DEFINES
#include <math.h>
#include "sincos.h"
#include "mathutils.h"

constexpr int NUM_ENTRIES = (2046);

static float sin_table[NUM_ENTRIES];
static float cos_table[NUM_ENTRIES];

constexpr float inc = D3DX_TAU / NUM_ENTRIES;

void make_sin_cos_tables()
{
    // calculate sine lookup table
    float radian=0;
    for (int i = 0; i < NUM_ENTRIES; i++)
    {
    	sin_table[i] = sin(radian);
      	radian += inc;
    }

    // calculate cosine lookup table
    radian = 0;
    for (int i = 0; i < NUM_ENTRIES; i++)
    {
    	cos_table[i] = cos(radian);
      	radian += inc;
    }
}

float get_sin(float i)
{
    i = mathutils::wrapRadians(i);
    i = i / D3DX_TAU;
    return sin_table[(int)(i*NUM_ENTRIES)];
}

float get_cos(float i)
{
    i = mathutils::wrapRadians(i);
    i = i / D3DX_TAU;
    return cos_table[(int)(i*NUM_ENTRIES)];
}
