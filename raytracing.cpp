#include "raytracing.h"
#include "math.h"
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
RayTracing::RayTracing(void)
{

}

RayTracing::RayTracing(float n,float fov,float w,float h,glm::vec3 eye,glm::vec3 center,glm::vec3 up)
{
    f=n;
    a=2*f*tan(glm::radians(fov)/2);
    b=w*a/h;
    Ze=glm::normalize((eye-center)); //NORMALIZANDO O VETOR
    Xe=glm::normalize(cross(up,Ze));
    Ye=cross(Ze,Xe);
    O=eye;
}


bool RayTracing::InOutTriangleVerification(glm::vec3 v1,glm::vec3 v2,glm::vec3 v3,glm::vec3 PointInt,glm::vec3 n)
{
    float A1,A2,A3,At;
    float alfa1,alfa2,alfa3;
    A1=glm::dot(n,glm::cross(v2-v1,PointInt-v1))/2;
    A2=glm::dot(n,glm::cross(v3-v2,PointInt-v2))/2;
    A3=glm::dot(n,glm::cross(v1-v3,PointInt-v3))/2;
    At=glm::dot(n,glm::cross(v1-v3,v2-v3))/2;
    alfa1=A1/At;
    alfa2=A2/At;
    alfa3=A3/At;
    if(alfa1<0 || alfa1>1 || alfa2<0 || alfa2>1 || alfa3<0 || alfa3>1 )
    {
        return false;
    }
    return true;
}
glm::vec3 RayTracing::PegarCoordBaricentricas(glm::vec3 v1,glm::vec3 v2,glm::vec3 v3,glm::vec3 PointInt,glm::vec3 n)
{
    float A1,A2,A3,At;
    float alfa1,alfa2,alfa3;
    A1=glm::dot(n,glm::cross(v2-v1,PointInt-v1))/2;
    A2=glm::dot(n,glm::cross(v3-v2,PointInt-v2))/2;
    A3=glm::dot(n,glm::cross(v1-v3,PointInt-v3))/2;
    At=glm::dot(n,glm::cross(v1-v3,v2-v3))/2;
    alfa3=A1/At;
    alfa1=A2/At;
    alfa2=A3/At;
    return glm::vec3(alfa1,alfa2,alfa3);
}
glm::vec3 RayTracing::CalculateDiffuse(float dot, glm::vec2 texInt,glm::vec3 diffuse,QImage Textura)
{
    int w =Textura.width();
    int h =Textura.height();
    QColor aux= Textura.pixelColor(texInt.x*w,h-texInt.y*h);
    glm::vec3 cor(aux.red()/255.0,aux.green()/255.0,aux.blue()/255.0);
    glm::vec3 termaterm(diffuse.x*cor.x,diffuse.y*cor.y,diffuse.z*cor.z);

    if(dot<0)
    {
        return glm::vec3(0,0,0);
    }
    return dot*termaterm;

}
glm::vec3 RayTracing::CalculateAmbient(glm::vec2 texInt,glm::vec3 amb,QImage Textura)
{
    int w =Textura.width();
    int h =Textura.height();
    QColor aux= Textura.pixelColor(texInt.x*w,h-texInt.y*h);
    glm::vec3 cor(aux.red()/255.0,aux.green()/255.0,aux.blue()/255.0);
    glm::vec3 termaterm(amb.x*cor.x,amb.y*cor.y,amb.z*cor.z);

    return termaterm;

}
glm::vec3 PegarCoordBaricentricas(glm::vec3 v1,glm::vec3 v2,glm::vec3 v3,glm::vec3 PointInt,glm::vec3 n)
{
    float A1,A2,A3,At;
    float alfa1,alfa2,alfa3;
    A1=glm::dot(n,glm::cross(v2-v1,PointInt-v1))/2;
    A2=glm::dot(n,glm::cross(v3-v2,PointInt-v2))/2;
    A3=glm::dot(n,glm::cross(v1-v3,PointInt-v3))/2;
    At=glm::dot(n,glm::cross(v1-v3,v2-v3))/2;
    alfa3=A1/At;
    alfa1=A2/At;
    alfa2=A3/At;
    return glm::vec3(alfa1,alfa2,alfa3);
}
glm::vec3 RayTracing::CalculateSpecular(glm::vec3 position,glm::vec3 PointInt,glm::vec3 normalInt,glm::vec2 texInt,glm::vec3 specular,QImage Textura,float shi)
{
    int w =Textura.width();
    int h =Textura.height();
    QColor aux= Textura.pixelColor(texInt.x*w,h-texInt.y*h);
    glm::vec3 cor(aux.red()/255.0,aux.green()/255.0,aux.blue()/255.0);
    glm::vec3 N = glm::normalize(normalInt);
    glm::vec3 L=glm::normalize(position-PointInt);
    float dot=glm::dot(L,N);
    glm::vec3 V=glm::normalize(O-PointInt);
    glm::vec3 H=glm::normalize(L+V);
    float i=glm::dot(N,H);
    float ispec;

    if(i>0)
    {
        ispec=pow(glm::dot(N,H),shi);
    }
    else
    {
        ispec=0;
    }
    glm::vec3 termaterm(specular.x,specular.y,specular.z);

    return ispec*termaterm;

}

