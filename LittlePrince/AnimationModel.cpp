/*system header*/
#include "Main.h"
#include "Renderer.h"
/*tools header*/
#include "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include/D3DX11.h"
/*scenes header*/
/*gameobjects header*/
/*self header*/
#include "AnimationModel.h"

#pragma comment (lib, "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64/d3dx11.lib")
#pragma comment(lib, "ThirdParty/assimp/lib/Debug/assimp-vc140-mt.lib")


void AnimationModel::Load(const char* filename)
{
	std::string modelPath(filename);

	m_Scene = aiImportFile(filename, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(m_Scene);

	m_VertexBuffer = new ID3D11Buffer * [m_Scene->mNumMeshes];
	m_IndexBuffer = new ID3D11Buffer * [m_Scene->mNumMeshes];

	m_DeformVertex = new std::vector<DEFORM_VERTEX>[m_Scene->mNumMeshes];

	CreateBone(m_Scene->mRootNode);

	for (unsigned int m = 0; m < m_Scene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_Scene->mMeshes[m];

		//VertexBuffer
		{
			VERTEX_3D* vertex = new VERTEX_3D[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				vertex[v].Position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
				vertex[v].Normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;

			Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer[m]);

			delete[] vertex;
		}

		//IndexBuffer
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace* face = &mesh->mFaces[f];
				assert(face->mNumIndices == 3);
				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;

			Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer[m]);

			delete[] index;
		}

		//DeformVertex
		{
			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				DEFORM_VERTEX deformVertex;
				deformVertex.Position = mesh->mVertices[v]; 
				deformVertex.Normal = mesh->mNormals[v];
				deformVertex.BoneNum = 0;

				for (unsigned int b = 0; b < 4; b++)
				{
					deformVertex.BoneName[b] = "";
					deformVertex.BoneWeight[b] = 0.0f;
				}

				m_DeformVertex[m].push_back(deformVertex);
			}
		}

		//Bone
		{
			for (unsigned int b = 0; b < mesh->mNumBones; b++)
			{
				aiBone* bone = mesh->mBones[b];

				m_Bone[bone->mName.C_Str()].OffsetMatrix = bone->mOffsetMatrix;

				for (unsigned int w = 0; w < bone->mNumWeights; w++)
				{
					aiVertexWeight weight = bone->mWeights[w];

					int num = m_DeformVertex[m][weight.mVertexId].BoneNum;

					m_DeformVertex[m][weight.mVertexId].BoneWeight[num] = weight.mWeight;
					m_DeformVertex[m][weight.mVertexId].BoneName[num] = bone->mName.C_Str();
					m_DeformVertex[m][weight.mVertexId].BoneNum++;

					assert(m_DeformVertex[m][weight.mVertexId].BoneNum <= 4);
				}
			}
		}
	}

	//Texture
	for (unsigned int m = 0; m < m_Scene->mNumMaterials; m++)
	{
		aiString path;
		if (m_Scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			if (path.data[0] == '*')
			{
				if (m_Texture[path.data] == nullptr)
				{
					ID3D11ShaderResourceView* texture;
					int id = atoi(&path.data[1]);

					D3DX11CreateShaderResourceViewFromMemory(Renderer::GetDevice().Get(), (const unsigned char*)m_Scene->mTextures[id]->pcData, m_Scene->mTextures[id]->mWidth, nullptr, nullptr, &texture, nullptr);
					assert(texture);

					m_Texture[path.data] = texture;
				}
			}
			else
			{
				//from file
				//1.解析texture路径
				//1.1 分离出model所在路径
				std::replace(modelPath.begin(), modelPath.end(), '\\', '/');
				size_t p1 = modelPath.rfind('/');
				std::string textureName = modelPath.substr(0, p1 + 1);
				//1.2 拼接得到texture所在路径
				std::string stringPath = path.C_Str();
				std::replace(stringPath.begin(), stringPath.end(), '\\', '/');
				size_t p2 = stringPath.rfind('/');
				textureName += stringPath.substr(p2 + 1, stringPath.size() - p2);

				//2.读取
				ID3D11ShaderResourceView* texture;

				D3DX11CreateShaderResourceViewFromFileA(Renderer::GetDevice().Get(),textureName.c_str(),NULL,NULL,&texture,NULL);
				//CreateWICTextureFromFile(CRenderer::GetDevice().Get(), L"../Asset/Texture/TreeColors_A.dds", nullptr, &texture);
				assert(texture);

				//3.放入map
				m_Texture[path.data] = texture;
			}
		}
		else
		{
			m_Texture[path.data] = nullptr;
		}
	}

	//控制线性插值动画的比率初始为1.0f
	m_t = 1.0f;
}

void AnimationModel::UnLoad()
{
	for (unsigned int m = 0; m < m_Scene->mNumMeshes; m++)
	{
		m_VertexBuffer[m]->Release();
		m_IndexBuffer[m]->Release();
	}

	delete[] m_VertexBuffer;
	delete[] m_IndexBuffer;

	delete[] m_DeformVertex;

	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : m_Texture)
	{
		pair.second->Release();
	}

	for (auto pair : m_Animation)
	{
		aiReleaseImport(pair.second);
	}

	aiReleaseImport(m_Scene);
}

