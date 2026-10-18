#include "pch.h"
#include "Importer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace bdt
{
    Importer::Importer() = default;
    Importer::~Importer() = default;

    bool Importer::LoadScene(const String& path, uint32 flags)
    {
        if (path.empty() || m_importer) return false;

        auto importer = std::make_unique<Assimp::Importer>();
        const aiScene* scene = importer->ReadFile(path.c_str(), flags);
        if (!scene || !scene->mRootNode)  return false;

        m_importer = std::move(importer);
        m_path = path;

        return true;
    }

    const aiScene* Importer::GetScene() const
    {
        if (!m_importer) return nullptr;

        return m_importer->GetScene();
    }

    const String& Importer::GetPath() const
    {
        return m_path;
    }

    mat4 Importer::ToMatrix(const aiMatrix4x4& m)
    {
        return mat4
        (
            m.a1, m.b1, m.c1, m.d1,
            m.a2, m.b2, m.c2, m.d2,
            m.a3, m.b3, m.c3, m.d3,
            m.a4, m.b4, m.c4, m.d4
        );
    }

    vec3 Importer::ToVector(const aiVector3D& v)
    {
        return vec3(v.x, v.y, v.z);
    }

    glm::quat Importer::ToQuaternion(const aiQuaternion& q)
    {
        return glm::quat(q.w, q.x, q.y, q.z);
    }
}