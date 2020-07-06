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
const bool debug = false;

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


class Line{
public:
    struct Point { double x, y, xn, yn, a; } * zln;
    struct Mouse { sf::Vector2i lastpos, pos; } mouse;
    struct Settings { int n, thickness, htn, min_distance; } settings;
    sf::VertexArray skin;
    
    Line(int n, int thickness, int mindist, sf::Vector2i mouse_pos){
        settings.n = n;
        settings.thickness = thickness;
        settings.htn = thickness/2;
        settings.min_distance = mindist;
        mouse.lastpos = mouse_pos;
        zln = (Point *)malloc(n * sizeof(Point));
        skin.setPrimitiveType(sf::TriangleStrip);
        skin.resize(n*2);
    }
    
    void reinit(int new_n, int new_thickness, int new_mindist){
        settings.n = new_n;
        settings.thickness = new_thickness;
        settings.htn = new_thickness/2;
        settings.min_distance = new_mindist;
        zln = (Point *)realloc(zln, new_n * sizeof(Point));
        skin.resize(new_n*2);
    }
    
    void update_line(sf::Vector2i mouse_new_pos){
        //pos = sf::Mouse::getPosition(window);
        mouse.pos = mouse_new_pos;
        if (dist(mouse.lastpos, mouse.pos) >= settings.min_distance){
            for (int i = settings.n - 1; i > 0; i--){
                zln[i] = zln[i - 1];
            }
            zln[0].x = mouse.pos.x;
            zln[0].y = mouse.pos.y;
            mouse.lastpos = mouse.pos;
            
            //zln[1]
            double x1 = zln[1].x, y1 = zln[1].y;
            double xm = x1 - zln[0].x, ym = y1 - zln[0].y;
            double xp = x1 - zln[2].x, yp = y1 - zln[2].y;
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
                zln[1].xn = sgn * xn;
                zln[1].yn = sgn * yn;
                zln[1].a  = alpha;
            } else {
                xn = xm - xp;
                yn = ym - yp;
                double ln = sqrt(xn*xn+yn*yn);
                zln[1].xn =  yn / ln;
                zln[1].yn = -xn / ln;
                zln[1].a  = M_PI / 2;
            }
            //zln[0]
            xp = zln[0].x - zln[1].x;
            yp = zln[0].y - zln[1].y;
            lp = sqrt(xp*xp+yp*yp);
            zln[0].xn = -yp / lp;
            zln[0].yn =  xp / lp;
            zln[0].a  = M_PI / 2;
            //zln[n - 1]
            xm = zln[settings.n - 1].x - zln[settings.n - 2].x;
            ym = zln[settings.n - 1].y - zln[settings.n - 2].y;
            lm = sqrt(xm*xm+ym*ym);
            zln[settings.n - 1].xn =  ym / lm;
            zln[settings.n - 1].yn = -xm / lm;
            zln[settings.n - 1].a  = M_PI / 2;
            
            for (int i = 0; i < settings.n; i++){
                int wx = int(zln[i].x), wy = int(zln[i].y);
                //int wxn = zln[i].xn * settings.htn, wyn = zln[i].yn * settings.htn;
                skin[i*2].color = sf::Color::Magenta;
                skin[i*2].position = sf::Vector2f(wx - zln[i].xn * settings.htn, wy - zln[i].yn * settings.htn);
                skin[i*2 + 1].position = sf::Vector2f(wx + zln[i].xn * settings.htn, wy + zln[i].yn * settings.htn);
            }
        }
    }
    
    
    

    
    void sin_line(sf::VertexArray *line, int n){
        int scale = 120, thickness = 30, htn = thickness/2;
        double xl = -5.0, xr = 5.0, step = (xr - xl)/n;
        //sf::VertexArray line(sf::TriangleStrip, n*2);
        Point zln[n];
        for (int i = 0; i < n; i++){
            zln[i].x = xl + i*step;
            zln[i].y = sin(zln[i].x);
        }
        
        for (int i = 1; i < n - 1; i++){
            double xi = zln[i].x, yi = zln[i].y;
            double xm = xi - zln[i - 1].x, ym = yi - zln[i - 1].y;
            double xp = xi - zln[i + 1].x, yp = yi - zln[i + 1].y;
            double lm = sqrt(xm*xm+ym*ym), lp = sqrt(xp*xp+yp*yp);
            double xn = xm / lm + xp / lp, yn = ym / lm + yp / lp;
            double ln = sqrt(xn*xn+yn*yn);
            zln[i].xn = dabs(xn / ln);
            zln[i].yn = dabs(yn / ln);
        }
        zln[0].xn = 0;
        zln[0].yn = 1;
        zln[n - 1].xn = 0;
        zln[n - 1].yn = 1;
        
        for (int i = 0; i < n; i++){
            int wx = int(xcenter + zln[i].x*scale), wy = int(ycenter + zln[i].y*scale);
            (*line)[i*2].position = sf::Vector2f(wx - zln[i].xn * htn, wy - zln[i].yn * htn);
            (*line)[i*2 + 1].position = sf::Vector2f(wx + zln[i].xn * htn, wy + zln[i].yn * htn);
        }
    }
    
    ~Line(){
        free(zln);
    }
};


class Dedug{
public:
};



sf::Text text_init(){
    sf::Text text;
    sf::Font font;
    std::string font_path = "/Users/gleb/Projects/C++/gSFML/Inconsolata/inconsolata.ttf";
    if (!font.loadFromFile(font_path)){
        std::cout << "\nLOAD FONT ERROR\n\n";
        //return EXIT_FAILURE;
    }
    text.setFont(font);
    text.setCharacterSize(36);
    text.setFillColor(sf::Color::Green);
    return text;
}


int main(){
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(width, height), "Thick Line", sf::Style::Default, settings);
    int iteration = 0;
    Line line(20, 24, 20, sf::Mouse::getPosition(window));
    sf::Vertex axes[] = {
        sf::Vertex(sf::Vector2f(0, ycenter)), sf::Vertex(sf::Vector2f(width, ycenter)),
        sf::Vertex(sf::Vector2f(xcenter, 0)), sf::Vertex(sf::Vector2f(xcenter, height)),
        sf::Vertex(sf::Vector2f(0, height)), sf::Vertex(sf::Vector2f(height, 0))
    };
    sf::Text text = text_init();
    
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
        line.update_line(sf::Mouse::getPosition(window));
        
        if (debug){
            std::string str = "iteration " + std::to_string(iteration) + "\n";
            char buf[100];
            for (int i = 0; i < line.settings.n; i++){
                int res = snprintf(buf, sizeof(buf), \
                                   "zln[%02.d] x = %4.d y = %4.d xn = %6.3f yn = %6.3f a = %6.3f\n", \
                                   i, int(line.zln[i].x), int(line.zln[i].y), line.zln[i].xn, \
                                   line.zln[i].yn, line.zln[i].a);
                if (res >= 0 && res < sizeof(buf)) { str += buf; }
                else { str = "format error"; break; }
            }
            text.setString(str);
            window.draw(text);
        }
            
        window.draw(axes, 6, sf::Lines);
        window.draw(line.skin);
        window.display();
    }
    return 0;
}