void AnimationModel::Calculate(unsigned int m,unsigned int v, aiMesh* mesh, VERTEX_3D* vertex)
{
	DEFORM_VERTEX* deformVertex = &m_DeformVertex[m][v];

	aiMatrix4x4 matrix[4];
	aiMatrix4x4 outMatrix;
	matrix[0] = m_Bone[deformVertex->BoneName[0]].Matrix;
	matrix[1] = m_Bone[deformVertex->BoneName[1]].Matrix;
	matrix[2] = m_Bone[deformVertex->BoneName[2]].Matrix;
	matrix[3] = m_Bone[deformVertex->BoneName[3]].Matrix;

	{
		//a
		outMatrix.a1 = matrix[0].a1 * deformVertex->BoneWeight[0]
			+ matrix[1].a1 * deformVertex->BoneWeight[1]
			+ matrix[2].a1 * deformVertex->BoneWeight[2]
			+ matrix[3].a1 * deformVertex->BoneWeight[3];

		outMatrix.a2 = matrix[0].a2 * deformVertex->BoneWeight[0]
			+ matrix[1].a2 * deformVertex->BoneWeight[1]
			+ matrix[2].a2 * deformVertex->BoneWeight[2]
			+ matrix[3].a2 * deformVertex->BoneWeight[3];

		outMatrix.a3 = matrix[0].a3 * deformVertex->BoneWeight[0]
			+ matrix[1].a3 * deformVertex->BoneWeight[1]
			+ matrix[2].a3 * deformVertex->BoneWeight[2]
			+ matrix[3].a3 * deformVertex->BoneWeight[3];

		outMatrix.a4 = matrix[0].a4 * deformVertex->BoneWeight[0]
			+ matrix[1].a4 * deformVertex->BoneWeight[1]
			+ matrix[2].a4 * deformVertex->BoneWeight[2]
			+ matrix[3].a4 * deformVertex->BoneWeight[3];

		//b
		outMatrix.b1 = matrix[0].b1 * deformVertex->BoneWeight[0]
			+ matrix[1].b1 * deformVertex->BoneWeight[1]
			+ matrix[2].b1 * deformVertex->BoneWeight[2]
			+ matrix[3].b1 * deformVertex->BoneWeight[3];

		outMatrix.b2 = matrix[0].b2 * deformVertex->BoneWeight[0]
			+ matrix[1].b2 * deformVertex->BoneWeight[1]
			+ matrix[2].b2 * deformVertex->BoneWeight[2]
			+ matrix[3].b2 * deformVertex->BoneWeight[3];

		outMatrix.b3 = matrix[0].b3 * deformVertex->BoneWeight[0]
			+ matrix[1].b3 * deformVertex->BoneWeight[1]
			+ matrix[2].b3 * deformVertex->BoneWeight[2]
			+ matrix[3].b3 * deformVertex->BoneWeight[3];

		outMatrix.b4 = matrix[0].b4 * deformVertex->BoneWeight[0]
			+ matrix[1].b4 * deformVertex->BoneWeight[1]
			+ matrix[2].b4 * deformVertex->BoneWeight[2]
			+ matrix[3].b4 * deformVertex->BoneWeight[3];

		//c
		outMatrix.c1 = matrix[0].c1 * deformVertex->BoneWeight[0]
			+ matrix[1].c1 * deformVertex->BoneWeight[1]
			+ matrix[2].c1 * deformVertex->BoneWeight[2]
			+ matrix[3].c1 * deformVertex->BoneWeight[3];

		outMatrix.c2 = matrix[0].c2 * deformVertex->BoneWeight[0]
			+ matrix[1].c2 * deformVertex->BoneWeight[1]
			+ matrix[2].c2 * deformVertex->BoneWeight[2]
			+ matrix[3].c2 * deformVertex->BoneWeight[3];

		outMatrix.c3 = matrix[0].c3 * deformVertex->BoneWeight[0]
			+ matrix[1].c3 * deformVertex->BoneWeight[1]
			+ matrix[2].c3 * deformVertex->BoneWeight[2]
			+ matrix[3].c3 * deformVertex->BoneWeight[3];

		outMatrix.c4 = matrix[0].c4 * deformVertex->BoneWeight[0]
			+ matrix[1].c4 * deformVertex->BoneWeight[1]
			+ matrix[2].c4 * deformVertex->BoneWeight[2]
			+ matrix[3].c4 * deformVertex->BoneWeight[3];

		//d
		outMatrix.d1 = matrix[0].d1 * deformVertex->BoneWeight[0]
			+ matrix[1].d1 * deformVertex->BoneWeight[1]
			+ matrix[2].d1 * deformVertex->BoneWeight[2]
			+ matrix[3].d1 * deformVertex->BoneWeight[3];

		outMatrix.d2 = matrix[0].d2 * deformVertex->BoneWeight[0]
			+ matrix[1].d2 * deformVertex->BoneWeight[1]
			+ matrix[2].d2 * deformVertex->BoneWeight[2]
			+ matrix[3].d2 * deformVertex->BoneWeight[3];

		outMatrix.d3 = matrix[0].d3 * deformVertex->BoneWeight[0]
			+ matrix[1].d3 * deformVertex->BoneWeight[1]
			+ matrix[2].d3 * deformVertex->BoneWeight[2]
			+ matrix[3].d3 * deformVertex->BoneWeight[3];

		outMatrix.d4 = matrix[0].d4 * deformVertex->BoneWeight[0]
			+ matrix[1].d4 * deformVertex->BoneWeight[1]
			+ matrix[2].d4 * deformVertex->BoneWeight[2]
			+ matrix[3].d4 * deformVertex->BoneWeight[3];
	}

	deformVertex->Position = mesh->mVertices[v];
	deformVertex->Position *= outMatrix;

	//法線変換用に移動成分を削除
	outMatrix.a4 = 0.0f;
	outMatrix.b4 = 0.0f;
	outMatrix.c4 = 0.0f;

	deformVertex->Normal = mesh->mNormals[v];
	deformVertex->Normal *= outMatrix;

	vertex[v].Position.x = deformVertex->Position.x;
	vertex[v].Position.y = deformVertex->Position.y;
	vertex[v].Position.z = deformVertex->Position.z;

	vertex[v].Normal.x = deformVertex->Normal.x;
	vertex[v].Normal.y = deformVertex->Normal.y;
	vertex[v].Normal.z = deformVertex->Normal.z;

	vertex[v].TexCoord.x = mesh->mTextureCoords[0][v].x;
	vertex[v].TexCoord.y = mesh->mTextureCoords[0][v].y;

	vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

}

