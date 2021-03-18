//
//  Metal.metal
//  XXapp
//
//  Created by VINSON on 2019/5/13.
//  Copyright © 2019 VINSON. All rights reserved.
//

#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;

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
vertex RasterizerData common_vertex(uint vertexID [[ vertex_id ]],                     // vertex_id是顶点shader每次处理的index，用于定位当前的顶点
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
                             constant ConvertMatrix *convertMatrix [[ buffer(0) ]])   // buffer表明是缓存数据，LYFragmentInputIndexMatrix是索引
{
    constexpr sampler textureSampler (mag_filter::linear, min_filter::linear); // sampler是采样器
    
    float3 yuv = float3(textureY.sample(textureSampler, input.textureCoordinate).r,
                        textureUV.sample(textureSampler, input.textureCoordinate).rg);
    
    float3 rgb = convertMatrix->matrix * (yuv + convertMatrix->offset);
    return float4(rgb, 1.0);
}


fragment float4 rgb_fragment(RasterizerData input [[stage_in]], // stage_in表示这个数据来自光栅化。（光栅化是顶点处理之后的步骤，业务层无法修改）
               texture2d<half> colorTexture [[ texture(0) ]]) // texture表明是纹理数据，0是索引
{
    constexpr sampler textureSampler (mag_filter::linear,
                                      min_filter::linear); // sampler是采样器
    
    half4 colorSample = colorTexture.sample(textureSampler, input.textureCoordinate); // 得到纹理对应位置的颜色
    return float4(colorSample);
}
