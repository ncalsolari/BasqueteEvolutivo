/* para linux, instalar os pacotes libglfw3-dev mesa-common-dev libglew-dev */
/* para compilar no linux: gcc algevol_basq.c -lglfw -lGL -lGLEW -lm -o algevol_basq.bin */



#include <GL/glew.h>  

#define GLFW_INCLUDE_NONE
#define GLFW_FALSE   0
#define speed_time 0.01
#define populacao  10
#define testetotal 100
#define posinicialX -0.95
#define posinicialY -0.95
#define g 10
#define pi 3.14
#define num_k 1


#include <GLFW/glfw3.h> /* verifique no seu SO onde fica o glfw3.h */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "time.h"





// variaveis globais
typedef struct{
    float x, y;
} coordenadas;

float matriz_pos_inicial[populacao][2]; // cada linha um individuo, coluna 0 = x coluna 1 = y
float matriz_tempo[populacao][2];
float matriz_forca_atual[populacao][2];
float matriz_cesta_atual[populacao][2];
float matriz_bola_atual[populacao][2];
float matriz_translado[populacao][2];
float matriz_translado_parcial[populacao][2];
float matriz_quina_acerto[populacao][1];
float Mmemoria[testetotal][4];
float vetor_forca_original[populacao];
float vetor_theta[populacao];
float vetor_cesta_acerto[populacao];
float vetor_distfinal[populacao];
float vetor_transldado_cesta[2];
float vetor_cesta_acertotxt[populacao];
float vetor_melhor_todos[2]; // coluna 0 o indice coluna 1 o valor
float distanciax;
float distanciay;
float subindo[populacao];
float bolaviva[populacao];

float percent_mutacao = 0.02;
int count_geracao = 1;
int predacao = 0;
float tempoteste = 0.00;
int stop = 0;
int treinos = 0;
int ind_memoria=-1;

// funcao da predacao memoria
static void pred_memoria(){

    int ind_pior=0;
    int ind_melhor = vetor_melhor_todos[0];
    float media_forca=0;
    float media_theta=0;

    //acha o pior
    for (int j=1; j<populacao; j++){

        if(j!=ind_melhor){

            if(vetor_distfinal[j]>vetor_distfinal[ind_pior]){
             ind_pior=j;
            }

        }
        

    }

    float k_dist[testetotal]; // alocar isso dinamicamente seria melhor

    for(int j=0;j<testetotal;j++){
        k_dist[j]=10;
    }


    for(int j=0; j<testetotal ; j++){
       
        k_dist[j]=sqrtf(pow(fabs(vetor_transldado_cesta[0] - Mmemoria[j][2]),2) + pow(fabs(vetor_transldado_cesta[1] - Mmemoria[j][3]),2));  

    }


     //VASCULHO OS K VALORES COM MENOR DISTANCIA GUARDO O INDICE Q ELE C REFERE DA MATRIZ PRINCIPAL 
    typedef struct{
        int indice;
        float valor;
    } struct_menores;

    struct_menores menoresK[num_k];
    int ind_menor=0;



    for(int k=0; k<num_k; k++){

        ind_menor=0;

        for(int j=1; j<testetotal; j++){

            if(k_dist[j]<k_dist[ind_menor]){
                ind_menor=j;
            }
            

        }

        menoresK[k].valor= k_dist[ind_menor];
        menoresK[k].indice=ind_menor;
        k_dist[ind_menor] = 10; //aqui eu aumento o valor do menor pra na proxima iteracao ele pegar o segundo menor valor e dps o terceiro menor valor e assim por diante

    }
        



    //PRA CADA TESTE EU PEGO OS K MENORES VALROES DA MATRIZ INICIAL PELO INDICE Q SALVEI NO PASSO ANTERIOR E CALCULO A MEDIA
    //PONHO ESSE VALOR COMO SENDO O VALOR PRO TESTE
    float medianovovalor[2];
    float mediaf=0;
    float mediatheta=0;
    int ind_media;

   
    for(int j=0; j<num_k; j++){
        ind_media=menoresK[j].indice;
        mediaf = mediaf + Mmemoria[ind_media][0];
        mediatheta = mediatheta +  Mmemoria[ind_media][1];

    }

    mediaf = mediaf/num_k;
    mediatheta = mediatheta/num_k;

    vetor_forca_original[ind_pior]=mediaf;
    vetor_theta[ind_pior]=mediatheta;
    ind_memoria = ind_pior;

    medianovovalor[0]=mediaf;
    medianovovalor[1]=mediatheta;



}