void AnimationModel::CSInit()
{
}

void AnimationModel::CSCompute()
{
}


void AnimationModel::Update(const std::string animationName, int Frame)
{
	if (m_CurAnimationName != animationName)
	{
		m_t -= 0.1f;
		if (m_t <= 0)
		{
			m_t = 1.0f;
			m_CurAnimationName = animationName;
		}
	}
	
	//const aiScene* scene = m_Animation[animationName];
	const aiScene* scene = m_Animation[m_CurAnimationName];
	if (!scene->HasAnimations())
	{
		return;
	}
	//calculate bone matrix from animation data
	aiAnimation* animation = scene->mAnimations[0];	

	//next scene
	const aiScene* scene2 = m_Animation[animationName];
	aiAnimation* animation2 = scene2->mAnimations[0];


	for (unsigned int c = 0; c < animation->mNumChannels; c++)
	{
		aiNodeAnim* nodeAnim = animation->mChannels[c];
		BONE* bone = &m_Bone[nodeAnim->mNodeName.C_Str()];

		int f;
		f = Frame % nodeAnim->mNumRotationKeys;
		aiQuaternion rot = nodeAnim->mRotationKeys[f].mValue;

		f = Frame % nodeAnim->mNumPositionKeys;
		aiVector3D pos = nodeAnim->mPositionKeys[f].mValue;
		
		aiVector3D totalPos;
		aiQuaternion totalRot;

		if (m_t < 1.0f)
		{
			//animation2 
			aiNodeAnim* nodeAnim2 = animation2->mChannels[c];
			f = Frame % nodeAnim2->mNumPositionKeys;
			aiVector3D pos2 = nodeAnim2->mPositionKeys[f].mValue;
			f = Frame % nodeAnim2->mNumRotationKeys;
			aiQuaternion rot2 = nodeAnim2->mRotationKeys[f].mValue;

			//pos
			totalPos.x = m_t* pos.x + (1 - m_t) * pos2.x;
			totalPos.y = m_t* pos.y + (1 - m_t) * pos2.y;
			totalPos.z = m_t* pos.z + (1 - m_t) * pos2.z;
			//rot
			aiQuaternion::Interpolate(totalRot, rot, rot2, 1- m_t);
		}
		else
		{
			totalPos = pos;
			totalRot = rot;
		}

		//bone->AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot, pos);
		bone->AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), totalRot, totalPos);
	}

	//update bone matrix
	UpdateBoneMatrix(m_Scene->mRootNode, aiMatrix4x4());

	int cnt = 0;
	//CPU skinning
	for (unsigned int m = 0; m < m_Scene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_Scene->mMeshes[m];

		D3D11_MAPPED_SUBRESOURCE ms;
		Renderer::GetDeviceContext()->Map(m_VertexBuffer[m], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		VERTEX_3D* vertex = (VERTEX_3D*)ms.pData;

		//unsigned int thread_num = 8;
		//unsigned int block_size = mesh->mNumVertices / thread_num;
		//std::vector<std::thread> threads;
		////开始并行计算
		//for (unsigned int x = 0; x < thread_num - 1; ++x)
		//{
		//	// i * block_size + n;
		//	
		//	threads.emplace_back(std::thread([&]() {
		//		for (unsigned int n = 0; n < block_size; ++n)
		//		{
		//			cnt++;
		//			Calculate(m, x * block_size + n, mesh, vertex);
		//		}
		//		}));
		//}
		////最后一个线程处理的数据量
		//unsigned int last_size = mesh->mNumVertices - block_size * (thread_num - 1);
		//{
		//	threads.emplace_back(std::thread([&]() {
		//		for (unsigned int n = 0; n < last_size; ++n)
		//		{
		//			cnt++;
		//			Calculate(m, (thread_num - 1) * block_size + n, mesh, vertex);
		//		}
		//		}));
		//}
		//for (auto& thread : threads)
		//{
		//	thread.join();
		//}


		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			DEFORM_VERTEX* deformVertex = &m_DeformVertex[m][v];

			aiMatrix4x4 matrix[4];
			aiMatrix4x4 outMatrix;
			matrix[0] = m_Bone[deformVertex->BoneName[0]].Matrix;
			matrix[1] = m_Bone[deformVertex->BoneName[1]].Matrix;
			matrix[2] = m_Bone[deformVertex->BoneName[2]].Matrix;
			matrix[3] = m_Bone[deformVertex->BoneName[3]].Matrix;

			{
				//a
				outMatrix.a1 = matrix[0].a1 * deformVertex->BoneWeight[0]
							+ matrix[1].a1 * deformVertex->BoneWeight[1]
							+ matrix[2].a1 * deformVertex->BoneWeight[2]
							+ matrix[3].a1 * deformVertex->BoneWeight[3];

				outMatrix.a2 = matrix[0].a2 * deformVertex->BoneWeight[0]
							+ matrix[1].a2 * deformVertex->BoneWeight[1]
							+ matrix[2].a2 * deformVertex->BoneWeight[2]
							+ matrix[3].a2 * deformVertex->BoneWeight[3];

				outMatrix.a3 = matrix[0].a3 * deformVertex->BoneWeight[0]
							+ matrix[1].a3 * deformVertex->BoneWeight[1]
							+ matrix[2].a3 * deformVertex->BoneWeight[2]
							+ matrix[3].a3 * deformVertex->BoneWeight[3];

				outMatrix.a4 = matrix[0].a4 * deformVertex->BoneWeight[0]
							+ matrix[1].a4 * deformVertex->BoneWeight[1]
							+ matrix[2].a4 * deformVertex->BoneWeight[2]
							+ matrix[3].a4 * deformVertex->BoneWeight[3];

				//b
				outMatrix.b1 = matrix[0].b1 * deformVertex->BoneWeight[0]
							+ matrix[1].b1 * deformVertex->BoneWeight[1]
							+ matrix[2].b1 * deformVertex->BoneWeight[2]
							+ matrix[3].b1 * deformVertex->BoneWeight[3];

				outMatrix.b2 = matrix[0].b2 * deformVertex->BoneWeight[0]
							+ matrix[1].b2 * deformVertex->BoneWeight[1]
							+ matrix[2].b2 * deformVertex->BoneWeight[2]
							+ matrix[3].b2 * deformVertex->BoneWeight[3];

				outMatrix.b3 = matrix[0].b3 * deformVertex->BoneWeight[0]
							+ matrix[1].b3 * deformVertex->BoneWeight[1]
							+ matrix[2].b3 * deformVertex->BoneWeight[2]
							+ matrix[3].b3 * deformVertex->BoneWeight[3];

				outMatrix.b4 = matrix[0].b4 * deformVertex->BoneWeight[0]
							+ matrix[1].b4 * deformVertex->BoneWeight[1]
							+ matrix[2].b4 * deformVertex->BoneWeight[2]
							+ matrix[3].b4 * deformVertex->BoneWeight[3];

				//c
				outMatrix.c1 = matrix[0].c1 * deformVertex->BoneWeight[0]
							+ matrix[1].c1 * deformVertex->BoneWeight[1]
							+ matrix[2].c1 * deformVertex->BoneWeight[2]
							+ matrix[3].c1 * deformVertex->BoneWeight[3];

				outMatrix.c2 = matrix[0].c2 * deformVertex->BoneWeight[0]
							+ matrix[1].c2 * deformVertex->BoneWeight[1]
							+ matrix[2].c2 * deformVertex->BoneWeight[2]
							+ matrix[3].c2 * deformVertex->BoneWeight[3];

				outMatrix.c3 = matrix[0].c3 * deformVertex->BoneWeight[0]
							+ matrix[1].c3 * deformVertex->BoneWeight[1]
							+ matrix[2].c3 * deformVertex->BoneWeight[2]
							+ matrix[3].c3 * deformVertex->BoneWeight[3];

				outMatrix.c4 = matrix[0].c4 * deformVertex->BoneWeight[0]
							+ matrix[1].c4 * deformVertex->BoneWeight[1]
							+ matrix[2].c4 * deformVertex->BoneWeight[2]
							+ matrix[3].c4 * deformVertex->BoneWeight[3];

				//d
				outMatrix.d1 = matrix[0].d1 * deformVertex->BoneWeight[0]
							+ matrix[1].d1 * deformVertex->BoneWeight[1]
							+ matrix[2].d1 * deformVertex->BoneWeight[2]
							+ matrix[3].d1 * deformVertex->BoneWeight[3];

				outMatrix.d2 = matrix[0].d2 * deformVertex->BoneWeight[0]
							+ matrix[1].d2 * deformVertex->BoneWeight[1]
							+ matrix[2].d2 * deformVertex->BoneWeight[2]
							+ matrix[3].d2 * deformVertex->BoneWeight[3];

				outMatrix.d3 = matrix[0].d3 * deformVertex->BoneWeight[0]
							+ matrix[1].d3 * deformVertex->BoneWeight[1]
							+ matrix[2].d3 * deformVertex->BoneWeight[2]
							+ matrix[3].d3 * deformVertex->BoneWeight[3];

				outMatrix.d4 = matrix[0].d4 * deformVertex->BoneWeight[0]
							+ matrix[1].d4 * deformVertex->BoneWeight[1]
							+ matrix[2].d4 * deformVertex->BoneWeight[2]
							+ matrix[3].d4 * deformVertex->BoneWeight[3];
			}

			deformVertex->Position = mesh->mVertices[v];
			deformVertex->Position *= outMatrix;

			//法線変換用に移動成分を削除
			outMatrix.a4 = 0.0f;
			outMatrix.b4 = 0.0f;
			outMatrix.c4 = 0.0f;

			deformVertex->Normal = mesh->mNormals[v];
			deformVertex->Normal *= outMatrix;

			vertex[v].Position.x = deformVertex->Position.x;
			vertex[v].Position.y = deformVertex->Position.y;
			vertex[v].Position.z = deformVertex->Position.z;

			vertex[v].Normal.x = deformVertex->Normal.x;
			vertex[v].Normal.y = deformVertex->Normal.y;
			vertex[v].Normal.z = deformVertex->Normal.z;

			vertex[v].TexCoord.x = mesh->mTextureCoords[0][v].x;
			vertex[v].TexCoord.y = mesh->mTextureCoords[0][v].y;

			vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		Renderer::GetDeviceContext()->Unmap(m_VertexBuffer[m], 0);

	}

}




