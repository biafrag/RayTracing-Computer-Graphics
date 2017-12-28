#include "image.h"
//int gama;
#include <QtMath>

Image::Image()
    : _width(0), _height(0), _depth(0),gama(1)
{
}


Image::Image(const Image& orig)
    : _width(orig._width)
    , _height(orig._height)
    , _depth(orig._depth)
    , _buffer(orig._buffer)
    , gama(1)
{
}


Image::Image(int width, int height, int depth)
    : _width(width)
    , _height(height)
    , _depth(depth)
    , _buffer(_width * _height * _depth, 0.0f) //Inicializa o buffer com 0's
    ,gama(1)
{
}


Image::Image(int width, int height, int depth, float* buffer)
    : _width(width)
    , _height(height)
    , _depth(depth)
    , _buffer(buffer, buffer + _width * _height * _depth)
    ,gama(1)
{
}


Image::~Image()
{
}


void Image::getDimensions(int& width, int& height, int& depth)
{
    width = _width;
    height = _height;
    depth = _depth;
}


std::vector<float>& Image::getColorBuffer()
{
    return _buffer;
}


Image Image::toGrayscale()
{
    if (_depth==1) return *this;

    Image outImage(_width,_height, 1);

    std::vector<float>& dst = outImage.getColorBuffer();

    for( int y = 0; y < _height; y++ )
    {
        for( int x = 0; x < _width; x++ )
        {
            int k = y*_width*_depth+x*_depth;
            dst[y*_width+x]=0.2126f*_buffer[k] +0.7152f*_buffer[k+1]+0.0722f*_buffer[k+2];
        }
    }
   return outImage;
}


void Image::fromQImage(const QImage &qImage)
{
    if( !qImage.isNull() )
    {
        _buffer.clear();

        _width = qImage.width();
        _height = qImage.height();
        _depth = qImage.isGrayscale() ? 1 : 3;

        _buffer.resize(_width*_height*_depth, 0.0f);

        int pos = 0;
        for( int y = 0; y < _height; y++ )
        {
            for( int x = 0; x < _width; x++ )
            {
                QColor color = qImage.pixel(x,y);

                _buffer[pos++] = color.red()/255.f;

                if( _depth == 3 )
                {
                    _buffer[pos++] = color.green()/255.f;
                    _buffer[pos++] = color.blue()/255.f;
                }
            }
        }
    }
}

int Image::get_gama (void)
{
    return gama;
}
void Image::set_gama(int value)
{
    gama=value;
}

unsigned char fix(float value){
    value=(value<0)?-value:value;
    value=(value>1)?1:value;
    return (unsigned char) (255*value);
}


QImage Image::toQImage()
{
    QImage qImage( _width, _height, QImage::Format_RGB32 );

    if( !_buffer.empty() )
    {
        int pos = 0;
        for( int y = 0; y < _height; y++ )
        {
            for( int x = 0; x < _width; x++ )
            {
                float value = fix(_buffer[pos++]);
                QColor color(value, value, value);

                if( _depth == 3 )
                {
                    color.setGreen(fix(_buffer[pos++]));
                    color.setBlue(fix(_buffer[pos++]));
                }

                qImage.setPixelColor(x,y,color);
            }
        }
    }
    return qImage;
}


/** Algoritmos a serem implementados **/


void smoothing(Image& input, Image& output) //usar filtro gaussiano -> ok
{

    int height,width,depth,i,j,pos,posmed;
    std::vector<int> Vpos;
    Vpos.resize(9);
    input.getDimensions(width,height,depth);
    std::vector<float> in=input.getColorBuffer();
    std::vector<float> out;
    out.resize(in.size());
    out=in;
    float soma;
    //passando gauss
 for(int z=0;z<depth;z++)
 {
     for(j=0;j<(height-3);j=j+1)
     {

       for(i=0;i<(width-3);i++)
       {
           /* gaussiano
            *pos[0] pos[1] pos[2]
            *pos[3] pos[4] pos[5]
            *pos[6] pos[7] pos[8]
            *
            *
            */

         Vpos[0]=j*width*depth+i*depth+z;
         Vpos[1]=j*width*depth+(i+1)*depth+z;
         Vpos[2]=j*width*depth+(i+2)*depth+z;
         Vpos[3]=(j+1)*width*depth+i*depth+z;
         Vpos[4]=(j+1)*width*depth+(i+1)*depth+z;
         Vpos[5]=(j+1)*width*depth+(i+2)*depth+z;
         Vpos[6]=(j+2)*width*depth+i*depth+z;
         Vpos[7]=(j+2)*width*depth+(i+1)*depth+z;
         Vpos[8]=(j+2)*width*depth+(i+2)*depth+z;
         posmed=Vpos[4];
         soma=(1*in[Vpos[0]]+2*in[Vpos[1]]+1*in[Vpos[2]]+2*in[Vpos[3]]+4*in[Vpos[4]]+2*in[Vpos[5]]+1*in[Vpos[6]]+2*in[Vpos[7]]+1*in[Vpos[8]])/16;
         out[posmed]=soma;
       }
     }
}

    output = Image(width,height,depth,&out[0]);
}