// funcao do genocidio
static void genocidio(){
    //valores maximos de força e angulo
    float rand_theta = 90;
    float rand_forca = 10;

    //reseto a porcentagem de mutacao
    percent_mutacao = 0.02;

    //salvo melhor de todos
    int ind_melhor = vetor_melhor_todos[0];


    //reseto valor de todos menos do melhor de todos
    for(int j=0; j<populacao; j++){
        if(j!=ind_melhor){

            vetor_theta[j]= ((float)rand()/(float)(RAND_MAX)) * rand_theta;
            vetor_forca_original[j]= ((float)rand()/(float)(RAND_MAX)) * rand_forca;

        }
    }

}


//funcao da predacao por sintese->nesse caso estou usando somente a predacao por memoria
static void pred_sintese(){

    int ind_pior=0;
    int ind_melhor = vetor_melhor_todos[0];
    float media_forca=0;
    float media_theta=0;

    //acha o pior
    for (int j=1; j<populacao; j++){

        if(j!=ind_melhor){

            if(vetor_distfinal[j]>vetor_distfinal[ind_pior]){
             ind_pior=j;
            }

        }
        

    }
    //faz uma media do resto da pop sem contar o melhor e o pior
    for (int j=0; j<populacao; j++){
        if(j!=ind_melhor && j!=ind_pior){
            
            media_forca = media_forca + vetor_forca_original[j];
            media_theta = media_theta + vetor_theta[j];
        }

    }

    media_forca = media_forca/(populacao-2); //media nao leva em consideracao melhor valor nem pior valor
    media_theta = media_theta/(populacao-2);

    //substitui o pior pela sintese
    vetor_forca_original[ind_pior]=media_forca;
    vetor_theta[ind_pior] = media_theta;



}

//funcao que faz o crossover
static void nova_geracao(){


    printf("melhor[%.0f]: %f\n\n",vetor_melhor_todos[0], vetor_melhor_todos[1] );
    int ind_melhor;
    float theta_melhor;
    float forca_melhor;
    int chance_mutacao;
    int gene_mutado;
    srand(time(NULL));

    ind_melhor = vetor_melhor_todos[0];

    //PEGO DADOS DO MELHOR
    theta_melhor = vetor_theta[ind_melhor];
    forca_melhor = vetor_forca_original[ind_melhor];

    //CROSSOVER
    for(int j = 0; j<populacao; j++){



        if(j!= ind_melhor){
            vetor_cesta_acerto[j]=100; //zero a avaliacao 
            vetor_theta[j]= (vetor_theta[j] + theta_melhor) / 2;
            vetor_forca_original[j]= (vetor_forca_original[j] + forca_melhor)/2;
        }


    }
    
    //MUTACAO E REARRANJO DA POPULACAO
    for(int j=0; j<populacao; j++){

       // srand(time(NULL)); srand null aqui faz com q todos os individuos tenham o msmo valor d chance d mutacao e gene mutado
       
        //esse parametro decide se o gene vai mudar pra mais ou pra menos e qual gene vai ser mutado
         chance_mutacao = rand()%100;
         gene_mutado = rand()%100;
        
        
         
        if(j != ind_melhor){

            if(gene_mutado<=50){//MUTAR O THETA

                if(chance_mutacao<=50){ //50% D CHANCE D MUTAR PRA MAIS OU PRA MENOS
                    vetor_theta[j]=vetor_theta[j]*(1 + percent_mutacao);
                }else{
                    vetor_theta[j]=vetor_theta[j]*(1- percent_mutacao);
                }

            }else{//MUDAR A FORCA

                if(chance_mutacao<=50){ //50% D CHANCE D MUTAR PRA MAIS OU PRA MENOS
                    vetor_forca_original[j]=vetor_forca_original[j]*(1 + percent_mutacao);
                }else{
                    vetor_forca_original[j]=vetor_forca_original[j]*(1 - percent_mutacao);
                }

            }

        }
         
    }



}