void AnimationModel::Update(const std::string animationName,const std::string animationName2, float blendRate, int Frame)
{
	//if (m_CurAnimationName != animationName)
	//{
	//	m_t -= 0.1f;
	//	if (m_t <= 0)
	//	{
	//		m_t = 1.0f;
	//		m_CurAnimationName = animationName;
	//	}
	//}

	const aiScene* scene = m_Animation[animationName];
	const aiScene* scene2 = m_Animation[animationName2];
	if (!scene->HasAnimations())
	{
		return;
	}
	if (!scene2->HasAnimations())
	{
		return;
	}

	//calculate bone matrix from animation data
	aiAnimation* animation = scene->mAnimations[0];
	aiAnimation* animation2 = scene2->mAnimations[0];


	for (unsigned int c = 0; c < animation->mNumChannels; c++)
	{
		aiNodeAnim* nodeAnim = animation->mChannels[c];
		BONE* bone = &m_Bone[nodeAnim->mNodeName.C_Str()];

		int f;
		f = Frame % nodeAnim->mNumRotationKeys;
		aiQuaternion rot = nodeAnim->mRotationKeys[f].mValue;

		f = Frame % nodeAnim->mNumPositionKeys;
		aiVector3D pos = nodeAnim->mPositionKeys[f].mValue;


		f = Frame % nodeAnim->mNumRotationKeys;
		aiQuaternion rot2 = nodeAnim->mRotationKeys[f].mValue;

		f = Frame % nodeAnim->mNumPositionKeys;
		aiVector3D pos2 = nodeAnim->mPositionKeys[f].mValue;

		aiVector3D totalPos = pos * (1.0f - blendRate) + pos2 * blendRate;
		aiQuaternion totalRot;
		aiQuaternion::Interpolate(totalRot, rot, rot2, blendRate);

		//aiVector3D totalPos;
		//aiQuaternion totalRot;

		//if (m_t < 1.0f)
		//{
		//	//animation2 
		//	aiNodeAnim* nodeAnim2 = animation2->mChannels[c];
		//	f = Frame % nodeAnim2->mNumPositionKeys;
		//	aiVector3D pos2 = nodeAnim2->mPositionKeys[f].mValue;
		//	f = Frame % nodeAnim2->mNumRotationKeys;
		//	aiQuaternion rot2 = nodeAnim2->mRotationKeys[f].mValue;

		//	//pos
		//	totalPos.x = m_t * pos.x + (1 - m_t) * pos2.x;
		//	totalPos.y = m_t * pos.y + (1 - m_t) * pos2.y;
		//	totalPos.z = m_t * pos.z + (1 - m_t) * pos2.z;
		//	//rot
		//	aiQuaternion::Interpolate(totalRot, rot, rot2, 1 - m_t);
		//}
		//else
		//{
		//	totalPos = pos;
		//	totalRot = rot;
		//}

		bone->AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), totalRot, totalPos);
	}

	//update bone matrix
	UpdateBoneMatrix(m_Scene->mRootNode, aiMatrix4x4());

	//CPU skinning
	for (unsigned int m = 0; m < m_Scene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_Scene->mMeshes[m];

		D3D11_MAPPED_SUBRESOURCE ms;
		Renderer::GetDeviceContext()->Map(m_VertexBuffer[m], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		VERTEX_3D* vertex = (VERTEX_3D*)ms.pData;

		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			DEFORM_VERTEX* deformVertex = &m_DeformVertex[m][v];

			aiMatrix4x4 matrix[4];
			aiMatrix4x4 outMatrix;
			matrix[0] = m_Bone[deformVertex->BoneName[0]].Matrix;
			matrix[1] = m_Bone[deformVertex->BoneName[1]].Matrix;
			matrix[2] = m_Bone[deformVertex->BoneName[2]].Matrix;
			matrix[3] = m_Bone[deformVertex->BoneName[3]].Matrix;

			{
				//a
				outMatrix.a1 = matrix[0].a1 * deformVertex->BoneWeight[0]
					+ matrix[1].a1 * deformVertex->BoneWeight[1]
					+ matrix[2].a1 * deformVertex->BoneWeight[2]
					+ matrix[3].a1 * deformVertex->BoneWeight[3];

				outMatrix.a2 = matrix[0].a2 * deformVertex->BoneWeight[0]
					+ matrix[1].a2 * deformVertex->BoneWeight[1]
					+ matrix[2].a2 * deformVertex->BoneWeight[2]
					+ matrix[3].a2 * deformVertex->BoneWeight[3];

				outMatrix.a3 = matrix[0].a3 * deformVertex->BoneWeight[0]
					+ matrix[1].a3 * deformVertex->BoneWeight[1]
					+ matrix[2].a3 * deformVertex->BoneWeight[2]
					+ matrix[3].a3 * deformVertex->BoneWeight[3];

				outMatrix.a4 = matrix[0].a4 * deformVertex->BoneWeight[0]
					+ matrix[1].a4 * deformVertex->BoneWeight[1]
					+ matrix[2].a4 * deformVertex->BoneWeight[2]
					+ matrix[3].a4 * deformVertex->BoneWeight[3];

				//b
				outMatrix.b1 = matrix[0].b1 * deformVertex->BoneWeight[0]
					+ matrix[1].b1 * deformVertex->BoneWeight[1]
					+ matrix[2].b1 * deformVertex->BoneWeight[2]
					+ matrix[3].b1 * deformVertex->BoneWeight[3];

				outMatrix.b2 = matrix[0].b2 * deformVertex->BoneWeight[0]
					+ matrix[1].b2 * deformVertex->BoneWeight[1]
					+ matrix[2].b2 * deformVertex->BoneWeight[2]
					+ matrix[3].b2 * deformVertex->BoneWeight[3];

				outMatrix.b3 = matrix[0].b3 * deformVertex->BoneWeight[0]
					+ matrix[1].b3 * deformVertex->BoneWeight[1]
					+ matrix[2].b3 * deformVertex->BoneWeight[2]
					+ matrix[3].b3 * deformVertex->BoneWeight[3];

				outMatrix.b4 = matrix[0].b4 * deformVertex->BoneWeight[0]
					+ matrix[1].b4 * deformVertex->BoneWeight[1]
					+ matrix[2].b4 * deformVertex->BoneWeight[2]
					+ matrix[3].b4 * deformVertex->BoneWeight[3];

				//c
				outMatrix.c1 = matrix[0].c1 * deformVertex->BoneWeight[0]
					+ matrix[1].c1 * deformVertex->BoneWeight[1]
					+ matrix[2].c1 * deformVertex->BoneWeight[2]
					+ matrix[3].c1 * deformVertex->BoneWeight[3];

				outMatrix.c2 = matrix[0].c2 * deformVertex->BoneWeight[0]
					+ matrix[1].c2 * deformVertex->BoneWeight[1]
					+ matrix[2].c2 * deformVertex->BoneWeight[2]
					+ matrix[3].c2 * deformVertex->BoneWeight[3];

				outMatrix.c3 = matrix[0].c3 * deformVertex->BoneWeight[0]
					+ matrix[1].c3 * deformVertex->BoneWeight[1]
					+ matrix[2].c3 * deformVertex->BoneWeight[2]
					+ matrix[3].c3 * deformVertex->BoneWeight[3];

				outMatrix.c4 = matrix[0].c4 * deformVertex->BoneWeight[0]
					+ matrix[1].c4 * deformVertex->BoneWeight[1]
					+ matrix[2].c4 * deformVertex->BoneWeight[2]
					+ matrix[3].c4 * deformVertex->BoneWeight[3];

				//d
				outMatrix.d1 = matrix[0].d1 * deformVertex->BoneWeight[0]
					+ matrix[1].d1 * deformVertex->BoneWeight[1]
					+ matrix[2].d1 * deformVertex->BoneWeight[2]
					+ matrix[3].d1 * deformVertex->BoneWeight[3];

				outMatrix.d2 = matrix[0].d2 * deformVertex->BoneWeight[0]
					+ matrix[1].d2 * deformVertex->BoneWeight[1]
					+ matrix[2].d2 * deformVertex->BoneWeight[2]
					+ matrix[3].d2 * deformVertex->BoneWeight[3];

				outMatrix.d3 = matrix[0].d3 * deformVertex->BoneWeight[0]
					+ matrix[1].d3 * deformVertex->BoneWeight[1]
					+ matrix[2].d3 * deformVertex->BoneWeight[2]
					+ matrix[3].d3 * deformVertex->BoneWeight[3];

				outMatrix.d4 = matrix[0].d4 * deformVertex->BoneWeight[0]
					+ matrix[1].d4 * deformVertex->BoneWeight[1]
					+ matrix[2].d4 * deformVertex->BoneWeight[2]
					+ matrix[3].d4 * deformVertex->BoneWeight[3];
			}

			deformVertex->Position = mesh->mVertices[v];
			deformVertex->Position *= outMatrix;

			//法線変換用に移動成分を削除
			outMatrix.a4 = 0.0f;
			outMatrix.b4 = 0.0f;
			outMatrix.c4 = 0.0f;

			deformVertex->Normal = mesh->mNormals[v];
			deformVertex->Normal *= outMatrix;

			vertex[v].Position.x = deformVertex->Position.x;
			vertex[v].Position.y = deformVertex->Position.y;
			vertex[v].Position.z = deformVertex->Position.z;

			vertex[v].Normal.x = deformVertex->Normal.x;
			vertex[v].Normal.y = deformVertex->Normal.y;
			vertex[v].Normal.z = deformVertex->Normal.z;

			vertex[v].TexCoord.x = mesh->mTextureCoords[0][v].x;
			vertex[v].TexCoord.y = mesh->mTextureCoords[0][v].y;

			vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		Renderer::GetDeviceContext()->Unmap(m_VertexBuffer[m], 0);

	}

}