void sobel(Image& input, Image& output) //ok
{
    Image gray =input.toGrayscale();
    int height,width,depth,i,j,pos,posmed;
    std::vector<int> Vposx,Vposy;
    Vposx.resize(6);
    Vposy.resize(6);
    gray.getDimensions(width,height,depth);
    std::vector<float> out,aux;
    aux.resize(gray.getColorBuffer().size());
    aux=gray.getColorBuffer();
    out.resize(out.size());
    out=aux;
    float somax,somay,result;
   //zerando as bordas -> Porque n
    for(i =0;i<width;i++)
    {
        pos=0*width*depth+i*depth;
        out[pos]=0;
    }
    for(j =0;j<height;j++)
    {
        pos=j*width*depth+0*depth;
       out[pos]=0;
    }
    for(i =0;i<width;i++)
    {
        pos=(height-1)*width*depth+i*depth;
        out[pos]=0;
    }
    for(j =0;j<width;j++)
    {
        pos=j*width*depth+(width-1)*depth;
        out[pos]=0;
    }
    //passando sobel
 for(j=0;j<(height-3);j=j+1)
 {

   for(i=0;i<(width-3);i++)
   {
       /* em x
        *pos[0] 0 pos[1]
        *pos[2] 0 pos[3]
        *pos[4] 0 pos[5]
        *
        * em y
        *pos[0] pos[1] pos[2]
        * 0      0      0
        *pos[3] pos[4] pos[5]
        *
        */
     somax=somay=0;
     Vposx[0]=j*width*depth+i*depth;
     Vposx[1]=j*width*depth+(i+2)*depth;
     Vposx[2]=(j+1)*width*depth+i*depth;
     Vposx[3]=(j+1)*width*depth+(i+2)*depth;
     Vposx[4]=(j+2)*width*depth+i*depth;
     Vposx[5]=(j+2)*width*depth+(i+2)*depth;
     Vposy[0]=Vposx[0];
     Vposy[1]=Vposx[1]=j*width*depth+(i+1)*depth;
     Vposy[2]=Vposx[1];
     Vposy[3]=Vposx[4];
     Vposy[4]=Vposx[5]=(j+2)*width*depth+(i+1)*depth;
     Vposy[5]=Vposx[5];
     posmed=(j+1)*width*depth+(i+1)*depth;
     somax=(-1*aux[Vposx[0]]+1*aux[Vposx[1]]-2*aux[Vposx[2]]+2*aux[Vposx[3]]-1*aux[Vposx[4]]+1*aux[Vposx[5]])/4;
     somay=(1*aux[Vposy[0]]+2*aux[Vposy[1]]+1*aux[Vposy[2]]-1*aux[Vposy[3]]-2*aux[Vposy[4]]-1*aux[Vposy[5]])/4;
     result=sqrt(somay*somay+somax*somax);
     out[posmed]=result;
   }
 }

    output = Image(width,height,depth,&out[0]);
}