//funcao que faz os procedimentos na populacao e depois poe os novos valores nos individuos
static void resetapop(){

    printf("\n\n---NOVA GERACAO---\nGeracao %d\nTaxa de mutacao %f\nDistancia do melhor %f\n", count_geracao,percent_mutacao,vetor_melhor_todos[1]);
    count_geracao ++;      
    tempoteste = 0;
    // pred_sintese();
    pred_memoria();
        

    if(predacao == 15){// se ficar 15 geracoes sem melhora ele faz genocidio guardando o melhor de todos
        
        printf("geracoes sem melhora: %d\n",predacao );
        printf("GENOCIDIO\n");
        genocidio();
        predacao = 0;

    }else{
        if(predacao>3){ // se ficar 3 geracoes sem melhora ele comeca a aumentar o valor da mutacao a cada geracao
                        // o cod ta tao otimizado que dificilmente ele fica mais de 3 geracoes sem achar um melhor

            percent_mutacao += 0.028;

        }
        printf("geracoes sem melhora: %d\n",predacao );
        nova_geracao();
        predacao = predacao + 1;
    
        
    }

    printf("cestas contabilizadas com acerto: %d\n", treinos );


    //pred_memoria(); pred memoria depois do genocidio faz com q o ind memoria nao copule com o melhor de todos
  
    //coloco os novos valores nos individuos
    for(int j=0; j<populacao;j++){

        matriz_forca_atual[j][0]= cos(vetor_theta[j]*pi/180)*vetor_forca_original[j];
        matriz_forca_atual[j][1]= sin(vetor_theta[j]*pi/180)*vetor_forca_original[j];

        matriz_tempo[j][0]=speed_time;
        matriz_tempo[j][1]=speed_time;

        matriz_pos_inicial[j][0]= posinicialX; 
        matriz_pos_inicial[j][1]= posinicialY; 

        matriz_translado[j][0] = matriz_pos_inicial[j][0] + matriz_forca_atual[j][0]*matriz_tempo[j][0];
        matriz_translado[j][1] = matriz_pos_inicial[j][1] + matriz_forca_atual[j][1]*matriz_tempo[j][1] - g*matriz_tempo[j][1]*matriz_tempo[j][1]/2;

        subindo[j]=1;
        bolaviva[j]=1;

        matriz_quina_acerto[j][0]=0;
        vetor_distfinal[j]=1000;

    }




}



