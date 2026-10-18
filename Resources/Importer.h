#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include <assimp/quaternion.h>

struct aiScene;
namespace Assimp { class Importer; }

namespace bdt
{
    class Importer
    {
        BDT_DISABLE_COPY(Importer)
        BDT_DISABLE_MOVE(Importer)

    public:
        virtual ~Importer();

    protected:
        Importer();

        bool LoadScene(const String& path, uint32 flags);

        const aiScene* GetScene() const;
        const String& GetPath() const;

        static mat4 ToMatrix(const aiMatrix4x4& matrix);
        static vec3 ToVector(const aiVector3D& vector);
        static glm::quat ToQuaternion(const aiQuaternion& quaternion);

    private:
        std::unique_ptr<Assimp::Importer> m_importer{ nullptr };
        String m_path{};
	};
}

