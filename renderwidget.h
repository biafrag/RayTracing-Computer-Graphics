#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>

#include <vector>
#include "raytracing.h"
#include "glm/glm.hpp"

class RenderWidget
        : public QOpenGLWidget
        , protected QOpenGLExtraFunctions
{
public:
    RenderWidget(QWidget* parent);
    virtual ~RenderWidget();
    bool ZBuffer;
    //bool Ray;
    QImage getImageRay(void);

private:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    void createCube();
    void createSphere();
    void createVBO();
    void createTexture(const std::string& imagePath);

    QOpenGLShaderProgram* program;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec3 > normals;
    std::vector< glm::vec2 > texCoords;
    std::vector< unsigned int > indices;
     std::vector< unsigned int >indicesS;

    struct Camera {
       glm::vec3 eye;      /* posicao do olho ou centro de projecao conica */
       glm::vec3 at;       /* ponto para onde a camera aponta              */
       glm::vec3 up;       /* orientacao da direcao "para cima" da camera  */
       float fovy;         /* angulo de abertura da camera                 */
       float zNear,zFar;   /* distancia do plano proximo e distante        */
       float width,height; /* largura e altura da janela em pixels         */
    };
    struct Luz {
       glm::vec3 position;      /* posicao da luz */
       glm::vec3 ambient;       /* componente ambiente da luz */
       glm::vec3 diffuse;       /* componente difusa da luz */
       glm::vec3 specular;      /* componente especular da luz */
       float shi; /* luminosidade */
    };

    Luz luz;
    Camera cam;
    glm::mat4x4 model;
    glm::mat4x4 view;
    glm::mat4x4 proj;
    glm::vec3 pt0;
    //glm::mat4x4 Mrot;
    float percentzoom;
    bool mousepress;
    QVector3D p0,p1; //pontos para fazer rotação
    unsigned int textureID;
    double radius; //Sphere Radius
    QVector3D Points_Sphere(QVector3D pointT);
};

#endif // RENDERWIDGET_H
