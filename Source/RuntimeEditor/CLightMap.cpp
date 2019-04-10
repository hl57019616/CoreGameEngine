#if 0
#include "CLightMap.h"
#include "IPhysicsStrategy.h"
#include "CPhysicsManager.h"
#include "PhysicsStructures.h"

#include "OgrePass.h"
#include "OgreMaterial.h"
#include "OgreTechnique.h"
#include "OgreSubMesh.h"
#include "OgreSubEntity.h"
#include "OgreStringConverter.h"
#include "OgreTextureManager.h"
#include "OgreMaterialManager.h"
#include "OgreHardwarePixelBuffer.h"

Vector<std::pair<int, int> > CLightMap::m_SearchPattern;
int CLightMap::m_iLightMapCounter = 0;
 
CLightMap::CLightMap(SubEntity* pSubEntity, Real PixelsPerUnit, int iTexSize, bool bDebugLightmaps)
: m_pSubEntity(pSubEntity)
, m_iTexSize(iTexSize)
, m_iCoordSet(0)
, m_PixelsPerUnit(PixelsPerUnit)
, m_bDebugLightmaps(bDebugLightmaps)
{
    if (m_SearchPattern.empty())
        BuildSearchPattern();
 
    m_LightMapName = "LightMap" + StringConverter::toString(m_iLightMapCounter++);
 
	m_LightMap = new cimg_library::CImg<unsigned char>();
    CalculateLightMap();
    AssignMaterial();
}
 
CLightMap::~CLightMap(void)
{
    // PROBLEM: THIS CAUSES CRASHES, FIND OUT WHY AND PUT IT BACK
/*    if (!m_Material.isNull())
    {
        MaterialManager::getSingleton().remove((ResourcePtr)m_Material);
        m_Material.setNull();
    }*/
    if (!m_Texture.isNull())
    {
        TextureManager::getSingleton().remove((ResourcePtr)m_Texture);
        m_Texture.setNull();
    }
}
 
void CLightMap::ResetCounter()
{
    m_iLightMapCounter = 0;
}
 
void CLightMap::loadResource(Resource *resource)
{
    Texture* texture = (Texture*)resource;
 
    // Get the pixel buffer
    HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
 
    // Lock the pixel buffer and get a pixel box
    pixelBuffer->lock(HardwareBuffer::HBL_DISCARD); // for best performance use HBL_DISCARD!
    const PixelBox &pixelBox = pixelBuffer->getCurrentLock();
 
    uint8* data = static_cast<uint8*>(pixelBox.data);
 
    assert(pixelBox.getWidth() == pixelBox.getHeight());
    const int iTexSize = (int)pixelBox.getWidth();
    const int iRowPitch = (int)pixelBox.rowPitch;
 
    int i, j;
 
    for (j = 0; j < iTexSize; j++)
    {
        for(i = 0; i < iTexSize; i++)
        {
            data[iRowPitch*j + i] = (*m_LightMap)(i, j);
        }
    }
 
    // Unlock the pixel buffer
    pixelBuffer->unlock();
}
 
String CLightMap::GetName()
{
    return m_LightMapName;
}
 
Vector3 CLightMap::GetBarycentricCoordinates(const Vector2 &P1, const Vector2 &P2, const Vector2 &P3, const Vector2 &P)
{
    Vector3 Coordinates(0.0);
    Real denom = (-P1.x * P3.y - P2.x * P1.y + P2.x * P3.y + P1.y * P3.x + P2.y * P1.x - P2.y * P3.x);
 
    if (fabs(denom) >= 1e-6)
    {
        Coordinates.x = (P2.x * P3.y - P2.y * P3.x - P.x * P3.y + P3.x * P.y - P2.x * P.y + P2.y * P.x) / denom;
        Coordinates.y = -(-P1.x * P.y + P1.x * P3.y + P1.y * P.x - P.x * P3.y + P3.x * P.y - P1.y * P3.x) / denom;
//        Coordinates.z = (-P1.x * P.y + P2.y * P1.x + P2.x * P.y - P2.x * P1.y - P2.y * P.x + P1.y * P.x) / denom;
    }
    Coordinates.z = 1 - Coordinates.x - Coordinates.y;
 
    return Coordinates;
}
 