void AnimationModel::Draw()
{
	// プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set material
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Renderer::SetMaterial(material);

	for (unsigned int m = 0; m < m_Scene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_Scene->mMeshes[m];
		aiMaterial* material = m_Scene->mMaterials[mesh->mMaterialIndex];

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer[m], &stride, &offset);

		// インデックスバッファ設定
		Renderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		aiString path;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		//Set texture
		Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[path.data]);

		// ポリゴン描画
		Renderer::GetDeviceContext()->DrawIndexed(m_Scene->mMeshes[m]->mNumFaces * 3, 0, 0);
	}
}

void AnimationModel::LoadAnimation(const std::string filename, const std::string animationName)
{
	m_Animation[animationName] = aiImportFile(filename.c_str(), aiProcess_ConvertToLeftHanded);
	assert(m_Animation[animationName]);
}

void AnimationModel::CreateBone(aiNode* node)
{
	BONE bone;
	m_Bone[node->mName.C_Str()] = bone;

	for (unsigned int n = 0; n < node->mNumChildren; n++)
	{
		CreateBone(node->mChildren[n]);
	}
}

void AnimationModel::UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix)
{
	BONE* bone = &m_Bone[node->mName.C_Str()];

	aiMatrix4x4 worldMatrix;

	worldMatrix = matrix;
	worldMatrix *= bone->AnimationMatrix;

	bone->Matrix = worldMatrix;
	bone->Matrix *= bone->OffsetMatrix;

	for (unsigned int n = 0; n < node->mNumChildren; n++)
	{
		UpdateBoneMatrix(node->mChildren[n], worldMatrix);
	}
}



