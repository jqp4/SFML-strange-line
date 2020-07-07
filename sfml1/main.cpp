//
//  main.cpp
//  sfml1
//
//  Created by  Gleb on 02.07.2020.
//  Copyright © 2020  Gleb. All rights reserved.
//

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>


double dabs(double x){ return x > 0 ? x : - x; }
double dist(sf::Vector2i a, sf::Vector2i b){ return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)); }
const int height = 1400, width = 2100;
const int xcenter = width/2, ycenter = height/2;
const bool debug = true;
int iteration = 0;


class Line{
private:
    double get_angel(double x, double y){
        y *= -1;
        if (x >= 0){
            if (y >= 0) return acos(x);
            else return 2 * M_PI - acos(x);
        } else {
            if (y >= 0) return M_PI - acos(-x);
            else return M_PI + acos(-x);
        }
    }
    
public:
    struct Point { double x, y, xn, yn, a; } * ghost;
    struct Mouse { sf::Vector2i lastpos, pos; } mouse;
    struct Settings { int n, thickness, htn, min_distance; } settings;
    sf::VertexArray skin;
    
    Line(int n, int thickness, int mindist, sf::Vector2i mouse_pos){
        settings.n = n;
        settings.thickness = thickness;
        settings.htn = thickness/2;
        settings.min_distance = mindist;
        mouse.lastpos = mouse_pos;
        ghost = (Point *)malloc(n * sizeof(Point));
        skin.setPrimitiveType(sf::TriangleStrip);
        skin.resize(n*2);
    }
    
    void reinit(int new_n, int new_thickness, int new_mindist){
        settings.n = new_n;
        settings.thickness = new_thickness;
        settings.htn = new_thickness/2;
        settings.min_distance = new_mindist;
        ghost = (Point *)realloc(ghost, new_n * sizeof(Point));
        skin.resize(new_n*2);
    }
    
    void update(sf::Vector2i mouse_new_pos){
        //pos = sf::Mouse::getPosition(window);
        mouse.pos = mouse_new_pos;
        if (dist(mouse.lastpos, mouse.pos) >= settings.min_distance){
            for (int i = settings.n - 1; i > 0; i--){
                ghost[i] = ghost[i - 1];
            }
            ghost[0].x = mouse.pos.x;
            ghost[0].y = mouse.pos.y;
            mouse.lastpos = mouse.pos;
            
            //zln[1]
            double x1 = ghost[1].x, y1 = ghost[1].y;
            double xm = x1 - ghost[0].x, ym = y1 - ghost[0].y;
            double xp = x1 - ghost[2].x, yp = y1 - ghost[2].y;
            double lm = sqrt(xm*xm+ym*ym), lp = sqrt(xp*xp+yp*yp);
            xm /= lm;
            ym /= lm;
            xp /= lp;
            yp /= lp;
            double xn = xm + xp, yn = ym + yp;
            if (xn != 0 && yn != 0){
                double ln = sqrt(xn*xn+yn*yn);
                xn /= ln;
                yn /= ln;
                double am = get_angel(xm, ym), an = get_angel(xn, yn);
                double alpha = an - am;
                alpha += alpha >= 0 ? 0 : 2*M_PI;
                int sgn = alpha <= M_PI ? 1 : -1;
                ghost[1].xn = sgn * xn;
                ghost[1].yn = sgn * yn;
                ghost[1].a  = alpha;
            } else {
                xn = xm - xp;
                yn = ym - yp;
                double ln = sqrt(xn*xn+yn*yn);
                ghost[1].xn =  yn / ln;
                ghost[1].yn = -xn / ln;
                ghost[1].a  = M_PI / 2;
            }
            //zln[0]
            xp = ghost[0].x - ghost[1].x;
            yp = ghost[0].y - ghost[1].y;
            lp = sqrt(xp*xp+yp*yp);
            ghost[0].xn = -yp / lp;
            ghost[0].yn =  xp / lp;
            ghost[0].a  = M_PI / 2;
            //zln[n - 1]
            xm = ghost[settings.n - 1].x - ghost[settings.n - 2].x;
            ym = ghost[settings.n - 1].y - ghost[settings.n - 2].y;
            lm = sqrt(xm*xm+ym*ym);
            ghost[settings.n - 1].xn =  ym / lm;
            ghost[settings.n - 1].yn = -xm / lm;
            ghost[settings.n - 1].a  = M_PI / 2;
            
            for (int i = 0; i < settings.n; i++){
                int wx = int(ghost[i].x);
                int wy = int(ghost[i].y);
                int wxn = ghost[i].xn * settings.htn;
                int wyn = ghost[i].yn * settings.htn;
                skin[i*2].color = sf::Color::Magenta;
                skin[i*2].position = sf::Vector2f(wx - wxn, wy - wyn);
                skin[i*2+1].position = sf::Vector2f(wx + wxn, wy + wyn);
            }
        }
    }
};


void set_text_settings(sf::Text *text, sf::Font *font, int size, sf::Color color){
    std::string font_path = "/Users/gleb/Projects/C++/SFML/Inconsolata/inconsolata.ttf";
    if (!font->loadFromFile(font_path)){
        std::cout << "\nLOAD FONT ERROR\n\n";
        //return EXIT_FAILURE;
    }
    text->setFont(*font);
    text->setCharacterSize(size);
    text->setFillColor(color);
}


std::string get_debug_info(Line line){
    std::string str = "iteration " + std::to_string(iteration) + "\n";
    char buf[100];
    for (int i = 0; i < line.settings.n; i++){
        int res = snprintf(buf, sizeof(buf), \
                           "zln[%02.d] x = %4.d y = %4.d xn = %6.3f yn = %6.3f a = %6.3f\n", \
                           i, int(line.ghost[i].x), int(line.ghost[i].y), line.ghost[i].xn, \
                           line.ghost[i].yn, line.ghost[i].a);
        if (res >= 0 && res < sizeof(buf)){
            str += buf;
        } else {
            str = "FORMAT ERROR";
            break;
        }
    }
    return str;
}


int main(){
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(width, height), "Thick Line", sf::Style::Default, settings);
    Line line(42, 20, 10, sf::Mouse::getPosition(window));
    sf::Vertex axes[] = {
        sf::Vertex(sf::Vector2f(0, ycenter)), sf::Vertex(sf::Vector2f(width, ycenter)),
        sf::Vertex(sf::Vector2f(xcenter, 0)), sf::Vertex(sf::Vector2f(xcenter, height)),
        sf::Vertex(sf::Vector2f(0, height)), sf::Vertex(sf::Vector2f(height, 0))
    };
    sf::Text text;
    sf::Font font;
    set_text_settings(&text, &font, 30, sf::Color::Green);
    
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                default:
                    break;
            }
        }
        
        iteration ++;
        window.clear();
        line.update(sf::Mouse::getPosition(window));
        
        window.draw(axes, 6, sf::Lines);
        if (debug){
            text.setString(get_debug_info(line));
            window.draw(text);
        }
        window.draw(line.skin);
        window.display();
        sf::sleep(sf::milliseconds(15));
    }
    free(line.ghost);
    return 0;
}
