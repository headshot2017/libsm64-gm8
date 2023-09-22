#define _CRT_SECURE_NO_WARNINGS 1 // for fopen

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

#include "../libsm64.h"

#include "audio.h"
#include "level.h"

uint8_t *utils_read_file_alloc( const char *path, size_t *fileLength )
{
    FILE *f = fopen( path, "rb" );

    if( !f ) return NULL;

    fseek( f, 0, SEEK_END );
    size_t length = (size_t)ftell( f );
    rewind( f );
    uint8_t *buffer = (uint8_t*)malloc( length + 1 );
    fread( buffer, 1, length, f );
    buffer[length] = 0;
    fclose( f );

    if( fileLength ) *fileLength = length;

    return buffer;
}

struct SM64MarioInputs marioInputs;
struct SM64MarioState marioState;
struct SM64MarioGeometryBuffers marioGeometry;

__declspec(dllexport) double gm8_libsm64_init()
{
    size_t romSize;
    uint8_t *rom = utils_read_file_alloc( "sm64.us.z64", &romSize );

    if (!rom) return 0;

    uint8_t *texture = (uint8_t*)malloc( 4 * SM64_TEXTURE_WIDTH * SM64_TEXTURE_HEIGHT );

    sm64_global_terminate();
    sm64_global_init( rom, texture );
    sm64_audio_init(rom);
    sm64_static_surfaces_load( surfaces, surfaces_count );

    //gm8_audio_init();

    free(texture);
    free(rom);
    return 1;
}

__declspec(dllexport) double gm8_libsm64_mario_create(double x, double y, double z)
{
    int32_t marioId = sm64_mario_create( x*-1, y, z*-1 );

    marioGeometry.position = (float*)malloc( sizeof(float) * 9 * SM64_GEO_MAX_TRIANGLES );
    marioGeometry.color    = (float*)malloc( sizeof(float) * 9 * SM64_GEO_MAX_TRIANGLES );
    marioGeometry.normal   = (float*)malloc( sizeof(float) * 9 * SM64_GEO_MAX_TRIANGLES );
    marioGeometry.uv       = (float*)malloc( sizeof(float) * 6 * SM64_GEO_MAX_TRIANGLES );
    marioGeometry.numTrianglesUsed = 0;

    return (double)marioId;
}

__declspec(dllexport) double gm8_libsm64_mario_set_input(double id, double A, double B, double Z, double camX, double camZ, double stickX, double stickY)
{
    marioInputs.buttonA = (uint8_t)A;
    marioInputs.buttonB = (uint8_t)B;
    marioInputs.buttonZ = (uint8_t)Z;
    marioInputs.camLookX = marioState.position[0] - camX;
    marioInputs.camLookZ = marioState.position[2] - camZ;
    marioInputs.stickX = stickX;
    marioInputs.stickY = stickY;

    return 0;
}

__declspec(dllexport) double gm8_libsm64_mario_tick(double id)
{
    sm64_mario_tick((int)id, &marioInputs, &marioState, &marioGeometry);
    return 1;
}

__declspec(dllexport) double gm8_libsm64_mario_get_triangles_used(double id)
{
    return marioGeometry.numTrianglesUsed;
}

__declspec(dllexport) double gm8_libsm64_mario_get_posX(double id)
{
    return marioState.position[0]*-1;
}

__declspec(dllexport) double gm8_libsm64_mario_get_posY(double id)
{
    return marioState.position[1];
}

__declspec(dllexport) double gm8_libsm64_mario_get_posZ(double id)
{
    return marioState.position[2];
}

__declspec(dllexport) double gm8_libsm64_mario_get_geometry_posX(double id, double triangleVertex)
{
    return marioGeometry.position[(int)triangleVertex+0]*-1;
}

__declspec(dllexport) double gm8_libsm64_mario_get_geometry_posY(double id, double triangleVertex)
{
    return marioGeometry.position[(int)triangleVertex+1];
}

__declspec(dllexport) double gm8_libsm64_mario_get_geometry_posZ(double id, double triangleVertex)
{
    return marioGeometry.position[(int)triangleVertex+2];
}

__declspec(dllexport) double gm8_libsm64_mario_get_geometry_normalX(double id, double triangleVertex)
{
    return marioGeometry.normal[(int)triangleVertex+0]*-1;
}

__declspec(dllexport) double gm8_libsm64_mario_get_geometry_normalY(double id, double triangleVertex)
{
    return marioGeometry.normal[(int)triangleVertex+1];
}

__declspec(dllexport) double gm8_libsm64_mario_get_geometry_normalZ(double id, double triangleVertex)
{
    return marioGeometry.normal[(int)triangleVertex+2];
}

__declspec(dllexport) double gm8_libsm64_mario_get_geometry_colorRed(double id, double triangleVertex)
{
    return marioGeometry.color[(int)triangleVertex+0]*255;
}

__declspec(dllexport) double gm8_libsm64_mario_get_geometry_colorGreen(double id, double triangleVertex)
{
    return marioGeometry.color[(int)triangleVertex+1]*255;
}

__declspec(dllexport) double gm8_libsm64_mario_get_geometry_colorBlue(double id, double triangleVertex)
{
    return marioGeometry.color[(int)triangleVertex+2]*255;
}

__declspec(dllexport) double gm8_libsm64_mario_get_geometry_uvX(double id, double vertex)
{
    return marioGeometry.uv[(int)vertex+0];
}

__declspec(dllexport) double gm8_libsm64_mario_get_geometry_uvY(double id, double vertex)
{
    return marioGeometry.uv[(int)vertex+1];
}
