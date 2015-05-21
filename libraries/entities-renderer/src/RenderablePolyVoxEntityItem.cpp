//
//  RenderablePolyVoxEntityItem.cpp
//  libraries/entities-renderer/src/
//
//  Created by Seth Alves on 5/19/15.
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <glm/gtx/quaternion.hpp>

#include <gpu/GPUConfig.h>

#include <DeferredLightingEffect.h>
#include <Model.h>
#include <PerfStat.h>

#include <PolyVoxCore/CubicSurfaceExtractorWithNormals.h>
#include <PolyVoxCore/MarchingCubesSurfaceExtractor.h>
#include <PolyVoxCore/SurfaceMesh.h>
#include <PolyVoxCore/SimpleVolume.h>

#include "model/Geometry.h"
#include "gpu/GLBackend.h"
#include "EntityTreeRenderer.h"
#include "RenderablePolyVoxEntityItem.h"

EntityItem* RenderablePolyVoxEntityItem::factory(const EntityItemID& entityID, const EntityItemProperties& properties) {
    return new RenderablePolyVoxEntityItem(entityID, properties);
}


void createSphereInVolume(PolyVox::SimpleVolume<uint8_t>& volData, float fRadius) {
    // This vector hold the position of the center of the volume
    PolyVox::Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

    // This three-level for loop iterates over every voxel in the volume
    for (int z = 0; z < volData.getDepth(); z++) {
        for (int y = 0; y < volData.getHeight(); y++) {
            for (int x = 0; x < volData.getWidth(); x++) {
                // Store our current position as a vector...
                PolyVox::Vector3DFloat v3dCurrentPos(x,y,z);	
                // And compute how far the current position is from the center of the volume
                float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

                uint8_t uVoxelValue = 0;

                // If the current voxel is less than 'radius' units from the center then we make it solid.
                if(fDistToCenter <= fRadius) {
                    // Our new voxel value
                    uVoxelValue = 255;
                }

                // Wrte the voxel value into the volume	
                volData.setVoxelAt(x, y, z, uVoxelValue);
            }
        }
    }
}


void RenderablePolyVoxEntityItem::getModel() {
    PolyVox::SimpleVolume<uint8_t> volData(PolyVox::Region(PolyVox::Vector3DInt32(0,0,0),
                                                           PolyVox::Vector3DInt32(63, 63, 63)));
    createSphereInVolume(volData, 15);

    // A mesh object to hold the result of surface extraction
    PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> polyVoxMesh;

    //Create a surface extractor. Comment out one of the following two lines to decide which type gets created.
    PolyVox::CubicSurfaceExtractorWithNormals<PolyVox::SimpleVolume<uint8_t>> surfaceExtractor
        (&volData, volData.getEnclosingRegion(), &polyVoxMesh);
    // MarchingCubesSurfaceExtractor<SimpleVolume<uint8_t>> surfaceExtractor(&volData,
    //                                                                       volData.getEnclosingRegion(),
    //                                                                       &polyVoxMesh);

    //Execute the surface extractor.
    surfaceExtractor.execute();


    model::Mesh* mesh = new model::Mesh();
    model::MeshPointer meshPtr(mesh);


    const std::vector<uint32_t>& vecIndices = polyVoxMesh.getIndices();
    auto indexBuffer = new gpu::Buffer(vecIndices.size() * sizeof(uint32_t), (gpu::Byte*)vecIndices.data());
    auto indexBufferPtr = gpu::BufferPointer(indexBuffer);
    mesh->setIndexBuffer(gpu::BufferView(indexBufferPtr, gpu::Element(gpu::SCALAR, gpu::UINT32, gpu::RAW)));


    const std::vector<PolyVox::PositionMaterialNormal>& vecVertices = polyVoxMesh.getVertices();
    auto vertexBuffer = new gpu::Buffer(vecVertices.size() * sizeof(PolyVox::PositionMaterialNormal),
                                        (gpu::Byte*)vecVertices.data());
    auto vertexBufferPtr = gpu::BufferPointer(vertexBuffer);
    mesh->setVertexBuffer(gpu::BufferView(vertexBufferPtr,
                                          0,
                                          vertexBufferPtr->getSize() - sizeof(float) * 3,
                                          sizeof(float) * 7,
                                          gpu::Element(gpu::VEC3, gpu::FLOAT, gpu::RAW)));
    mesh->addAttribute(gpu::Stream::NORMAL,
                       gpu::BufferView(vertexBufferPtr,
                                       sizeof(float) * 3,
                                       vertexBufferPtr->getSize() - sizeof(float) * 3,
                                       sizeof(float) * 7,
                                       gpu::Element(gpu::VEC3, gpu::FLOAT, gpu::RAW)));


    qDebug() << "-------------XXXXXXXXXXXXXXXXXXXX-------------------";
    qDebug() << "---- vecIndices.size() =" << vecIndices.size();
    qDebug() << "---- vecVertices.size() =" << vecVertices.size();
    // [DEBUG] [05/19 20:46:38] ---- vecIndices.size() = 101556
    // [DEBUG] [05/19 20:46:38] ---- vecVertices.size() = 67704


    _modelGeometry.setMesh(meshPtr);
    _needsModelReload = false;
}



void RenderablePolyVoxEntityItem::render(RenderArgs* args) {
    PerformanceTimer perfTimer("RenderablePolyVoxEntityItem::render");
    assert(getType() == EntityTypes::PolyVox);

    if (_needsModelReload) {
        getModel();
    }

    glm::vec3 position = getPosition();
    // glm::vec3 dimensions = getDimensions();
    glm::quat rotation = getRotation();
    glPushMatrix();
        glTranslatef(position.x, position.y, position.z);
        glm::vec3 axis = glm::axis(rotation);
        glRotatef(glm::degrees(glm::angle(rotation)), axis.x, axis.y, axis.z);

        gpu::Batch batch;

        auto mesh = _modelGeometry.getMesh();
        batch.setInputFormat(mesh->getVertexFormat());
        batch.setIndexBuffer(gpu::UINT32, mesh->getIndexBuffer()._buffer, 0);
        batch.setInputStream(0, mesh->makeBufferStream());
        batch.drawIndexed(gpu::TRIANGLES, mesh->getNumIndices(), 0);
        gpu::GLBackend::renderBatch(batch);

    glPopMatrix();
    RenderableDebugableEntityItem::render(this, args);
}
