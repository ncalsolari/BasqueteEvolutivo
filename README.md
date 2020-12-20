# BasqueteEvolutivo

<h3>SSC0719 - Sistemas Evolutivos Aplicados à robótica (2020)</h3>
<h3>Nelson Calsolari Neto</h3>
<p>Prof. Assoc. Eduardo do Valle Simões</p>

O código se trata de um algoritmo evolutivo que aprende a arremessar bolas de basquete em cestas que variam de posição.
Neste algoritmo foi implementado alguns atributos referentes à algoritmos evolutivos os quais serão descritos mais abaixo.
Temos 4 elementos diferentes:

Cesta <br>
Individuo padrão da populaço (bola laranja)<br>
Individuo melhor de todos (bola azul)<br>
Individuo predado por memória (bola verde)<br>

<hr>

<h3>Descrição:</h3>
<p>Cada indivíduo possui 2 genes, força (varia de 0 à 10) e theta (angulo, varia de 0 à 90). A força se refere a grandeza vetorial, então o movimento da bola no eixo x é descrito pelo cos(theta)*força e em y por sen(theta)*força. Para o movimento é utiliuzado as fórmulas que descrevem o lançamento oblíquo.</p>

<p>A população inicia com todos os indivíduos recebendo valores aleatórios de força e theta. Aqueles individuos que chegarem mais próximos do centro da cesta são taxados como "melhor de todos", os indivíduos só são avaliados enquanto fazem um movimento de "descida" e enquanto não passam por debaixo da cesta (isto evita cestas feitas de baixo pra cima, que é uma jogada não válida no basquete.)</p>

<p>Quando termina o tempo de vida da populção acontece o crossover, os genes dos individuos (menos do "melhor de todos") são substituídos pela média aritmética entre seus genes e o gene do "melhor de todos" (método Elitismo). Depois cada individuo sofre uma "mutaçao", uma variação de UM dos genes ( a escolha de qual gene será mutado e se positivamente ou negativamente é aleatório), esta variação é inicialmente em uma taxa de 2%. Assim a população é reiniciada, agora com seus novos valores de gene, este processo configura uma geração </p>

<p>Caso passe 4 gerações e o indivíduo "melhor de todos" permaneça o mesmo a taxa de mutação sofre um acréscimo de 2.8% por geração até achar um novo "melhor de todos" (nesse caso ela volta a ser 2%) ou até alcançar 15 gerações sem alterar o "melhor de todos". Neste caso a taxa volta a ser 2% e é feito o genocídio, toda a populaçao (com excessão ao "melhor de todos") tem seus genes reiniciados com valores aleatórios.  </p>

<p>Se a distância entre o "melhor de todos" e a cesta alcançar um valor mínimo estipulado (no caso pegou-se um valor menor que o raio da bola garantindo assim que a mesma passou pelo meio da cesta) é contabilizado um acerto. Então os dados da posição da cesta e os valores do "melhor de todos" são salvos (no que chamarei de "memória"), a cesta assume uma nova posição e a população é resetada recebendo valores de genes aleatórios, neste caso reseta-se o "melhor de todos" também </p>

<p>A cada geração, paralelo ao crossover acontece uma predação particular que chamei de "predação por memória", o pior indivíduo da população (o que chegou menos perto da cesta) tem seus genes substítuidos pelos genes da "memória". A "memória" se trata das cestas anteriormente dadas como acertadas. Ou seja, a cada nova posição de cesta, é feita uma busca nas cestas já validadas, é escolhida aquela que estava mais próxima da cesta atual, e os valores dos genes do "melhor de todos" que uma vez acertou esta cesta passada é copiado para este pior indivíduo.</p>
<p>Trata-se de uma adaptação de um método de machine learning chamado KNN, porém aqui minha "base de dados" (a "memória") começa sem nenhum dado armazenado, a medida que as cestas vão sendo validadas mais cestas eu tenho na "memória", portanto maior minha variabilidade de posiçoes que eu "ja sei acertar a bola". Isso faz com que a população convirja muito mais rapidamente para cestas válidas quanto mais cestas válidas eu já tenho salvo.</p>

<p>Apesar de inicialmente esta predação se mostrar quase inútil, pois no começo não tenho muitos dados na "memória" ele acaba sendo muito eficaz a medida que o algoritmo vai se desenvolvendo, pois cestas em posiçoes semelhantes exigem genes semelhantes nas bolas para se ter o acerto. Foi feito testes usando outras predações, utilizando a predação por síntese (pior indivíduo recebe o valor da média do resto da população) demorou cerca de 2000 gerações para ter 100 cestas validadas, com a predação por memória teve-se uma média de 400 gerações para se ter 100 cestas validadas. Uma otimização de 5x.</p>

<hr>

#### Execução
Em linux, instalar os pacotes: libglfw3-dev mesa-common-dev libglew-dev<br>
Para compilar no linux: gcc algevol_basq.c -lglfw -lGL -lGLEW -lm -o algevol_basq.bin<br>
Para executar: ./algevol_basq.bin<br>

<br>

#### Versão do código
1.0

<br>

#### Observações
Versões alternativas do código, como por exemplo a versão com predação por síntese, e a versão que salva os valores das cestas em um txt para plotagem do gráfico e análise da evolução do algoritmo estão em no seguinte repósitorio: <br>
https://github.com/ncalsolari/AlgEvolutivo-SSC0713

<br>
<br>

![basqueteevol1](https://user-images.githubusercontent.com/26654523/102576693-fdc93b00-40d4-11eb-8b53-d065c9a780be.png)