Real CLightMap::GetTriangleArea(const Vector3 &P1, const Vector3 &P2, const Vector3 &P3)
{
    return 0.5*(P2-P1).crossProduct(P3-P1).length();
}
 
bool CLightMap::CalculateLightMap()
{
    // Reset the lightmap to all 0's
    if (m_LightMap)
        m_LightMap->fill(0);
 
    // Get the submesh
    SubMesh* submesh = m_pSubEntity->getSubMesh();
    Matrix4 WorldTransform;
    m_pSubEntity->getWorldTransforms(&WorldTransform);
 
    // Get vertex positions
    Vector<Vector3> MeshVertices;
    {
        VertexData* vertex_data = submesh->useSharedVertices ? submesh->parent->sharedVertexData : submesh->vertexData;
        const VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
        HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
        unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
 
        float* pReal;
 
        MeshVertices.resize(vertex_data->vertexCount);
 
        for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
        {
            posElem->baseVertexPointerToElement(vertex, &pReal);
            MeshVertices[j] = WorldTransform*Vector3(pReal[0],pReal[1],pReal[2]);
        }
 
        vbuf->unlock();
    }
 
    // Get vertex normals
    Quaternion Rotation = WorldTransform.extractQuaternion();
    Vector<Vector3> MeshNormals;
    {
        VertexData* vertex_data = submesh->useSharedVertices ? submesh->parent->sharedVertexData : submesh->vertexData;
        const VertexElement* normalElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_NORMAL);
        HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(normalElem->getSource());
        unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
 
        float* pReal;
 
        MeshNormals.resize(vertex_data->vertexCount);
 
        for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
        {
            normalElem->baseVertexPointerToElement(vertex, &pReal);
            MeshNormals[j] = Rotation*Vector3(pReal[0],pReal[1],pReal[2]);
        }
 
        vbuf->unlock();
    }
 
    // Get vertex UV coordinates
    Vector<Vector2> MeshTextureCoords;
    {
        VertexData* vertex_data = submesh->useSharedVertices ? submesh->parent->sharedVertexData : submesh->vertexData;
        // Get last set of texture coordinates
        int i = 0;
        const VertexElement* texcoordElem;
        const VertexElement* pCurrentElement = NULL;
        do
        {
            texcoordElem = pCurrentElement;
            pCurrentElement = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_TEXTURE_COORDINATES, i++);
        } while (pCurrentElement);
        m_iCoordSet = i-2;
        if (!texcoordElem)
            return false;
        HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(texcoordElem->getSource());
        unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
 
        float* pReal;
 
        MeshTextureCoords.resize(vertex_data->vertexCount);
 
        for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
        {
            texcoordElem->baseVertexPointerToElement(vertex, &pReal);
            MeshTextureCoords[j] = Vector2(pReal[0], pReal[1]);
        }
 
        vbuf->unlock();
    }
 
    IndexData* index_data = submesh->indexData;
 
    size_t numTris = index_data->indexCount / 3;
    HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
 
    bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
 
    unsigned int Indices[3];
    void* pBuffer = ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
 
    // Calculate the lightmap texture size
    if (m_PixelsPerUnit && m_Texture.isNull())
    {
        Real SurfaceArea = 0;
        for ( size_t k = 0; k < numTris*3; k+=3)
        {
            for (int i=0; i<3; ++i)
            {
                if (use32bitindexes)
                    Indices[i] = ((unsigned int*)pBuffer)[k+i];
                else
                    Indices[i] = ((unsigned short*)pBuffer)[k+i];
            }
            SurfaceArea += GetTriangleArea(MeshVertices[Indices[0]], MeshVertices[Indices[1]], MeshVertices[Indices[2]]);
        }
        Real TexSize = Math::Sqrt(SurfaceArea)*m_PixelsPerUnit;
 
        int iTexSize = 1;
        while (iTexSize < TexSize)
            iTexSize *= 2;
 
        m_iTexSize = iTexSize;
    }
 
    // Create the texture with the new size
    CreateTexture();
 
    // Fill in the lightmap
    for ( size_t k = 0; k < numTris*3; k+=3)
    {
        for (int i=0; i<3; ++i)
        {
            if (use32bitindexes)
                Indices[i] = ((unsigned int*)pBuffer)[k+i];
            else
                Indices[i] = ((unsigned short*)pBuffer)[k+i];
        }
        LightTriangle(MeshVertices[Indices[0]], MeshVertices[Indices[1]], MeshVertices[Indices[2]],
                        MeshNormals[Indices[0]], MeshNormals[Indices[1]], MeshNormals[Indices[2]],
                        MeshTextureCoords[Indices[0]], MeshTextureCoords[Indices[1]], MeshTextureCoords[Indices[2]]);
    }
 
    ibuf->unlock();
 
    FillInvalidPixels();
    m_LightMap->blur(1.0);
 
    return true;
}
 
