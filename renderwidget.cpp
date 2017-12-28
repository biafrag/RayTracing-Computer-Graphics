#include "renderwidget.h"

#include <QImage>
#include <QGLWidget>
#include <QMouseEvent>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <QLabel>
#include <cmath>
#include "raytracing.h"
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif


RenderWidget::RenderWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , program(nullptr)
{
    cam.at = glm::vec3(0.f,0.f,0.f);
    cam.eye =  glm::vec3(0.f,0.f,0.f);
    cam.eye.z = cam.at.z + 20.f;
    cam.up = glm::vec3(0.f,2.f,0.f);
    cam.zNear = 0.1f;
    cam.zFar  = 100.f;
    cam.fovy  = 60.f;
    cam.width = width();
    cam.height = height();
}


RenderWidget::~RenderWidget()
{
    delete program;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void RenderWidget::initializeGL()
{
    ZBuffer=true;
    //Ray=false;
    initializeOpenGLFunctions();
    percentzoom=0.8;
    mousepress=false;
    glEnable(GL_DEPTH_TEST);

    glClearColor(0,0,0,1);
    glViewport(0,0,width(),height());

    //Compilar os shaders
    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertexshader.glsl");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragmentshader.glsl");
    program->link();

    //Criar objeto e textura
    createCube();
    createTexture(":/textures/cube_texture.png");

    //createSphere();
   // createTexture(":/textures/sphere_texture.jpg");

    //Criar VBO e VAO
    createVBO();
}


void RenderWidget::paintGL()
{
    //Limpar a tela
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    //Linkar o VAO
    glBindVertexArray(VAO);

    //Linkar o programa e passar as uniformes
    program->bind();

    //Definir matriz view e projection
    view = glm::lookAt(cam.eye, cam.at, cam.up);
    proj = glm::perspective(glm::radians(cam.fovy), (float)cam.width/cam.height, cam.zNear, cam.zFar);

    QMatrix4x4 m(glm::value_ptr(glm::transpose(model)));
    QMatrix4x4 v(glm::value_ptr(glm::transpose(view)));
    QMatrix4x4 p(glm::value_ptr(glm::transpose(proj)));

    //Passar as uniformes da luz e do material
    program->setUniformValue("light.position", v*QVector3D(5,5,-5) );
    program->setUniformValue("material.ambient", QVector3D(0.3f,0.3f,0.3f));
    program->setUniformValue("material.diffuse", QVector3D(1.0f,1.0f,1.0f));
    program->setUniformValue("material.specular", QVector3D(1.0f,1.0f,1.0f));
    program->setUniformValue("material.shininess", 24.0f);

    //Definir propriedades da luz
    QVector3D aux(QVector3D(5,5,-5));
    luz.position.x=aux.x();
    luz.position.y=aux.y();
    luz.position.z=aux.z();
    luz.ambient = glm::vec3(0.3f,0.3f,0.3f);
    luz.diffuse = glm::vec3(1.0f,1.0f,1.0f);
    luz.specular = glm::vec3 (1.0f,1.0f,1.0f);
    luz.shi = 24.0f;

    //Ativar e linkar a textura
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    program->setUniformValue("sampler", 0);

    //Passar as matrizes de transformação
   // for( int x = -3; x <= 3; x+=3 )
   // {
     //   for( int z = -3; z <= 3; z+=3)
      //  {
            QMatrix4x4 mObj;
          //  mObj.translate(x,0,z);

            QMatrix4x4 mv = v * (m * mObj); //Porque não faz ao contrário?
            QMatrix4x4 mvp = p * mv;
            program->setUniformValue("mv", mv);
            program->setUniformValue("mv_ti", mv.inverted().transposed());
            program->setUniformValue("mvp", mvp);
//          if(ZBuffer ==true)
          {
            //Desenhar
            glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);

          }
      //}
    //}
         // update();
}


void RenderWidget::resizeGL(int w, int h)
{
    //Atualizar a viewport
    glViewport(0,0,w,h);

    //Atualizar a câmera
    cam.width = w;
    cam.height = h;
    glm::vec3 zero(0);
    radius=((glm::min(h,w)/2.0)-1);
}

QVector3D RenderWidget::Points_Sphere(QVector3D pointT)
{
    QVector3D pointf;
    double r,s;
    pointf.setX((pointT.x()-(cam.width/2))/radius);
    pointf.setY((pointT.y()-(cam.height/2))/radius);
    r=pointf.x()*pointf.x()+pointf.y()*pointf.y();

    if(r>1.0)
    {
        s=1.0/sqrt(r);
        pointf.setX(s*pointf.x());
        pointf.setY(s*pointf.y());
        pointf.setZ(0);
    }
    else
    {
        pointf.setZ(sqrt(1.0-r));
    }
    return pointf;

}

