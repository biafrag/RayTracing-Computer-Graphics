#ifndef RAYTRACING_H
#define RAYTRACING_H
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <QOpenGLWidget>
class RayTracing
{
public:
    RayTracing();
    RayTracing(float n,float fov,float w,float h,glm::vec3 eye,glm::vec3 center,glm::vec3 up);
    bool InOutTriangleVerification(glm::vec3 v1,glm::vec3 v2,glm::vec3 v3,glm::vec3 PointInt,glm::vec3 n);
    QImage VetordePixels(int w, int h,
                                                  std::vector< glm::vec3 > vertices,
                                                  std::vector< glm::vec3 > normals,
                                                  std::vector< glm::vec2 > texCoords,
                                                  std::vector< unsigned int > indices, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shi, QImage Textura);
    glm::vec3 CalculateDiffuse(float dot,glm::vec2 texInt,glm::vec3 diffuse,QImage Textura);
    glm::vec3 CalculateAmbient(glm::vec2 texInt,glm::vec3 amb,QImage Textura);
    glm::vec3 CalculateSpecular(glm::vec3 position,glm::vec3 PointInt,glm::vec3 normalInt,glm::vec2 texInt,glm::vec3 specular,QImage Textura,float shi);
    glm::vec3 PegarCoordBaricentricas(glm::vec3 v1,glm::vec3 v2,glm::vec3 v3,glm::vec3 PointInt,glm::vec3 n);
private:
    float f,a,b;
    glm::vec3 O,Ze,Xe,Ye; //olho -> centro óptico
      //distancia do raio para a imagem
};

#endif // RAYTRACING_H
