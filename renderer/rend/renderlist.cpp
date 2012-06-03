/*
 * renderlist.cpp
 *
 *  Created on: Mar 10, 2012
 *      Author: flamingo
 */

#include "renderlist.h"
#include "camera.h"
#include "vertex.h"

namespace rend
{

void RenderList::createTriangles(const VertexBuffer &vertexBuffer, list<math::Triangle> &output)
{
    math::Triangle triangle;
    // all mesh vertices
    auto vertices = vertexBuffer.getVertices();
    // mesh indices
    auto indices = vertexBuffer.getIndices();
    // FIXME: not here!
    // mesh world transformation
//    const math::M44 &tr = mesh.getTransformation();
    math::M44 tr;

    switch(vertexBuffer.getType())
    {
    case VertexBuffer::INDEXEDTRIANGLELIST:

        for(size_t ind = 0, t = 0; ind < indices.size(); ind += 3, t++)
        {
            if ((ind + 2) >= indices.size())
                break;

            // form the triangle
            triangle.v(0) = vertices[indices[ind]];
            triangle.v(1) = vertices[indices[ind + 1]];
            triangle.v(2) = vertices[indices[ind + 2]];

            // translate and rotate the triangle
            triangle.v(0).p = triangle.v(0).p * tr;
            triangle.v(1).p = triangle.v(1).p * tr;
            triangle.v(2).p = triangle.v(2).p * tr;

            triangle.setMaterial(vertexBuffer.getMaterial());

            // set material
//            if (!vertexBuffer.getMaterial()->
//                triangle.material() = mesh.materials()[t];
//            else

            // compute normals
            triangle.computeNormal();

            // save it
            output.push_back(triangle);
        }
        break;

    case VertexBuffer::TRIANGLELIST:

        for(size_t v = 0; v < vertices.size(); v += 3)
        {
            if ((v + 2) >= vertices.size())
                break;

            triangle.v(0) = vertices[v];
            triangle.v(1) = vertices[v + 1];
            triangle.v(2) = vertices[v + 2];

            //triangle.material() = Material();//Material(Color3(255, 0, 0), Material::SM_FLAT);

//            triangle.setWindingOrder(mesh.getWindingOrder());
            triangle.computeNormal();

            output.push_back(triangle);
        }
        break;

    case VertexBuffer::UNDEFINED:
    default:
        *syslog << "Can't draw this mesh" << logerr;
        break;
    }
}

void RenderList::append(const Mesh &mesh)
{
    const list<VertexBuffer> &subMeshes = mesh.getSubmeshes();

    foreach (const VertexBuffer &vb, subMeshes)
        RenderList::createTriangles(vb, m_triangles);
}

void RenderList::zsort()
{
    m_triangles.sort(math::ZCompareAvg);
}

void RenderList::removeBackfaces(const sptr(Camera) cam)
{
    auto t = m_triangles.begin();

    while (t != m_triangles.end())
    {
        if (t->normal().isZero())
        {
            t++;
            continue;
        }

        if (t->getSideType() == math::Triangle::TWO_SIDE)
        {
            t++;
            continue;
        }

        math::vec3 view = cam->getPosition() - t->v(0).p;
        view.normalize();
        double dp = t->normal().dotProduct(view);

        if (dp <= 0)
        {
            t = m_triangles.erase(t);
            continue;
        }

        t++;
    }
}

}