// funcao para processar eventos de teclado
static void key_event(GLFWwindow* window, int key, int scancode, int action, int mods){
   

}



 
int main(void){


    // inicicializando o sistema de\ janelas
    glfwInit();

    // deixando a janela invisivel, por enquanto
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

 
    // criando uma janela
    GLFWwindow* window = glfwCreateWindow(700, 700, "Basquete Evolutivo", NULL, NULL);

    
    // tornando a janela como principal 
    glfwMakeContextCurrent(window);

    // inicializando Glew (para lidar com funcoes OpenGL)
    GLint GlewInitResult = glewInit();
    printf("GlewStatus: %s", glewGetErrorString(GlewInitResult));


    // GLSL para Vertex Shader
    char* vertex_code =
    "attribute vec2 position;\n"
    "uniform mat4 mat_transformation;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = mat_transformation*vec4(position, 0.0, 1.0);\n"
    "}\n";

    // GLSL para Fragment Shader
    char* fragment_code =
    "uniform vec4 color;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = color;\n"
    "}\n";

    // Requisitando slot para a GPU para nossos programas Vertex e Fragment Shaders
    GLuint program = glCreateProgram();
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

    // Associando nosso código-fonte GLSL aos slots solicitados
    glShaderSource(vertex, 1, &vertex_code, NULL);
    glShaderSource(fragment, 1, &fragment_code, NULL);

    // Compilando o Vertex Shader e verificando erros
    glCompileShader(vertex);

    GLint isCompiled = 0;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE){

        //descobrindo o tamanho do log de erro
        int infoLength = 512;
        glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &infoLength);

        //recuperando o log de erro e imprimindo na tela
        char info[infoLength];
        glGetShaderInfoLog(vertex, infoLength, NULL, info);

        printf("Erro de compilacao no Vertex Shader.\n");
        printf("--> %s\n",&info);

    }

    

    // Compilando o Fragment Shader e verificando erros
    glCompileShader(fragment);

    isCompiled = 0;
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE){

        //descobrindo o tamanho do log de erro
        int infoLength = 512;
        glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &infoLength);

        //recuperando o log de erro e imprimindo na tela
        char info[infoLength];
        glGetShaderInfoLog(fragment, infoLength, NULL, info);

        printf("Erro de compilacao no Fragment Shader.\n");
        printf("--> %s\n",&info);

    }

    // Associando os programas compilado ao programa principal
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    // Linkagem do programa e definindo como default
    glLinkProgram(program);
    glUseProgram(program);
 

    // Preparando dados para enviar a GPU
    int num_vertices = 32;
    coordenadas vertices[52];//36
   
    float counter = 0;
    float radius = 0.03;
    float angle = 0.0;
    float x, y;

    //cria os vertices que desenham o circulo
    for(int i=0; i < num_vertices; i++){
        angle += (2.0*pi)/num_vertices;
        x = cos(angle)*radius;
        y = sin(angle)*radius;
        vertices[i].x = x ;
        vertices[i].y = y ;
        
    }


    //cesta
    vertices[32].x = -0.075;
    vertices[32].y = +0.00;
    vertices[33].x = +0.075;
    vertices[33].y = +0.00;

    //tabela
    vertices[34].x = +0.085;
    vertices[34].y = -0.01;
    vertices[35].x = +0.085;
    vertices[35].y = +0.19;

    float redey = 0.15;
    //rede
    vertices[36].x = -0.065;//41
    vertices[36].y = +0.00;
    vertices[37].x = -0.065;
    vertices[37].y = -redey;

    vertices[38].x = +0.065;//54
    vertices[38].y = +0.00;
    vertices[39].x = +0.065;
    vertices[39].y = -redey;

    vertices[40].x = -0.065;
    vertices[40].y = +0.00;
    vertices[41].x = +0.065;
    vertices[41].y = -redey;

    vertices[42].x = +0.065;
    vertices[42].y = +0.00;
    vertices[43].x = -0.065;
    vertices[43].y = -redey;

    vertices[44].x = +0.00;
    vertices[44].y = +0.00;
    vertices[45].x = -0.065;
    vertices[45].y = -redey/2;

    vertices[46].x = -0.065;
    vertices[46].y = -redey/2;
    vertices[47].x = +0.00;
    vertices[47].y = -redey + 0.02;

    vertices[48].x = +0.00;
    vertices[48].y = -redey + 0.02;
    vertices[49].x = +0.065;
    vertices[49].y = -redey/2;

    vertices[50].x = +0.065;
    vertices[50].y = -redey/2;
    vertices[51].x = +0.00;
    vertices[51].y = +0.00;

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);


    // Abaixo, nós enviamos todo o conteúdo da variável vertices.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);


    // Associando variáveis do programa GLSL (Vertex Shaders) com nossos dados
    GLint loc = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*) 0); // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
 

    // Associando nossa janela com eventos de teclado
    glfwSetKeyCallback(window, key_event); // teclado

    // Exibindo nossa janela
    glfwShowWindow(window);
    

    GLint loc_color = glGetUniformLocation(program, "color");

    srand((unsigned int)time(NULL));
    float random_theta = 90;
    float random_forca = 10;

    //inicia valores aleatorios de angulo e forca
    for(int j=0;j<populacao;j++){

        vetor_theta[j]= ((float)rand()/(float)(RAND_MAX)) * random_theta;
        vetor_forca_original[j]= ((float)rand()/(float)(RAND_MAX)) * random_forca;
        

    }


    //inicializar valores dos vetores
    for(int j=0; j<populacao;j++){
 
        matriz_forca_atual[j][0]= cos(vetor_theta[j]*pi/180)*vetor_forca_original[j];
        matriz_forca_atual[j][1]= sin(vetor_theta[j]*pi/180)*vetor_forca_original[j];

        matriz_tempo[j][0]=speed_time;  
        matriz_tempo[j][1]=speed_time;

        matriz_pos_inicial[j][0]= posinicialX; //-0.799995;
        matriz_pos_inicial[j][1]= posinicialY; //-0.79415;

        matriz_translado[j][0] = matriz_pos_inicial[j][0] + matriz_forca_atual[j][0]*matriz_tempo[j][0];
        matriz_translado[j][1] = matriz_pos_inicial[j][1] + matriz_forca_atual[j][1]*matriz_tempo[j][1] - g*matriz_tempo[j][1]*matriz_tempo[j][1]/2;

        subindo[j]=1;
        bolaviva[j]=1;

        matriz_quina_acerto[j][0]=0;

        vetor_cesta_acerto[j]=1000;
        vetor_cesta_acertotxt[j]=1000;
        vetor_distfinal[j]=1000;
       

    }

    //inicio matriz memoria
    for(int j=0;j<testetotal;j++){
        Mmemoria[j][0]= 0;
        Mmemoria[j][1]= 0;
        Mmemoria[j][2]= 0;
        Mmemoria[j][3]= 0;
    }

    //inicio variaveis de controle
    vetor_melhor_todos[0]=-10; 
    vetor_melhor_todos[1]=1000000;
    vetor_transldado_cesta[0] = 0;
    vetor_transldado_cesta[1] = 0;
  
   //acho posicao do meio da cesta e do meio da bola
    float cestax = (vertices[32].x + vertices[33].x) /2  + vetor_transldado_cesta[0];
    float cestay = (vertices[32].y + vertices[33].y) /2  + vetor_transldado_cesta[1];
    float bolax = (vertices[0].x + vertices[16].x)/2 ;
    float bolay = vertices[0].y ; 
    float quinax1 = vertices[32].x + vetor_transldado_cesta[0];
    float quinax2 = vertices[33].x + vetor_transldado_cesta[0];
    float quinay1 = vertices[32].y + vetor_transldado_cesta[1];
    float quinay2 = vertices[33].y + vetor_transldado_cesta[1];//na vdd quinay1 e quinay2 tem o msmo valor pq n tem cesta inclinada
    float tabelax1 =vertices[34].x + vetor_transldado_cesta[0]; //ponto de baixo da tabela
    float tabelay1 =vertices[34].y + vetor_transldado_cesta[1];
    float tabelax2 =vertices[35].x + vetor_transldado_cesta[0];//ponto de cima da tabela
    float tabelay2 =vertices[35].y + vetor_transldado_cesta[1];


    float R = 0;
    float G = 0;
    float B = 1;
    float aux;
    
   

    while (!glfwWindowShouldClose(window) && !stop ){


        tempoteste = tempoteste + 0.001; //essa variavel controla o tempo de vida das bolas pra gerar nova geracao


        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

       
        // enviando a matriz de transformacao para a GPU, vertex shader, variavel mat_transformation
        loc = glGetUniformLocation(program, "mat_transformation");

        //atualizo posicao da cesta e variaveis associadas
        cestax = (vertices[32].x + vertices[33].x) /2  + vetor_transldado_cesta[0];
        cestay = (vertices[32].y + vertices[33].y) /2  + vetor_transldado_cesta[1];
        quinax1 = vertices[32].x + vetor_transldado_cesta[0];
        quinax2 = vertices[33].x + vetor_transldado_cesta[0];
        quinay1 = vertices[32].y + vetor_transldado_cesta[1];
        quinay2 = vertices[33].y + vetor_transldado_cesta[1];//na vdd quinay1 e quinay2 tem o msmo valor pq n tem cesta inclinada
        tabelax1 =vertices[34].x + vetor_transldado_cesta[0]; //ponto de baixo da tabela
        tabelay1 =vertices[34].y + vetor_transldado_cesta[1];
        tabelax2 =vertices[35].x + vetor_transldado_cesta[0];//ponto de cima da tabela
        tabelay2 =vertices[35].y + vetor_transldado_cesta[1];
        

        for(int j = 0; j<populacao; j++){ 
  

            //atualizo o tempo
            matriz_tempo[j][0]= matriz_tempo[j][0] + speed_time;  //tempo x
            matriz_tempo[j][1]= matriz_tempo[j][1] + speed_time;  //tempo y

            //translado parcial eh o quanto ele movimenta nessa iteracao
            matriz_translado_parcial[j][0] =  matriz_pos_inicial[j][0] + matriz_forca_atual[j][0]*matriz_tempo[j][0]- matriz_translado[j][0];
            matriz_translado_parcial[j][1] = matriz_pos_inicial[j][1] + matriz_forca_atual[j][1]*matriz_tempo[j][1] - g*matriz_tempo[j][1]*matriz_tempo[j][1]/2 - matriz_translado[j][1];
          

            //atualizo as posicoes da bola e da cesta
            matriz_cesta_atual[j][0]=cestax;
            matriz_cesta_atual[j][1]=cestay;
            matriz_bola_atual[j][0]=matriz_translado[j][0];
            matriz_bola_atual[j][1]=matriz_translado[j][1];

            //calculo a distancia entre a bola e o centro da cesta
            float distanciax = fabsf(matriz_cesta_atual[j][0]-matriz_bola_atual[j][0]);
            float distanciay = fabsf(matriz_cesta_atual[j][1]-matriz_bola_atual[j][1]);
            vetor_cesta_acerto[j]= sqrt(distanciax*distanciax + distanciay*distanciay);

            //calcula o mais proximo q a bola chegou da cesta
            if(vetor_distfinal[j]>vetor_cesta_acerto[j]){
                vetor_distfinal[j]=vetor_cesta_acerto[j];
            }
           

            //atualizo melhor de todos
            //checa o valor, checa c a bola esta descendo e checa c a bola esta viva, bola viva=bola que nao passou por debaixo da cesta em nenhum momento
            if(vetor_cesta_acerto[j]<vetor_melhor_todos[1] && bolaviva[j] > 0 && subindo[j]<0){
               // if(vetor_cesta_acerto[j]<vetor_melhor_todos[1] ){
                vetor_melhor_todos[0] = j;
                vetor_melhor_todos[1] = vetor_cesta_acerto[j];
                predacao = 0;
                percent_mutacao = 0.02;
            }
           
            
            //calculo colisao com as quinas, aqui se eu mudar a pos da cesta a pos das quinas nao muda
            float distancia_quina1;
            float distancia_quina2;
           
 
            distancia_quina1 = fabsf(quinax1 - matriz_bola_atual[j][0]) * fabsf(quinax1 - matriz_bola_atual[j][0] );
            distancia_quina1 = distancia_quina1 + fabsf(quinay1 - matriz_bola_atual[j][1] )*fabsf(quinay1 - matriz_bola_atual[j][1] );
            distancia_quina1 = sqrt(distancia_quina1);

            distancia_quina2 = fabsf(quinax2 - matriz_bola_atual[j][0] ) * fabsf(quinax2 - matriz_bola_atual[j][0] );
            distancia_quina2 = distancia_quina2 + fabsf(quinay2 - matriz_bola_atual[j][1] )*fabsf(quinay2 - matriz_bola_atual[j][1] );
            distancia_quina2 = sqrt(distancia_quina2);



            // checo colisao com quina quina 1 da direita e 2 eh da esquerda
            if(distancia_quina1 < radius){

                if(matriz_quina_acerto[j][0]==1){
                    matriz_quina_acerto[j][0]=11; //isso eh para c ja houve um quique ele nao resete e fique quicando antes da bola "sair" da colisao infinitamente
                }
                if(matriz_quina_acerto[j][0]==0){
                    matriz_quina_acerto[j][0]= 1;
                    
                   
                }
                
            }else{

                if(distancia_quina2 < radius){
                    if(matriz_quina_acerto[j][0]==2){
                    matriz_quina_acerto[j][0]=22;
                    }
                    if(matriz_quina_acerto[j][0]==0){
                        matriz_quina_acerto[j][0]= 2;
                    }

                    

                }else{

                    matriz_quina_acerto[j][0]= 0;

                }


            }
           
            //caso haja colisao ponho o quique da bola
            if(distancia_quina1  < radius && matriz_quina_acerto[j][0] == 1){

                if(matriz_bola_atual[j][1] + matriz_translado_parcial[j][1]>quinay1){ // se a bola ta pra cima da quina entao o quique eh pra cima

                    if(matriz_bola_atual[j][0] + matriz_translado_parcial[j][0]>quinax1){ // se a bola ta pra esquerda da quina entao o quique eh pra esquerda caso contrario pra direita
                        matriz_forca_atual[j][0] = 0.7*matriz_forca_atual[j][0];
                    }else{
                        matriz_forca_atual[j][0] = -0.7*matriz_forca_atual[j][0];
                    }

                    matriz_forca_atual[j][1]= 0.7*matriz_forca_atual[j][1];
                    matriz_tempo[j][0]=speed_time;
                    matriz_tempo[j][1]=speed_time;
                    matriz_pos_inicial[j][0]= matriz_translado[j][0];
                    matriz_pos_inicial[j][1]= matriz_translado[j][1];


                }

                if(matriz_bola_atual[j][1] + matriz_translado_parcial[j][1]<quinay1){ // se a bola ta pra baixo da quina entao o quique eh pra baixo

                    if(matriz_bola_atual[j][0] + matriz_translado_parcial[j][0]>quinax1){ // mesma analise acima
                        matriz_forca_atual[j][0] = 0.7*matriz_forca_atual[j][0];
                    }else{
                        matriz_forca_atual[j][0] = -0.7*matriz_forca_atual[j][0];
                    }

                    matriz_forca_atual[j][1]= -0.7*matriz_forca_atual[j][1];
                    matriz_tempo[j][0]=speed_time;
                    matriz_tempo[j][1]=speed_time;
                    matriz_pos_inicial[j][0]= matriz_translado[j][0];
                    matriz_pos_inicial[j][1]= matriz_translado[j][1];


                }

            }


            //repito pra quina 2
            if(distancia_quina2 < radius && matriz_quina_acerto[j][0] == 2){

                if(matriz_bola_atual[j][1]>quinay1){ // se a bola ta pra cima da quina entao o quique eh pra cima

                    if(matriz_bola_atual[j][0]>quinax1){ // se a bola ta pra esquerda da quina entao o quique eh pra esquerda caso contrario pra direita
                        matriz_forca_atual[j][0] = 0.7*matriz_forca_atual[j][0];
                    }else{
                        matriz_forca_atual[j][0] = -0.7*matriz_forca_atual[j][0];
                    }

                    matriz_forca_atual[j][1]= 0.7*matriz_forca_atual[j][1];
                    matriz_tempo[j][0]=speed_time;
                    matriz_tempo[j][1]=speed_time;
                    matriz_pos_inicial[j][0]= matriz_translado[j][0];
                    matriz_pos_inicial[j][1]= matriz_translado[j][1];


                }

                if(matriz_bola_atual[j][1]<quinay1){ // se a bola ta pra baixo da quina entao o quique eh pra baixo

                    if(matriz_bola_atual[j][0]>quinax1){ // mesma analise acima
                        matriz_forca_atual[j][0] = 0.7*matriz_forca_atual[j][0];
                    }else{
                        matriz_forca_atual[j][0] = -0.7*matriz_forca_atual[j][0];
                    }

                    matriz_forca_atual[j][1]= -0.7*matriz_forca_atual[j][1];
                    matriz_tempo[j][0]=speed_time;
                    matriz_tempo[j][1]=speed_time;
                    matriz_pos_inicial[j][0]= matriz_translado[j][0];
                    matriz_pos_inicial[j][1]= matriz_translado[j][1];

                }

            }


            //colisao com tabela
            //vejo se ta na altura da tabela
            if((matriz_bola_atual[j][1] + matriz_translado_parcial[j][1] > tabelay1) && (matriz_bola_atual[j][1] + matriz_translado_parcial[j][1] < tabelay2)){


                if(matriz_bola_atual[j][0] + matriz_translado_parcial[j][0] + radius > tabelax1 && matriz_bola_atual[j][0] + matriz_translado_parcial[j][0] - radius  < tabelax1){

                    matriz_forca_atual[j][0] = -0.7*matriz_forca_atual[j][0];
                    if(subindo[j]<0){
                         matriz_forca_atual[j][1]= -0.7*matriz_forca_atual[j][1];
                    }else{
                        matriz_forca_atual[j][1]= 0.7*matriz_forca_atual[j][1];
                    }
                    
                    matriz_tempo[j][0]=speed_time;
                    matriz_tempo[j][1]=speed_time;
                    matriz_pos_inicial[j][0]= matriz_translado[j][0];
                    matriz_pos_inicial[j][1]= matriz_translado[j][1];


                }

            }




            //acrescento o movimento em x e y e o possivel quique com o chao
            if(matriz_bola_atual[j][0] - radius < 1.0){

                if(matriz_bola_atual[j][1] - radius < -1.0){ //quique
                   
                   if(matriz_forca_atual[j][1]<0){
                    matriz_forca_atual[j][1] = -0.7*matriz_forca_atual[j][1];//se a forca ta negativa eu reseto e coloco ela positiva
                    }else{
                     matriz_forca_atual[j][1] = 0.7*matriz_forca_atual[j][1];
                    }
                    
                    matriz_tempo[j][1]=speed_time;
                    matriz_pos_inicial[j][1]= matriz_translado[j][1];
                }

                if(fabsf(matriz_forca_atual[j][1]) > 0.005){ // se a força for grande o suficiente ele movimenta em y
                    
                    aux = matriz_translado[j][1];
                    matriz_translado[j][1] = matriz_pos_inicial[j][1] + matriz_forca_atual[j][1]*matriz_tempo[j][1] - g*matriz_tempo[j][1]*matriz_tempo[j][1]/2;

                    subindo[j] = matriz_translado[j][1]- aux;// posicao menos a posicao anterior se der negativo ele ta descendo
                                                            // guardar essa info para nao aceitar cestas de baixo pra cima

                    //se a bola ta subindo e ta embaixo do aro ela eh invalidada
                    if(subindo[j]>0 && (matriz_translado[j][0]>quinax1 && matriz_translado[j][0]<quinax2) && matriz_translado[j][1]<quinay1){
                        bolaviva[j]=-1;
                    }
                        
                    

                }

                //movimento em x
                matriz_translado[j][0] = matriz_pos_inicial[j][0] + matriz_forca_atual[j][0]*matriz_tempo[j][0];


            }

           

            //atualizo matriz que vai por o movimento nas bolas
            for(int j = 0; j<populacao; j++){
                float mat_translation_ball[16] = {
                    1.0f, 0.0f, 0.0f, matriz_translado[j][0] ,
                    0.0f, 1.0f, 0.0f, matriz_translado[j][1] ,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f
                };
 

            if(vetor_melhor_todos[0]==j){
                //pinto a melhor de todos de azul
                R=0.00;
                G=0.30;
                B=1.00;

             
            }else{

                if(ind_memoria == j){
                    //pinto a que sofreu predacao por memoria de verde
                    R=0.3;
                    G=1.0;
                    B=0.0;

                  
                }else{

                    //as demais pinto de laranja
                    R=1.0;
                    G=0.614;
                    B=0.0;
                }
               
            }
            
            
            //renderizando, printando na tela
            glUniform4f(loc_color, R, G, B, 1.0); 
            glUniformMatrix4fv(loc, 1, GL_TRUE, mat_translation_ball);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 32);
            

            }


        }

            
       
        // criando a matriz de translacao da cesta 
        float mat_translation_basket[16] = {
            1.0f, 0.0f, 0.0f,  vetor_transldado_cesta[0] ,
            0.0f, 1.0f, 0.0f,  vetor_transldado_cesta[1] ,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        //cor da cesta
        R=1;
        G=1;
        B=1;
        //desenha cesta
        glUniformMatrix4fv(loc, 1, GL_TRUE, mat_translation_basket);
        glUniform4f(loc_color, R, G, B, 1.0); 
        glDrawArrays(GL_LINES, 32, 2);

        //cor da tabela
        R=1;
        G=0;
        B=0;
        //desenha tabela
        glUniformMatrix4fv(loc, 1, GL_TRUE, mat_translation_basket);
        glUniform4f(loc_color, R, G, B, 1.0); 
        glDrawArrays(GL_LINES, 34, 2);

        //cor da rede
        R=0.502;
        G=0.502;
        B=0.502;
         //desenha rede
        glUniformMatrix4fv(loc, 1, GL_TRUE, mat_translation_basket);
        glUniform4f(loc_color, R, G, B, 1.0); 
        glDrawArrays(GL_LINES, 36, 16);

        glfwSwapBuffers(window);
       


        //reseta o lancamento sozinho
        if(tempoteste > 0.2){
        
            resetapop();

        }
  
 

        //muda a cesta de lugar caso ja tenha sido cesta
        if( vetor_melhor_todos[1]<0.026){ 

            //guardo os valroes da cesta e angulo e forca para acertar essa cesta na matriz memoria para fazer a rpedacao memoria
            Mmemoria[treinos][0] = vetor_forca_original[(int)vetor_melhor_todos[0]];
            Mmemoria[treinos][1] = vetor_theta[(int)vetor_melhor_todos[0]];
            Mmemoria[treinos][2] = vetor_transldado_cesta[0];
            Mmemoria[treinos][3] = vetor_transldado_cesta[1];

           

            float random = 1.6;
            //gera um nmero aleatorio entre 0 e rand max, divide por rand max, nmero entre 0 e 1
            //vezes o 1.6 (nmero entre 0 e 1.6), -0.8 nmero entre -0.8 e 0.8
            vetor_transldado_cesta[0]= (((float)rand()/(float)(RAND_MAX)) * random) - 0.8; 
            vetor_transldado_cesta[1]= (((float)rand()/(float)(RAND_MAX)) * random) - 0.8; 

           //reseto variaveis de controle
            tempoteste=0;
            vetor_melhor_todos[0]=-10; // com esse valor no genocidio ele vai resetar o melhor de todos tambem, que eh oq queremos nesse genocidio
            vetor_melhor_todos[1]=1000000;
            genocidio();
            pred_memoria();
            predacao = 0;

            for(int j=0; j<populacao;j++){

                matriz_forca_atual[j][0]= cos(vetor_theta[j]*pi/180)*vetor_forca_original[j];
                matriz_forca_atual[j][1]= sin(vetor_theta[j]*pi/180)*vetor_forca_original[j];

                matriz_tempo[j][0]=speed_time;
                matriz_tempo[j][1]=speed_time;

                matriz_pos_inicial[j][0]= posinicialX; 
                matriz_pos_inicial[j][1]= posinicialY;

                matriz_translado[j][0] = matriz_pos_inicial[j][0] + matriz_forca_atual[j][0]*matriz_tempo[j][0];
                matriz_translado[j][1] = matriz_pos_inicial[j][1] + matriz_forca_atual[j][1]*matriz_tempo[j][1] - g*matriz_tempo[j][1]*matriz_tempo[j][1]/2;

                subindo[j]=1;
                bolaviva[j]=1;

                matriz_quina_acerto[j][0]=0;
                vetor_distfinal[j]=1000;
      

            }

            //treinos controla quantas cestas o codigo deve acertar antes de fechar
            treinos += 1;

            if(treinos > testetotal){
                stop = 1;
            }

               

        }


    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);

}