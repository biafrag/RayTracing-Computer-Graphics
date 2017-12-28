#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <vector>
#include <iostream>
#include <QGenericMatrix>
#include <math.h>

class Image
{
public:
    /**
     * Construtor vazio. buffer = nullptr.
     */
    Image();

    /**
     * Construtor de cópia
     */
    Image(const Image& orig);

    /**
     * Construtor. Aloca o buffer com tamanho width * height * depth.
     */
    Image(int width, int height, int depth);

    /**
     * Construtor. Inicializa as dimensões e o buffer com os valores
     * passados como argumento. Torna-se owner do buffer.
     */
    Image(int width, int heigth, int depth, float* buffer);

    /**
     * Destrutor. Desaloca o buffer.
     */
    virtual ~Image();

    /**
     * Retorna as dimensões da imagem: largura, altura e profundidade
     */
    void getDimensions(int& width, int& height, int& depth);

    /**
     * Retorna o color buffer.
     * A ordenação do buffer deve seguir o padrão:
     * index = y * width * depth + x * depth;
     */
    std::vector<float>& getColorBuffer();

    /**
     * Retorna uma imagem em tons de cinza, convertendo os pixels da imagem
     * original para o valor de sua luminância.
     */
    Image toGrayscale();

    /**
     * Funções de comunicação com o QT (classe QImage)
     * para permitir exibição e leitura/escrita
     */
    void fromQImage(const QImage& qImage);
    int get_gama();
    void set_gama(int value);
    QImage toQImage();

protected:
    int _width; //largura da imagem
    int _height; //altura da imagem
    int _depth;  //numero de canais -> se eh RGB ou preto e branco
    int gama;
    std::vector<float> _buffer; //armazena os pixels da imagem -> para acessar um canal z de um pixel fazer: y*_width*_depth+x*_depth+z
};
void printavectorbuffer(std::vector<float> aux,int height,int width,int depth);
void smoothing(Image &input, Image& output);
void sobel(Image& input, Image& output);
void haar(Image& input, Image& output);
void haarInv(Image& input, Image& output);
void enhanceHaar(Image& input, Image& output);


#endif // IMAGE_H
