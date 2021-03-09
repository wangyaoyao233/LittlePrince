#pragma once

#include "ThirdParty/assimp/include/assimp/cimport.h"
#include "ThirdParty/assimp/include/assimp/scene.h"
#include "ThirdParty/assimp/include/assimp/postprocess.h"
#include "ThirdParty/assimp/include/assimp/matrix4x4.h"

struct DEFORM_VERTEX
{
	aiVector3D Position;
	aiVector3D Normal;

	int BoneNum;//骨骼数量
	std::string BoneName[4];//对应骨骼名称
	float BoneWeight[4];//对应骨骼权重
};

struct BONE
{
	aiMatrix4x4 Matrix;
	aiMatrix4x4 AnimationMatrix;
	aiMatrix4x4 OffsetMatrix;
};


class AnimationModel
{
public:
	void Load(const char* filename);
	void UnLoad();
	void Update(const std::string animationName, int Frame);
	void Draw();

	void LoadAnimation(const std::string filename, const std::string animationName);

	void SetInitAnimation(std::string name) { m_CurAnimationName = name; }

	void Update(const std::string animationName, const std::string animationName2, float blendRate, int Frame);

private:
	const aiScene* m_Scene = nullptr;
	ID3D11Buffer** m_VertexBuffer;
	ID3D11Buffer** m_IndexBuffer;

	std::map<std::string, ID3D11ShaderResourceView*> m_Texture;

	void CreateBone(aiNode* node);
	void UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix);

	std::vector<DEFORM_VERTEX>* m_DeformVertex;
	std::map<const std::string, BONE> m_Bone;

	std::map<const std::string,const aiScene*> m_Animation;


	std::string m_CurAnimationName;
	//控制线性插值动画的比率
	float m_t;

	void Calculate(unsigned int m, unsigned int v, aiMesh* mesh, VERTEX_3D* vertex);


	// GPU Skinning
	void CSInit();
	void CSCompute();
	ComPtr<ID3D11ComputeShader> m_CS;
	ComPtr<ID3D11Buffer> m_ConstantBuffer; // 常量缓冲区

};