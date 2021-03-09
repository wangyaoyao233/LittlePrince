#pragma once
#pragma once
#include "Main.h"

struct DirectionalLight
{
	DirectionalLight() = default;

	DirectionalLight(const DirectionalLight&) = default;
	DirectionalLight& operator=(const DirectionalLight&) = default;

	DirectionalLight(DirectionalLight&&) = default;
	DirectionalLight& operator=(DirectionalLight&&) = default;

	DirectionalLight(const XMFLOAT4& _ambient, const XMFLOAT4& _diffuse, const XMFLOAT4& _specular, const XMFLOAT3& _direction) :Ambient(_ambient), Diffuse(_diffuse), Specular(_specular), Direction(_direction), Pad() {}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float Pad;
};

struct PointLight
{
	PointLight() = default;

	PointLight(const PointLight&) = default;
	PointLight& operator=(const PointLight&) = default;

	PointLight(PointLight&&) = default;
	PointLight& operator=(PointLight&&) = default;

	PointLight(const XMFLOAT4& _ambient, const XMFLOAT4& _diffuse, const XMFLOAT4& _specular, const XMFLOAT3& _position, float _range, const XMFLOAT3& _att) : Ambient(_ambient), Diffuse(_diffuse), Specular(_specular), Position(_position), Range(_range), Att(_att), Pad() {}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	//打包成4D向量: (Position, Range)
	XMFLOAT3 Position;
	float Range;

	//打包成4D向量: (A0, A1, A2, Pad)
	XMFLOAT3 Att;
	float Pad;
};

struct SpotLight
{
	SpotLight() = default;

	SpotLight(const SpotLight&) = default;
	SpotLight& operator=(const SpotLight&) = default;

	SpotLight(SpotLight&&) = default;
	SpotLight& operator=(SpotLight&&) = default;

	SpotLight(const XMFLOAT4& _ambient, const XMFLOAT4& _diffuse, const XMFLOAT4& _specular, const XMFLOAT3& _position, float _range, const XMFLOAT3& _direction, float _spot, const XMFLOAT3& _att) : Ambient(_ambient), Diffuse(_diffuse), Specular(_specular), Position(_position), Range(_range), Direction(_direction), Spot(_spot), Att(_att), Pad() {}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	//打包成4D向量: (Position, Range)
	XMFLOAT3 Position;
	float Range;

	//打包成4D向量: (Direction, Spot)
	XMFLOAT3 Direction;
	float Spot;

	//打包成4D向量: (A0, A1, A2, Pad)
	XMFLOAT3 Att;
	float Pad;
};