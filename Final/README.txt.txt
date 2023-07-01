Apresentação: https://www.youtube.com/watch?v=1R57tW24StU
Aluno: Pedro Evaldt Machado.

Decisões:
- Criado classes para tratativar de objetos e estruturas da cena (nao sei codificar em c++, entao pode ter mt coisa fora do padrão).
- Os movimentos/curvas estão sendo feitos por pontos gerados aleatóriamente no espaço, achei mais simples implementar dessa maneira.
- A leitura dos serão todos os arquivos .obj de uma pasta lida (padrão do projeto ./objetos/).
- Vertex e fragment shader também são lidos de uma pasta (padrão é ./shaders/).
- As instruções e escolhas são definidas e lidas no console da aplicação.

Estrutura do arquivo de cenas:

obj desk.obj
i -0.00509968307 0.00000000 -0.0998698547 0
i 0.00000000 0.100000001 0.00000000 0
i 0.0998698547 0.00000000 -0.00509968307 0
i -0.524951398 -0.587261200 0.0947597623 1
ma 0

sendo:
obj [nome do arquivo]
i [valores da matriz 4x4]
ma [movimento automático (0 = não, 1 = sim) / curva]