void RenderWidget::mousePressEvent(QMouseEvent *event)
{
    if(mousepress==false && event->button() == Qt::LeftButton)
    {
        mousepress=true;
       QVector3D point( event->x(), height()-event->y(), 0 ); // Pegando o ponto que está na tela
       point.setZ(0.f);
       p0=Points_Sphere(point);


    }
    //Fit
    if(event->button() == Qt::MiddleButton)
    {
          cam.eye =  glm::vec3(0.f,0.f,20.f);
          glm::mat4x4 aux(1.f);
          model=aux;
    }

    update();
}


void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mousepress=false;
    update();
}


void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(mousepress==true)
    {

            QVector3D point( event->x(), height()-event->y(), 0 ); // Pegando o ponto que está na tela
            point.setZ(0.f);
            p1=Points_Sphere(point);
            glm::vec3 pt0(p0.x(),p0.y(),p0.z());
            glm::vec3 pt1(p1.x(),p1.y(),p1.z());
            glm::dquat Q0(0,pt0);
            glm::dquat Q1(0,pt1);
            glm::dquat Qrot=Q1*glm::conjugate(Q0);
            glm::mat4x4 Matrot;
            Matrot=glm::mat4_cast(Qrot);
           model=Matrot*model;
           p0=p1;
    }
    update();
}


void RenderWidget::wheelEvent(QWheelEvent *event)
{
    //Aqui o zoom
      if(event->delta() > 0) //Quer dizer que estou rolando para cima-> zoom in
      {
         cam.eye=cam.eye*percentzoom;
      }
      else if(event->delta() < 0) //Quer dizer que estou rolando para baixo-> zoom out
      {
           cam.eye=cam.eye/percentzoom;
      }
      update();
}


void RenderWidget::createCube()
{
    //Definir vértices, normais e índices
    vertices = {
        { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 },
        { +1, -1, -1 }, { +1, -1, -1 }, { +1, -1, -1 },
        { +1, -1, +1 }, { +1, -1, +1 }, { +1, -1, +1 },
        { -1, -1, +1 }, { -1, -1, +1 }, { -1, -1, +1 },
        { -1, +1, -1 }, { -1, +1, -1 }, { -1, +1, -1 },
        { +1, +1, -1 }, { +1, +1, -1 }, { +1, +1, -1 },
        { +1, +1, +1 }, { +1, +1, +1 }, { +1, +1, +1 },
        { -1, +1, +1 }, { -1, +1, +1 }, { -1, +1, +1 }
    };

    normals = {
        {  0, -1,  0 }, { -1,  0,  0 }, {  0,  0, -1 },
        {  0, -1,  0 }, { +1,  0,  0 }, {  0,  0, -1 },
        {  0, -1,  0 }, { +1,  0,  0 }, {  0,  0, +1 },
        {  0, -1,  0 }, { -1,  0,  0 }, {  0,  0, +1 },
        { -1,  0,  0 }, {  0,  0, -1 }, {  0, +1,  0 },
        { +1,  0,  0 }, {  0,  0, -1 }, {  0, +1,  0 },
        { +1,  0,  0 }, {  0,  0, +1 }, {  0, +1,  0 },
        { -1,  0,  0 }, {  0,  0, +1 }, {  0, +1,  0 }
    };

    texCoords = {
        {0.25, 0.50}, {0.25, 0.50}, {0.50, 0.75},
        {0.00, 0.50}, {1.00, 0.50}, {0.75, 0.75},
        {0.00, 0.25}, {1.00, 0.25}, {0.75, 0.00},
        {0.25, 0.25}, {0.25, 0.25}, {0.50, 0.00},
        {0.50, 0.50}, {0.50, 0.50}, {0.50, 0.50},
        {0.75, 0.50}, {0.75, 0.50}, {0.75, 0.50},
        {0.75, 0.25}, {0.75, 0.25}, {0.75, 0.25},
        {0.50, 0.25}, {0.50, 0.25}, {0.50, 0.25}
    };

    indices = {
        0,   3,  6, //normal: (  0, -1,  0 )
        0,   6,  9, //normal: (  0, -1,  0 )
        12,  1, 10, //normal: ( -1,  0,  0 )
        12, 10, 21, //normal: ( -1,  0,  0 )
        18,  7,  4, //normal: ( +1,  0,  0 )
        18,  4, 15, //normal: ( +1,  0,  0 )
        22, 11,  8, //normal: (  0,  0, +1 )
        22,  8, 19, //normal: (  0,  0, +1 )
        16,  5,  2, //normal: (  0,  0, -1 )
        16,  2, 13, //normal: (  0,  0, -1 )
        23, 20, 17, //normal: (  0, +1,  0 )
        23, 17, 14  //normal: (  0, +1,  0 )
    };
}


