#version 430 core

#define M_PI 3.1415926535897932384626433832795

#define EPS 1e-2

#define SAMPLES_COUNT 1
#define POINT_LIGHT 0
#define VECTOR_LIGHT 1
#define UNIFORM_TEXTURE 0
#define MAPPED_TEXTURE 1

struct Light {
    vec3 col;
    float intensity;
    vec3 v;
    uint type;
};

struct Material {
    uint alb;
    uint rgh;
    uint ao;
    uint nrm;
};

struct Sphere {
    vec3 center;
    float radius;
    uint mat;
    vec3 padding;
};

struct TextureMeta {
    vec3 val;
    uint idx;
};

struct Intersection {
    float d;
    vec3 normal;
    vec3 pWorld;
    vec2 pTex;
    uint mat;
};

layout (binding = 0, rgba32f) uniform writeonly image2D framebuffer;
uniform uint size;

layout(binding = 3) buffer SphereBuffer {
    Sphere spheres[];
};

layout(binding = 4) buffer TextureBuffer {
    TextureMeta textureMeta[];
};

layout(binding = 5) buffer LightBuffer {
    Light lights[];
};

layout(binding = 6) buffer MaterialBuffer {
    Material materials[];
};

uniform sampler2DArray textureAtlas;

uniform vec3 eye;
uniform vec3 ray00;
uniform vec3 ray01;
uniform vec3 ray11;
uniform vec3 ray10;

uniform float ambient;
uniform uint textureCount;
uniform uint samplesCount;

//Geometry function
float G(const float NdotV, const float NdotL, const float rgh);
//Normal distribution
float D(const float NdotH, const float rgh);
//Frenel equation
vec3 F(const float VdotH, const vec3 F0);
vec3 lightImpact(const uint idx, const vec3 N, const vec3 p);
vec3 lightDirection(const uint idx, const vec3 p);
//BRDF
vec3 countIrradiance(const vec3 p, const vec3 v, const vec3 n, const vec2 vt, const uint mat);

bool intersectSphere(int i, const vec3 origin, const vec3 dir, float tMin, float tMax, out Intersection is);
vec3 trace(const vec3 origin, const vec3 dir);
bool nearestCollision(const vec3 origin, const vec3 dir, float tMin, float tMax, inout Intersection is);
bool anyCollision(const vec3 origin, const vec3 dir, float tMin, float tMax);
vec3 textureAtPoint(const uint idx, const vec2 point);

uint nextPowerOfTwo(uint v);

layout (local_size_x = 32, local_size_y = 32) in;
void main(void) {
    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(framebuffer);
    if (pix.x >= size.x || pix.y >= size.y){
        return;
    }
    vec2 pos = vec2(pix) / vec2(size.x - 1, size.y - 1);
    vec3 dir = mix(mix(ray00, ray01, pos.y), mix(ray10, ray11, pos.y), pos.x);
    vec3 col = vec3(0);
    double eps = float(EPS) / float(SAMPLES_COUNT) / 2.0;
    int lines = int(abs(sqrt(SAMPLES_COUNT)));
    for (int x = 0; x < lines; ++x) {
        for (int y = 0; y < lines; ++y) {
            col += trace(eye, dir + vec3((x - lines / 2.0) * eps, (y - lines / 2.0) * eps, 0));

        }
    }
    col /= lines * lines;
    imageStore(framebuffer, pix, vec4(col, 0));
}

float getCoord(uint capacity, uint layer)
{
    return max(0, min(float(capacity - 1), floor(float(layer) + 0.5)));
}

vec3 textureAtPoint(const uint idx, const vec2 point) {
    return texture(textureAtlas, vec3(point, getCoord(textureCount, idx))).rgb;
}

vec3 trace(const vec3 origin, const vec3 dir) {
    Intersection is;
    if (nearestCollision(origin, dir, 1, 1000, is)) {
        return textureAtPoint(materials[is.mat].alb, is.pTex) * ambient + countIrradiance(is.pWorld, dir, is.normal, is.pTex, is.mat);
    } else {
        return vec3(1.0, 1.0, 1.0);
    }
}

bool nearestCollision(const vec3 origin, const vec3 dir, float tMin, float tMax, inout Intersection nearest) {
    int len = spheres.length();
    bool foundOne = false;
    for (int i = 0; i < len; i++){
        Intersection is;
        if (intersectSphere(i, origin, dir, tMin, tMax, is)) {
            if (!foundOne || nearest.d > is.d) {
                nearest = is;
                foundOne = true;
            }
        }
    }
    return foundOne;
}

bool anyCollision(const vec3 origin, const vec3 dir, float tMin, float tMax) {
    int len = spheres.length();
    Intersection is;
    for (int i = 0; i < len; i++){
        if (intersectSphere(i, origin, dir, tMin, tMax, is)) {
            return true;
        }
    }
    return false;
}

