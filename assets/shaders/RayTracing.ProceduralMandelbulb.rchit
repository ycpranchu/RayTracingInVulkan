#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_ray_tracing : require
//#extension GL_EXT_debug_printf : enable
#include "Material.glsl"

layout(binding = 4) readonly buffer VertexArray { float Vertices[]; };
layout(binding = 5) readonly buffer IndexArray { uint Indices[]; };
layout(binding = 6) readonly buffer MaterialArray { Material[] Materials; };
layout(binding = 7) readonly buffer OffsetArray { uvec2[] Offsets; };
layout(binding = 8) uniform sampler2D[] TextureSamplers;
layout(binding = 12) readonly buffer MandelbulbArray { vec4[] Mandelbulbs; };

#include "Scatter.glsl"
#include "Vertex.glsl"

hitAttributeEXT vec4 Sphere;
rayPayloadInEXT RayPayload Ray;

vec2 GetSphereTexCoord(const vec3 point)
{
	const float phi = atan(point.x, point.z);
	const float theta = asin(point.y);
	const float pi = 3.1415926535897932384626433832795;

	return vec2
	(
		(phi + pi) / (2* pi),
		1 - (theta + pi /2) / pi
	);
}

vec3 rotate( vec3 pos, float x, float y, float z )
{
	mat3 rotX = mat3( 1.0, 0.0, 0.0, 0.0, cos( x ), -sin( x ), 0.0, sin( x ), cos( x ) );
	mat3 rotY = mat3( cos( y ), 0.0, sin( y ), 0.0, 1.0, 0.0, -sin(y), 0.0, cos(y) );
	mat3 rotZ = mat3( cos( z ), -sin( z ), 0.0, sin( z ), cos( z ), 0.0, 0.0, 0.0, 1.0 );

	return rotX * rotY * rotZ * pos;
}

float dist(in vec3 p)
{
	const float pi = 3.1415926535897932384626433832795;
	p = rotate(p, sin(.25*pi), cos(.25*pi), 0.);
	vec3 zn = vec3(p.xyz);
	float m = dot(zn, zn);
	float dz = 1.;

	vec4 tp = vec4(abs(zn), m);
	float h = 0.;

	float n = 8.;

	float th, phi, rad;
	float _th, _phi, _rad;

	for (int i = 0; i < 8; i++)
	{
		rad = length(zn);
		if (rad > 2.) 
		{
		    h = 0.25*log(m)*sqrt(m)/dz;
		} 
		else 
		{
		    dz = 8.*pow(m,3.5)*dz + 1.;
		    
		    th = atan(length(zn.xy), zn.z);
		    phi = atan(zn.y, zn.x);

		    _rad = pow(rad, n);
		    _th = n * th;
		    _phi = n * phi;

		    zn.x = _rad * sin(_th) * cos(_phi);
		    zn.y = _rad * sin(_th) * sin(_phi);
		    zn.z = _rad * cos(_th);
		    zn += p;
		}

		m = dot(zn, zn);
	}

	return h;
}

void main()
{
	//debugPrintfEXT("Cube chit\n");
	// Get the material.
	const uvec2 offsets = Offsets[gl_InstanceCustomIndexEXT];
	const uint indexOffset = offsets.x;
	const uint vertexOffset = offsets.y;
	const Vertex v0 = UnpackVertex(vertexOffset + Indices[indexOffset]);
	const Material material = Materials[v0.MaterialIndex];

	// Compute the ray hit point properties.
	const vec4 sphere = Mandelbulbs[gl_InstanceCustomIndexEXT];
	const vec3 center = sphere.xyz;
	const float radius = sphere.w;
	const vec3 point = gl_WorldRayOriginEXT + gl_HitTEXT * gl_WorldRayDirectionEXT;

	// use my own normal calc
	const vec3 eps = vec3(.01, 0., 0.);
	const vec3 n = normalize(vec3( dist( point + eps - center ) - dist( point - eps - center ),
	  			dist( point + eps.yxz - center ) - dist( point - eps.yxz - center ),
	  			dist( point + eps.zyx - center ) - dist( point - eps.zyx - center ) ));

	Ray = Scatter(material, gl_WorldRayDirectionEXT, n, v0.TexCoord, gl_HitTEXT, Ray.RandomSeed);


	//const vec3 normal = (point - center) / radius;
	//const vec2 texCoord = GetSphereTexCoord(normal);

	//Ray = Scatter(material, gl_WorldRayDirectionEXT, normal, texCoord, gl_HitTEXT, Ray.RandomSeed);
}