QImage RayTracing::VetordePixels(int w,int h,
                                              std::vector< glm::vec3 > vertices,
                                              std::vector< glm::vec3 > normals,
                                              std::vector< glm::vec2 > texCoords,
                                              std::vector< unsigned int > indices,
                                              glm::vec3 position,glm::vec3 ambient,glm::vec3 diffuse,glm::vec3 specular,float shi,QImage Textura)
{
    QImage image(w, h, QImage::Format_RGB32);
    image.fill("red");


    #pragma omp parallel for
    for(int y = 0;y<h;y++)
    {
        for(int x=0;x<w;x++)
        {
            glm::vec3 PointInt;

            float menor=FLT_MAX;
            glm::vec3 nMen;
            glm::vec3 vMen;
            glm::vec2 texMen;
            int ind1,ind2,ind3;
            glm::vec3 d=-f*Ze+a*(((float)(h-y)/(float)h)-0.5)*Ye+b*(((float)x/(float)w)-0.5)*Xe;
            for(unsigned int i = 0;i<indices.size()-2;i=i+3)
            {
                QPoint aux(x,y);
                float t;
                glm::vec3 n;
                int i1,i2,i3;
                i1=indices[i];
                i2=indices[i+1];
                i3=indices[i+2];
                glm::vec3 e1=vertices[i2]-vertices[i1];
                glm::vec3 e2=vertices[i3]-vertices[i2];
                glm::vec3 e3=vertices[i1]-vertices[i3];

                n=(glm::normalize(glm::cross(e2,e3)));

                if(glm::dot(d,n)!=0)
                {

                    t=(glm::dot(vertices[i1]-O,n))/(glm::dot(d,n));
                    PointInt=O+t*d;
                    bool verif =InOutTriangleVerification(vertices[i1],vertices[i2],vertices[i3],PointInt,n);
                    if(verif && t<menor && t>0)
                    {
                        menor=t;
                        ind1=i1;
                        ind2=i2;
                        ind3=i3;
                        vMen=vertices[i1];
                        nMen=n;
                        texMen=texCoords[i1];

                    }
                }
            }
            if(menor<FLT_MAX)
        {
            PointInt=O+menor*d;
            glm::vec3 alfas= PegarCoordBaricentricas(vertices[ind1],vertices[ind2],vertices[ind3],PointInt,nMen);
            glm::vec3 normalInt=alfas.x*normals[ind1]+alfas.y*normals[ind2]+alfas.z*normals[ind3];
            glm::vec2 texInt=alfas.x*texCoords[ind1]+alfas.y*texCoords[ind2]+alfas.z*texCoords[ind3];
            glm::vec3 N = glm::normalize(normalInt);
            glm::vec3 L = glm::normalize(position-PointInt);
            float dot =glm::dot(L,N);
            glm::vec3 diff=CalculateDiffuse(dot,texInt,diffuse,Textura);
            glm::vec3 amb=CalculateAmbient(texInt,ambient,Textura);
            QColor cor;
           if(dot>0)
            {
                glm::vec3 spec=CalculateSpecular(position,PointInt,normalInt,texInt,specular,Textura,shi);
                glm::vec3 coraux((diff.x+amb.x+spec.x)*255,(diff.y+amb.y+spec.y)*255,(diff.z+amb.z+spec.z)*255);
                if(coraux.x>255)
                {
                    coraux.x=255;
                }
                if(coraux.y>255)
                {
                    coraux.y=255;
                }
                if(coraux.z>255)
                {
                    coraux.z=255;
                }
                cor= QColor(coraux.x,coraux.y,coraux.z);
            }
            else
            {
                 cor = QColor((diff.x+amb.x)*255,(diff.y+amb.y)*255,(diff.z+amb.z)*255);
            }
           image.setPixelColor(x,y,cor);
          }
          else
            image.setPixelColor(x,y,QColor(0,0,0));
        }

    }

  return image;
}