bool intersectSphere(int i, const vec3 origin, const vec3 dir, float tMin, float tMax, out Intersection is) {
    float R = spheres[i].radius;
    vec3 O = spheres[i].center;
    vec3 OC = origin - O;
    float k1 = 2 * dot(dir, dir);
    float k2 = 2 * dot(dir, OC);
    float k3 = dot(OC, OC) - R * R;
    float d = k2 * k2 - 2 * k1 * k3;
    if (d < 0) {
        return false;
    }
    float sqr = sqrt(d);
    float t1 = (sqr - k2) / (k1);
    float t2 = (-sqr - k2) / (k1);
    bool b1 = t1 > tMin && t1 < tMax,
    b2 = t2 > tMin && t2 < tMax;
    if (b1 || b2) {
        float min = 0;
        if (b1 && b2) {
            min = t1 < t2 ? t1 : t2;
        } else {
            min = b1 ? t1 : t2;
        }
        vec3 p_world = origin + min * dir;
        vec3 p_local = p_world;
        vec3 p_n = normalize(p_local);
        float x_t = 0.5 + atan(p_n[0], p_n[2]) / (M_PI);
        float y_t = 0.5 - asin(p_n[1]) / M_PI;
        vec2 p_texture = vec2(x_t, y_t);
        vec3 normal = normalize(O - p_world);
        vec2 vt = vec2(x_t, y_t);
        is = Intersection(min, normal, p_world, vt, spheres[i].mat);
        return true;
    } else {
        return false;
    }
}

vec3 lightImpact(const uint idx, const vec3 N, const vec3 p) {
    float i;
    Light l = lights[idx];
    switch (l.type) {
        case POINT_LIGHT: {
            vec3 LP = normalize(p - l.v);
            float nrm = length(LP);
            nrm *= nrm;
            float LPdotN = dot(LP, N);
            i = l.intensity *  LPdotN / nrm;
            break;
        }
        case VECTOR_LIGHT: {
            i = l.intensity * dot(l.v, N);
            break;
        }
    }
    return i > 0 ? l.col * i : vec3(0, 0, 0);
}

vec3 lightDirection(const uint idx, const vec3 p) {
    Light l = lights[idx];
    switch (l.type) {
        case POINT_LIGHT: {
            return normalize(p - l.v);
        }
        case VECTOR_LIGHT: {
            return normalize(l.v);
        }
    }
    return vec3(0);
}
//Schlick GGX attenuation
float G(const float NdotV, const float NdotL, const float rgh) {
    float mult1 = NdotV / (NdotV * (1.0 - rgh) + rgh);
    float mult2 = NdotL / (NdotL * (1.0 - rgh) + rgh);
    return mult1 * mult2;
}
//GGX ditribution
float D(const float NdotH, const float rgh) {
    float a = rgh;// rgh^2;
    float divider = (a - 1.0) * NdotH * NdotH + 1;
    return a / (divider * divider);
}
//Schlick approximation
vec3 F(const float VdotH, const vec3 F0) {
    float p = pow(1 - VdotH, 5.0);
    return F0 + (vec3(1, 1, 1) - F0) * p;
}
//BRDF
vec3 countIrradiance(const vec3 p, const vec3 v, const vec3 normal, const vec2 vt, const uint mat) {
    vec3 irradiance = vec3(0, 0, 0);
    vec3 alb = textureAtPoint(materials[mat].alb, vt);
    vec3 Fdiffuse = alb;
    //        return textureAtPoint(mat.alb, vt);
    float rgh = textureAtPoint(materials[mat].rgh, vt).x;
    float a = rgh * rgh;
    //    float rgh = 0.9;
    vec3 N = mix(normal, textureAtPoint(materials[mat].nrm, vt), rgh);
    vec3 frenel = mix(alb, vec3(0.04), rgh);
    //        return vec3(rgh);
    //N = normal;
    //                return frenel;
    float ao = 1;
    //            return N;
    float k = (a + 1) * (a + 1) / 8;
    //    k = rgh * rgh / 2;
    float NdotH, NdotL, NdotV, VdotH;
    float _G, _D;
    vec3 _F;
    const uint lightCount = lights.length();
    for (int i = 0; i < lightCount; ++i) {
        vec3 l = lightDirection(i, p);
        if (anyCollision(p, -l, 1e-2, 1000)) {
            continue;
        }
        vec3 h = normalize(l + v);
        NdotL = max(dot(N, l), 0.0);
        NdotV = max(dot(N, v), 0.0);
        NdotH = max(dot(N, h), 0.0);
        VdotH = max(dot(v, h), 0.0);
        float divider = 4 * NdotL * NdotV + 1e-16;
        _G = G(NdotV, NdotL, k);
        //        return vec3(_G);
        _D = D(NdotH, a);
        //        return vec3(_D);
        _F = F(VdotH, frenel);
        //        return _F;
        vec3 Fcook_torrance = _G * _F * _D / divider;
        //        return Fcook_torrance;
        vec3 L = lightImpact(i, N, p);
        //        return L;
        irradiance += (rgh * Fdiffuse + Fcook_torrance) * L;
    }
    return irradiance * ao / M_PI;
}