void CLightMap::CreateTexture()
{
    if (!m_Texture.isNull())
        return;

	CORE_DELETE(m_LightMap);
    m_LightMap = new cimg_library::CImg<unsigned char>(m_iTexSize, m_iTexSize, 1, 2, 0);

    if (TextureManager::getSingleton().resourceExists(m_LightMapName))
        TextureManager::getSingleton().remove(m_LightMapName);
    // Create the texture
    m_Texture = TextureManager::getSingleton().createManual(
            m_LightMapName, // name
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            TEX_TYPE_2D,      // type
            m_iTexSize, m_iTexSize,         // width & height
            -1,                // number of mipmaps
            PF_L8,     // pixel format
            TU_DEFAULT,
            this);
}

void CLightMap::AssignMaterial()
{
    if (!m_Material.isNull())
        return;
    
	if (MaterialManager::getSingleton().resourceExists(m_LightMapName))
        MaterialManager::getSingleton().remove(m_LightMapName);
    
	if (m_bDebugLightmaps)
    {
        m_Material = MaterialManager::getSingleton().create(m_LightMapName, StringUtil::BLANK, true);
    }
    else
    {
        MaterialPtr PrevMaterial = m_pSubEntity->getMaterial();
        m_Material = PrevMaterial->clone(m_LightMapName);
    }

    Pass* pPass = m_Material->getTechnique(0)->getPass(0);
    pPass->setLightingEnabled(false);
    TextureUnitState* pTextureUnitState = pPass->createTextureUnitState(m_Texture->getName(), m_iCoordSet);
    pTextureUnitState->setColourOperation(LBO_MODULATE);
    pTextureUnitState->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
    m_pSubEntity->setMaterialName(m_LightMapName);
}

void CLightMap::FillInvalidPixels()
{
    int i, j;
    int x, y;
    Vector<std::pair<int, int>>::iterator itSearchPattern;
    for (i=0; i<m_iTexSize; ++i)
    {
        for (j=0; j<m_iTexSize; ++j)
        {
            // Invalid pixel found
            if ((*m_LightMap)(i, j, 0, 1) == 0)
            {
                for (itSearchPattern = m_SearchPattern.begin(); itSearchPattern != m_SearchPattern.end(); ++itSearchPattern)
                {
                    x = i+itSearchPattern->first;
                    y = j+itSearchPattern->second;
                    if (x < 0 || x >= m_iTexSize)
                        continue;
                    if (y < 0 || y >= m_iTexSize)
                        continue;
                    // If search pixel is valid assign it to the invalid pixel and stop searching
                    if ((*m_LightMap)(x, y, 0, 1) == 1)
                    {
                        (*m_LightMap)(i, j) = (*m_LightMap)(x, y);
                        break;
                    }
                }
            }
        }
    }
}
 
void CLightMap::BuildSearchPattern()
{
    m_SearchPattern.clear();
    const int iSize = 5;
    int i, j;
    for (i=-iSize; i<=iSize; ++i)
    {
        for (j=-iSize; j<=iSize; ++j)
        {
            if (i==0 && j==0)
                continue;
            m_SearchPattern.push_back(std::make_pair(i, j));
        }
    }
    sort(m_SearchPattern.begin(), m_SearchPattern.end(), SortCoordsByDistance());
}
 