void haar(Image& input, Image& output) //ok
{
    int i,n,j,posf,pos,pos1,l,i1;
    int height,width,depth;
    std::vector<float> in=input.getColorBuffer();
    std::vector<float> out;
    std::vector<float> finale;
    out.resize(in.size());
    finale.resize(in.size());
    input.getDimensions(width,height,depth);
    for(n=0;n<depth;n++)
    {
        for(j=0;j<height;j++)
        {
            for(i=0,l=0;i<(width);i=i+2,l++)
            {
               pos=j*width*depth+i*depth+n;
               pos1=j*width*depth+(i+1)*depth+n;
               posf=j*width*depth+(l)*depth+n;
              out[posf]=(in[pos]+in[pos1])/2;

            }
            for(i=0;i<width;l++,i=i+2)
            {
                pos=j*width*depth+i*depth+n;
                pos1=j*width*depth+(i+1)*depth+n;
                posf=j*width*depth+(l)*depth+n;
                out[posf]=(in[pos]-in[pos1])/2;

            }
         }
        for(i=0;i<width;i++)
        {
            for(j=0,l=0;j<height;j=j+2,l++)
            {
               pos=j*width*depth+i*depth+n;
               pos1=(j+1)*width*depth+i*depth+n;
               posf=l*width*depth+i*depth+n;
              finale[posf]=(out[pos]+out[pos1])/2;

            }
            for(j=0;j<width;l++,j=j+2)
            {
                pos=j*width*depth+i*depth+n;
                pos1=(j+1)*width*depth+i*depth+n;
                posf=l*width*depth+i*depth+n;
               finale[posf]=(out[pos]-out[pos1])/2;

            }
         }
    }
    output = Image(width,height,depth,&finale[0]);
}


void haarInv(Image& input, Image& output) //ainda não acabada
{
    int i,n,j,posf,pos,pos1,l;
    int height,width,depth;
    std::vector<float> in=input.getColorBuffer();
    std::vector<float> out;
    std::vector<float> finale;
    out.resize(in.size());
    finale.resize(out.size());
    input.getDimensions(width,height,depth);
    for(n=0;n<depth;n++)
    {
        for(i=0;i<width;i++)
        {
            for(j=0,l=0;j<height/2;j=j+1,l=l+2)
            {
               pos=j*width*depth+i*depth+n;
               pos1=((height/2)+j)*width*depth+i*depth+n;
               posf=l*width*depth+i*depth+n;
              out[posf]=(in[pos]+in[pos1]);
              out[(l+1)*width*depth+i*depth+n]=(in[pos]-in[pos1]);
           }
       }
        for(j=0;j<height;j++)
        {
            for(i=0,l=0;i<width/2;i=i+1,l=l+2)
            {
                pos=j*width*depth+i*depth+n;
                pos1=j*width*depth+((width/2)+i)*depth+n;
                posf=j*width*depth+l*depth+n;
                finale[posf]=(out[pos]+out[pos1]);
                finale[j*width*depth+(l+1)*depth+n]=(out[pos]-out[pos1]);

            }
         }

    }

    output = Image(width,height,depth,&finale[0]);
}


void enhanceHaar(Image& input, Image& output)
{
    std::vector<float> in=input.getColorBuffer();
    std::vector<float> aux,out;
    aux.resize(in.size());
    //out.resize(in.size());
    aux=in;
    //out=in;
    int height,width,depth;
    int i,j,n;
    int pos;
    float v;
    //output.set_gama=1;
    int gama=input.get_gama();
    input.getDimensions(width,height,depth);
    printf("gama: %d\n",input.get_gama());

    //O certo
    for(i=(width/2)-1;i<width;i++)
    {
        for(j=0;j<height/2;j++)
        {
            pos=j*width*depth+i*depth;
            v=sqrt(pow(aux[pos+0],2)+pow(aux[pos+1],2)+pow(aux[pos+2],2));
            v=pow(v,(1.0)/gama);
            aux[pos+0]=v;
            aux[pos+1]=v;
            aux[pos+2]=v;
        }
    }
    for(i=0;i<width;i++)
    {
        for(j=(height/2)-1;j<height;j++)
        {
            pos=j*width*depth+i*depth;
            v=sqrt(pow(aux[pos+0],2)+pow(aux[pos+1],2)+pow(aux[pos+2],2));
            v=pow(v,(1.0)/gama);
            aux[pos+0]=v;
            aux[pos+1]=v;
            aux[pos+2]=v;
        }
    }


    output = Image(width,height,depth,&aux[0]);
    //output.set_gama(gama);
}
void printavectorbuffer(std::vector<float> aux,int height,int width,int depth)
{

    int i,j,pos,n;
  //for(n=0;n<depth;n++)
   //{
      //std::cout<<"Canal "<<n<<"\n";
        for(i=0;i<width;i++)
        {
            for(j=0;j<height;j++)
            {
               pos=j*width*depth+i*depth+0;
               std::cout<<""<<aux[pos]<<" ";
            }
            std::cout<<"\n";
        }
         std::cout<<"\n";
  //}
}
