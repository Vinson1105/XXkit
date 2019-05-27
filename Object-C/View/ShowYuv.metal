//
//  ShowYuv.metal
//  XXapp
//
//  Created by VINSON on 2019/5/13.
//  Copyright © 2019 VINSON. All rights reserved.
//

#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;

kernel void yuv_rgb(texture2d<float, access::read> y_inTexture [[ texture(0) ]],
                     texture2d<float, access::read> uv_inTexture [[ texture(1) ]],
                     texture2d<float, access::write> outTexture [[ texture(2) ]],
                     uint2 gid [[ thread_position_in_grid ]]) {
    float4 yFloat4 = y_inTexture.read(gid);
    float4 uvFloat4 = uv_inTexture.read(gid/2); //这里使用yuv420格式进行像素计算
    float y = yFloat4.x;
    float u = uvFloat4.x - 0.5;
    float v = uvFloat4.y - 0.5;
    
//    float r = y + 1.4075 * (v-128);
//    if(r > 255) r = 255; if(r < 0)   r = 0;
//
//    float g = y - 0.3455 * (u-128) - 0.7169 * (v - 128);
//    if(g > 255) g = 255; if(g < 0)   g = 0;
//
//    float b = y + 1.779 * (u-128);
//    if(b > 255) b = 255; if(b < 0)   b = 0;
    
    
    float r = y + 1.403 * v;
    r = (r < 0.0) ? 0.0 : ((r > 1.0) ? 1.0 : r);
    //r = 1 - r;
    float g = y - 0.343 * u - 0.714 * v;
    g = (g < 0.0) ? 0.0 : ((g > 1.0) ? 1.0 : g);
    //g = 1 - g;
    float b = y + 1.770 * u;
    b = (b < 0.0) ? 0.0 : ((b > 1.0) ? 1.0 : b);
    //b = 1 - b;
    outTexture.write(float4(r, g, b, 1.0), gid);
}

kernel void gamma_filter(
                         texture2d<float, access::read> inTexture [[texture(0)]],
                         texture2d<float, access::write> outTexture [[texture(1)]],
                         uint2 gid [[thread_position_in_grid]])
{
    float4 inColor = inTexture.read(gid);
    const float4 outColor = float4(pow(inColor.rgb, float3(0.4/* gamma校正参数 */)), inColor.a);
    outTexture.write(outColor, gid);
}


//
//
//
//
//
typedef struct{
    vector_float4 position;     // 顶点坐标
    vector_float2 coordinate;   // 纹理坐标
} Vertex;

typedef struct {
    matrix_float3x3 matrix;     // 转换矩阵
    vector_float3 offset;       // 转换偏移
} ConvertMatrix;

typedef struct{
    float4 clipSpacePosition [[position]];  // position的修饰符表示这个是顶点
    float2 textureCoordinate;               // 纹理坐标，会做插值处理
} RasterizerData;

// 返回给片元着色器的结构体
vertex RasterizerData yuv_vertex(uint vertexID [[ vertex_id ]],                     // vertex_id是顶点shader每次处理的index，用于定位当前的顶点
                                 constant Vertex *vertexArray [[ buffer(0) ]])      // buffer表明是缓存数据，0是索引
{
    RasterizerData out;
    out.clipSpacePosition = vertexArray[vertexID].position;
    out.textureCoordinate = vertexArray[vertexID].coordinate;
    return out;
}

fragment float4 yuv_fragment(RasterizerData input [[stage_in]],                       // stage_in表示这个数据来自光栅化。（光栅化是顶点处理之后的步骤，业务层无法修改）
                             texture2d<float> textureY [[ texture(0) ]],              // texture表明是纹理数据，0是Y的索引
                             texture2d<float> textureUV [[ texture(1) ]],             // texture表明是纹理数据，1是UV索引
                             constant ConvertMatrix *convertMatrix [[ buffer(0) ]])   //buffer表明是缓存数据，LYFragmentInputIndexMatrix是索引
{
    constexpr sampler textureSampler (mag_filter::linear, min_filter::linear); // sampler是采样器
    
    float3 yuv = float3(textureY.sample(textureSampler, input.textureCoordinate).r,
                        textureUV.sample(textureSampler, input.textureCoordinate).rg);
    
    float3 rgb = convertMatrix->matrix * (yuv + convertMatrix->offset);
    return float4(rgb, 1.0);
}