void CLightMap::LightTriangle(const Vector3 &P1, const Vector3 &P2, const Vector3 &P3,
                              const Vector3 &N1, const Vector3 &N2, const Vector3 &N3,
                              const Vector2 &T1, const Vector2 &T2, const Vector2 &T3)
{
    Vector2 TMin = T1, TMax = T1;
    TMin.makeFloor(T2);
    TMin.makeFloor(T3);
    TMax.makeCeil(T2);
    TMax.makeCeil(T3);
    int iMinX = GetPixelCoordinate(TMin.x);
    int iMinY = GetPixelCoordinate(TMin.y);
    int iMaxX = GetPixelCoordinate(TMax.x);
    int iMaxY = GetPixelCoordinate(TMax.y);
    int i, j;
    Vector2 TextureCoord;
    Vector3 BarycentricCoords;
    Vector3 Pos;
    Vector3 Normal;
    for (i=iMinX; i<=iMaxX; ++i)
    {
        for (j=iMinY; j<=iMaxY; ++j)
        {
            TextureCoord.x = GetTextureCoordinate(i);
            TextureCoord.y = GetTextureCoordinate(j);
            BarycentricCoords = GetBarycentricCoordinates(T1, T2, T3, TextureCoord);
            Pos = BarycentricCoords.x * P1 + BarycentricCoords.y * P2 + BarycentricCoords.z * P3;
            Normal = BarycentricCoords.x * N1 + BarycentricCoords.y * N2 + BarycentricCoords.z * N3;
            Normal.normalise();
            if ((*m_LightMap)(i, j, 0, 1) == 1 || BarycentricCoords.x < 0 || BarycentricCoords.y < 0 || BarycentricCoords.z < 0)
                continue;
            (*m_LightMap)(i, j) = GetLightIntensity(Pos, Normal);
            (*m_LightMap)(i, j, 0, 1) = 1;
        }
    }
 
}
 
/*
This is the only function which you should need to modify. Basically given the position coordinate
and the surface normal at that point, you should return the light intensity value as a number between
0 and 255. In this example I use the PhysX library to cast a ray in a fixed direction to see if it
intersects with any other objects in the scene, if it does then this point is in the shade.
*/
uint8 CLightMap::GetLightIntensity(const Vector3 &Position, const Vector3 &Normal)
{
    const Real Tolerance = 1e-3;
    const Real Distance = 100;
    const uint8 AmbientValue = 1;
    const uint8 MaxValue = 255;
 
    Vector3 LightDirection(0, 100, 0);
    LightDirection.normalise();
 
    Real Intensity = -LightDirection.dotProduct(Normal);
    if (Intensity < 0)
        return AmbientValue;
 
    uint8 LightValue = AmbientValue+Intensity*(MaxValue-AmbientValue);
 
    Vector3 Origin = Position-Distance*LightDirection;

	auto strat = Core::Physics::CPhysicsManager::Instance()->GetStrategy();
	Core::Physics::SPhysicsRayCastReport report;
	strat->RaycastClosestShape(Origin, LightDirection, report);
	if(report.Data)
	{
		return AmbientValue;
	}
	else
	{
		strat->RaycastClosestShape(Position, -LightDirection, report);
		if(report.Data)
		{
			return AmbientValue;
		}
		return LightValue;
	}

    //NxRay Ray;
    //NxRaycastHit RayHit;
    //Ray.orig = Convert(Origin);
    //Ray.dir = Convert(LightDirection);
    //bool bHit = PHYSICS->GetNxScene()->raycastAnyShape(Ray, NX_ALL_SHAPES, (1<<GROUP_WORLD), Distance-Tolerance);
    //if (bHit)
    //{
    //    return AmbientValue;
    //}
    //else
    //{
    //    // Cast the ray back the other way also to be sure that we can see the light
    //    Ray.orig = Convert(Position);
    //    Ray.dir = Convert(-LightDirection);
    //    bHit = PHYSICS->GetNxScene()->raycastAnyShape(Ray, NX_ALL_SHAPES, (1<<GROUP_WORLD));
    //    if (bHit)
    //        return AmbientValue;
    //    else
    //        return LightValue;
    //}
}

#endif