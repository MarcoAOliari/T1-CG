#ifndef CONFIGURACOES_JOGO_H
#define CONFIGURACOES_JOGO_H

#include <vector>

#include "SVGReader.h"
#include "Circle.h"
#include "Character.h"
#include "Rect.h"
#include "Obstacle.h"

using std::vector;

class ConfiguracoesJogo {
    Character player;
    vector<Circle> inimigos;
    vector<Obstacle> obstaculos;

    public:
        ConfiguracoesJogo(const char *pFilename);
        void Desenha();
};

#endif /* CONFIGURACOES_JOGO_H */