int getIndex( int i, int j, int n )
{
    return j + i * ( n + 1 );
}
void RenderWidget::createSphere()
{
    const int n = 100;
    const int m = 100;

    const int numTriangles = 2 * n * m;
    const int numVertices = ( n + 1 ) * ( m + 1 );

    for( unsigned int i = 0; i <= n; i++ )
    {
        for( unsigned int j = 0; j <= m; j++ )
        {
            //Atualizar as coordenadas de textura
            float s = (float) i / n;
            float t = (float) j / m;
            texCoords.push_back(glm::vec2(s,t));

            //Calcula os parâmetros
            double theta = 2 * s * M_PI;
            double phi = t * M_PI;
            double sinTheta = sin( theta );
            double cosTheta = cos( theta );
            double sinPhi = sin( phi );
            double cosPhi = cos( phi );

            //Calcula os vértices == equacao da esfera
            vertices.push_back( glm::vec3(cosTheta * sinPhi,
                                          cosPhi,
                                          sinTheta * sinPhi) );
        }
    }

    normals = vertices;

    indicesS.resize(numTriangles*3);

    //Preenche o vetor com a triangulação
    unsigned int k = 0;
    for( unsigned int i = 0; i < n; i++ )
    {
        for( unsigned int j = 0; j < m; j++ )
        {
            indicesS[ k++ ] = getIndex( i + 1, j, n );
            indicesS[ k++ ] = getIndex( i + 1, j + 1, n );
            indicesS[ k++ ] = getIndex( i, j, n );


            indicesS[ k++ ] = getIndex( i + 1, j + 1, n );
            indicesS[ k++ ] = getIndex( i, j + 1, n );
            indicesS[ k++ ] = getIndex( i, j, n );
        }
    }
}


void RenderWidget::createVBO()
{
    //Construir vetor do vbo
    //OBS: Os dados já poderiam estar sendo armazenados assim na classe.
    struct vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    std::vector< vertex > vbo;
    vbo.reserve( vertices.size() );
    for( unsigned int i = 0; i < vertices.size(); i++ )
    {
        vbo.push_back({vertices[i], normals[i], texCoords[i]});
    }

    //Criar VBO, linkar e copiar os dados
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vbo.size()*sizeof(vertex), &vbo[0], GL_STATIC_DRAW);

    //Criar EBO, linkar e copiar os dados
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    //Criar VAO, linkar e definir layouts
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //Habilitar, linkar e definir o layout dos buffers
    glBindBuffer( GL_ARRAY_BUFFER, VBO );

    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE,
                           sizeof(vertex),
                           (void*)0 );

    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE,
                           sizeof(vertex),
                           (void*)sizeof(glm::vec3) );

    glEnableVertexAttribArray( 2 );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE,
                           sizeof(vertex),
                           (void*)(2*sizeof(glm::vec3)) );

    //Linkar o EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}


void RenderWidget::createTexture(const std::string& imagePath)
{
    //Criar a textura
    glGenTextures(1, &textureID);

    //Linkar (bind) a textura criada
    glBindTexture(GL_TEXTURE_2D, textureID);

    //Abrir arquivo de imagem com o Qt
    QImage texImage = QGLWidget::convertToGLFormat(QImage(imagePath.c_str()));

    //Enviar a imagem para o OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA,
                 texImage.width(), texImage.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);
}
std::vector< glm::vec3 > applyModel(std::vector< glm::vec3 > vertices, glm::mat4x4 model)
{
    std::vector<glm::vec3> result;
    for(int i=0;i<vertices.size();i++)
    {
        glm::vec4 aux(vertices[i],1);
        aux = model*aux;
        result.push_back(glm::vec3(aux.x/aux.w,aux.y/aux.w,aux.z/aux.w));
    }
    return result;

}

std::vector< glm::vec3 > applyModelNormal(std::vector< glm::vec3 > vertices, glm::mat4x4 model)
{
    std::vector<glm::vec3> result;
    for(int i=0;i<vertices.size();i++)
    {
        glm::vec4 aux(vertices[i],0);
        aux = model*aux;
        result.push_back(glm::vec3(aux.x,aux.y,aux.z));
    }
    return result;

}
QImage RenderWidget::getImageRay(void)
{
     QImage image;
     QImage Textura(":/textures/cube_texture.png");
     RayTracing aux(cam.zNear,cam.fovy,cam.width,cam.height,cam.eye,cam.at,cam.up);
     std::vector< glm::vec3 > auxv= applyModel(vertices,model);
     std::vector< glm::vec3 > auxn= applyModelNormal(normals,(glm::transpose(glm::inverse(model))));
     image =aux.VetordePixels(cam.width,cam.height,auxv,auxn,texCoords,indices,luz.position,luz.ambient,luz.diffuse,luz.specular,luz.shi,Textura);
     return image;
}
