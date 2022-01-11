#include "ConfiguracoesJogo.h"
#include "SVGReader.h"

#include <iostream>

using namespace std;

ConfiguracoesJogo::ConfiguracoesJogo(const char *pFilename) {
    SVGReader svg(pFilename);

    vector<Circle> circles = svg.getCircles();
    vector<Rect> rects = svg.getRects();

    float larguraTotal, arenaX, arenaY, larguraArena;
    Rect arena;

    for (Rect r : rects) {
        if (r.getIsArena()) {
            larguraTotal = r.width * 500 / r.height;
            this->limiteArena = larguraTotal;
            arena = r;
            break;
        }
    }

    for (Circle c : circles) {
        if (c.getIsPlayer()) {
            this->player = new Character(c, arena, larguraTotal);
        } else {
            this->inimigos.push_back(new Character (c, arena, larguraTotal));
        }
    }

    for (Rect r : rects) {
        if (!r.getIsArena()) {
            Obstacle o(r, arena, larguraTotal);
            this->obstaculos.push_back(o);
        }
    }

    Obstacle chao(larguraTotal);
    this->chao = chao;

}

void ConfiguracoesJogo::PlataformaInimigos(GLfloat dy) {
    bool colidiu = false;
    for (Character* c : this->inimigos) {
        this->CaiInimigo(c, dy);
    }
}

// puxar deltaT
void ConfiguracoesJogo::CaiInimigo(Character* c, GLfloat dy) {
    bool plataforma = false;

    while (!plataforma) {
        for (Obstacle o : this->obstaculos) {
            // refatorar também
            if (c->ColisaoChao(o, 16)) {
                plataforma = true;
                this->plataformaInimigos.push_back(o);
                break;
            } else if (c->getYPe() >= 500 - dy) {
                plataforma = true;
                this->plataformaInimigos.push_back(this->chao);
                break;
            }
        }
        // refatorar
        c->Cai(16);
    }

}

void ConfiguracoesJogo::Desenha() {
    this->player->Desenha();

    for (Obstacle o : this->obstaculos) {
        o.Desenha();
    }

    for (Character* c : this->inimigos) {
        c->Desenha();
    }

    for (Tiro* t : this->tiros) {
        t->DesenhaTiro();
    }

}

bool ConfiguracoesJogo::ColisaoCharacterObstaculo(Character* c, GLfloat dx, GLfloat dy, GLdouble deltaT) {
    for (Obstacle o : this->obstaculos) {
        if (c->ColisaoObstacle(o, dx, dy, deltaT)){ 
            return true;
        }
    }
    return false;
}

bool ConfiguracoesJogo::ColisaoCharacterCharacter(Character* c, GLfloat dx, GLfloat dy, GLdouble deltaT) {
    for (Character* c2 : this->inimigos) {
        if (c->ColisaoCharacter(c2, dx, dy, deltaT)) {
            return true;
        }
    }

    if (c->ColisaoCharacter(this->player, dx, dy, deltaT)) {
        return true;
    }

    return false;
}

bool ConfiguracoesJogo::ColisaoTiro(Tiro* t) {
    for (Obstacle o : this->obstaculos) {
        if (t->ColisaoObstaculo(o)){
            return true;
        }
    }

    vector<Character*>::iterator it;

    for (it = this->inimigos.begin(); it != this->inimigos.end(); ++it) {
        if ((*it)->ColisaoTiro(t)){
            it = this->inimigos.erase(it);
            return true;
        } 
    }

    if (this->player->ColisaoTiro(t)) {
        return true;
    }

    return false;
}

bool ConfiguracoesJogo::ColisaoCabeca(Character* c, GLdouble deltaT) {
    for (Character* c2 : this->inimigos) {
        if(c->ColisaoCabeca(c2, deltaT)) {
            return true;
        }
    }

    if (c->ColisaoCabeca(this->player, deltaT)) {
        return true;
    }

    return false;
}

bool ConfiguracoesJogo::ColisaoChao(Character* c, GLdouble deltaT) {
    for (Obstacle o : this->obstaculos) {
        if (c->ColisaoChao(o, deltaT)){ 
            return true;
        }
    }
    return false;
}

bool ConfiguracoesJogo::ColisaoTeto(Character* c, GLdouble deltaT) {
    for (Obstacle o : this->obstaculos) {
        if (c->ColisaoTeto(o, deltaT)){ 
            return true;
        }
    }
    return false;
}

void ConfiguracoesJogo::AndaPlayer(GLfloat dx, GLdouble deltaT, char direcao) {
    if (!this->ColisaoCharacterObstaculo(this->player, dx, 0, deltaT) && !this->ColisaoCharacterCharacter(this->player, dx, 0, deltaT))
        this->player->Anda(dx, deltaT, true, direcao);
}

void ConfiguracoesJogo::ParaDeAndarPlayer() {
    this->player->ParaDeAndar();
}

void ConfiguracoesJogo::PulaPlayer(GLdouble deltaT) {
    if (!this->ColisaoTeto(this->player, deltaT) && !this->player->getPlayerCaindo()) {
        this->player->Pula(deltaT);
    } else {
        this->CaiPlayer(deltaT);
    }
}

void ConfiguracoesJogo::CaiPlayer(GLdouble deltaT) {
    if (!this->ColisaoChao(this->player, deltaT) && !this->ColisaoCabeca(this->player, deltaT)) {
        this->player->Cai(deltaT);
    } else {
        this->player->EstadoInicialY();
    }
}

GLfloat ConfiguracoesJogo::getCentroCamera() {
    return this->player->getCentroCamera();
}

void ConfiguracoesJogo::MoveBracoPlayer(GLfloat x, GLfloat y) {
    this->player->MoveBraco(x, y);
}

void ConfiguracoesJogo::AtiraPlayer(GLfloat velocidadeTiro, GLdouble deltaT) {
    this->tiros.push_back(this->player->CriaTiro(velocidadeTiro));
}

// dar delete no tiro
void ConfiguracoesJogo::MoveTiros(GLdouble deltaT) {
    vector<Tiro*>::iterator it;

    for (it = this->tiros.begin(); it != this->tiros.end();) {

        if ((*it)->Valido(this->limiteArena) && !this->ColisaoTiro((*it))){
            (*it)->Move(deltaT);
            ++it;
        } else {
            it = this->tiros.erase(it);
        }
    }
}

// void ConfiguracoesJogo::AndaInimigo(Character* c, GLfloat dx, GLdouble deltaT) {
//     if (ColisaoCharacterObstaculo(c, dx, 0, deltaT)) {
//         c->alteraDirecao();
//     }

//     char dir = c->getDirecao();

//     if (dir == 'd') {
//         c->Anda(dx, deltaT, false, dir);
//     } else if (dir == 'e') {
//         c->Anda(-dx, deltaT, false, dir);
//     }

// }

// void ConfiguracoesJogo::MoveInimigos(GLdouble deltaT, GLfloat dx) {
//     for (int i = 0; i < this->inimigos.size(); i++) {
//         AndaInimigo(&this->inimigos[i], dx, deltaT);
//     }
// }