/*

void AnimationModel::Update(const std::string animationName, int Frame)
{
	if (m_CurAnimationName != animationName)
	{
		m_t -= 0.1f;
		if (m_t <= 0)
		{
			m_t = 1.0f;
			m_CurAnimationName = animationName;
		}
	}

	//const aiScene* scene = m_Animation[animationName];
	const aiScene* scene = m_Animation[m_CurAnimationName];
	if (!scene->HasAnimations())
	{
		return;
	}
	//calculate bone matrix from animation data
	aiAnimation* animation = scene->mAnimations[0];

	//next scene
	const aiScene* scene2 = m_Animation[animationName];
	aiAnimation* animation2 = scene2->mAnimations[0];


	for (unsigned int c = 0; c < animation->mNumChannels; c++)
	{
		aiNodeAnim* nodeAnim = animation->mChannels[c];
		BONE* bone = &m_Bone[nodeAnim->mNodeName.C_Str()];

		int f;
		f = Frame % nodeAnim->mNumRotationKeys;
		aiQuaternion rot = nodeAnim->mRotationKeys[f].mValue;

		f = Frame % nodeAnim->mNumPositionKeys;
		aiVector3D pos = nodeAnim->mPositionKeys[f].mValue;

		aiVector3D totalPos;
		aiQuaternion totalRot;

		if (m_t < 1.0f)
		{
			//animation2
			aiNodeAnim* nodeAnim2 = animation2->mChannels[c];
			f = Frame % nodeAnim2->mNumPositionKeys;
			aiVector3D pos2 = nodeAnim2->mPositionKeys[f].mValue;
			f = Frame % nodeAnim2->mNumRotationKeys;
			aiQuaternion rot2 = nodeAnim2->mRotationKeys[f].mValue;

			//pos
			totalPos.x = m_t* pos.x + (1 - m_t) * pos2.x;
			totalPos.y = m_t* pos.y + (1 - m_t) * pos2.y;
			totalPos.z = m_t* pos.z + (1 - m_t) * pos2.z;
			//rot
			aiQuaternion::Interpolate(totalRot, rot, rot2, 1- m_t);
		}
		else
		{
			totalPos = pos;
			totalRot = rot;
		}

		//bone->AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot, pos);
		bone->AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), totalRot, totalPos);
	}

	//update bone matrix
	UpdateBoneMatrix(m_Scene->mRootNode, aiMatrix4x4());

	//CPU skinning
	for (unsigned int m = 0; m < m_Scene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_Scene->mMeshes[m];

		D3D11_MAPPED_SUBRESOURCE ms;
		CRenderer::GetDeviceContext()->Map(m_VertexBuffer[m], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		VERTEX_3D* vertex = (VERTEX_3D*)ms.pData;

		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			DEFORM_VERTEX* deformVertex = &m_DeformVertex[m][v];

			aiMatrix4x4 matrix[4];
			aiMatrix4x4 outMatrix;
			matrix[0] = m_Bone[deformVertex->BoneName[0]].Matrix;
			matrix[1] = m_Bone[deformVertex->BoneName[1]].Matrix;
			matrix[2] = m_Bone[deformVertex->BoneName[2]].Matrix;
			matrix[3] = m_Bone[deformVertex->BoneName[3]].Matrix;

			{
				//a
				outMatrix.a1 = matrix[0].a1 * deformVertex->BoneWeight[0]
							+ matrix[1].a1 * deformVertex->BoneWeight[1]
							+ matrix[2].a1 * deformVertex->BoneWeight[2]
							+ matrix[3].a1 * deformVertex->BoneWeight[3];

				outMatrix.a2 = matrix[0].a2 * deformVertex->BoneWeight[0]
							+ matrix[1].a2 * deformVertex->BoneWeight[1]
							+ matrix[2].a2 * deformVertex->BoneWeight[2]
							+ matrix[3].a2 * deformVertex->BoneWeight[3];

				outMatrix.a3 = matrix[0].a3 * deformVertex->BoneWeight[0]
							+ matrix[1].a3 * deformVertex->BoneWeight[1]
							+ matrix[2].a3 * deformVertex->BoneWeight[2]
							+ matrix[3].a3 * deformVertex->BoneWeight[3];

				outMatrix.a4 = matrix[0].a4 * deformVertex->BoneWeight[0]
							+ matrix[1].a4 * deformVertex->BoneWeight[1]
							+ matrix[2].a4 * deformVertex->BoneWeight[2]
							+ matrix[3].a4 * deformVertex->BoneWeight[3];

				//b
				outMatrix.b1 = matrix[0].b1 * deformVertex->BoneWeight[0]
							+ matrix[1].b1 * deformVertex->BoneWeight[1]
							+ matrix[2].b1 * deformVertex->BoneWeight[2]
							+ matrix[3].b1 * deformVertex->BoneWeight[3];

				outMatrix.b2 = matrix[0].b2 * deformVertex->BoneWeight[0]
							+ matrix[1].b2 * deformVertex->BoneWeight[1]
							+ matrix[2].b2 * deformVertex->BoneWeight[2]
							+ matrix[3].b2 * deformVertex->BoneWeight[3];

				outMatrix.b3 = matrix[0].b3 * deformVertex->BoneWeight[0]
							+ matrix[1].b3 * deformVertex->BoneWeight[1]
							+ matrix[2].b3 * deformVertex->BoneWeight[2]
							+ matrix[3].b3 * deformVertex->BoneWeight[3];

				outMatrix.b4 = matrix[0].b4 * deformVertex->BoneWeight[0]
							+ matrix[1].b4 * deformVertex->BoneWeight[1]
							+ matrix[2].b4 * deformVertex->BoneWeight[2]
							+ matrix[3].b4 * deformVertex->BoneWeight[3];

				//c
				outMatrix.c1 = matrix[0].c1 * deformVertex->BoneWeight[0]
							+ matrix[1].c1 * deformVertex->BoneWeight[1]
							+ matrix[2].c1 * deformVertex->BoneWeight[2]
							+ matrix[3].c1 * deformVertex->BoneWeight[3];

				outMatrix.c2 = matrix[0].c2 * deformVertex->BoneWeight[0]
							+ matrix[1].c2 * deformVertex->BoneWeight[1]
							+ matrix[2].c2 * deformVertex->BoneWeight[2]
							+ matrix[3].c2 * deformVertex->BoneWeight[3];

				outMatrix.c3 = matrix[0].c3 * deformVertex->BoneWeight[0]
							+ matrix[1].c3 * deformVertex->BoneWeight[1]
							+ matrix[2].c3 * deformVertex->BoneWeight[2]
							+ matrix[3].c3 * deformVertex->BoneWeight[3];

				outMatrix.c4 = matrix[0].c4 * deformVertex->BoneWeight[0]
							+ matrix[1].c4 * deformVertex->BoneWeight[1]
							+ matrix[2].c4 * deformVertex->BoneWeight[2]
							+ matrix[3].c4 * deformVertex->BoneWeight[3];

				//d
				outMatrix.d1 = matrix[0].d1 * deformVertex->BoneWeight[0]
							+ matrix[1].d1 * deformVertex->BoneWeight[1]
							+ matrix[2].d1 * deformVertex->BoneWeight[2]
							+ matrix[3].d1 * deformVertex->BoneWeight[3];

				outMatrix.d2 = matrix[0].d2 * deformVertex->BoneWeight[0]
							+ matrix[1].d2 * deformVertex->BoneWeight[1]
							+ matrix[2].d2 * deformVertex->BoneWeight[2]
							+ matrix[3].d2 * deformVertex->BoneWeight[3];

				outMatrix.d3 = matrix[0].d3 * deformVertex->BoneWeight[0]
							+ matrix[1].d3 * deformVertex->BoneWeight[1]
							+ matrix[2].d3 * deformVertex->BoneWeight[2]
							+ matrix[3].d3 * deformVertex->BoneWeight[3];

				outMatrix.d4 = matrix[0].d4 * deformVertex->BoneWeight[0]
							+ matrix[1].d4 * deformVertex->BoneWeight[1]
							+ matrix[2].d4 * deformVertex->BoneWeight[2]
							+ matrix[3].d4 * deformVertex->BoneWeight[3];
			}

			deformVertex->Position = mesh->mVertices[v];
			deformVertex->Position *= outMatrix;

			//法線変換用に移動成分を削除
			outMatrix.a4 = 0.0f;
			outMatrix.b4 = 0.0f;
			outMatrix.c4 = 0.0f;

			deformVertex->Normal = mesh->mNormals[v];
			deformVertex->Normal *= outMatrix;

			vertex[v].Position.x = deformVertex->Position.x;
			vertex[v].Position.y = deformVertex->Position.y;
			vertex[v].Position.z = deformVertex->Position.z;

			vertex[v].Normal.x = deformVertex->Normal.x;
			vertex[v].Normal.y = deformVertex->Normal.y;
			vertex[v].Normal.z = deformVertex->Normal.z;

			vertex[v].TexCoord.x = mesh->mTextureCoords[0][v].x;
			vertex[v].TexCoord.y = mesh->mTextureCoords[0][v].y;

			vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		CRenderer::GetDeviceContext()->Unmap(m_VertexBuffer[m], 0);

	}

}



*/