//
//  main.cpp
//  sfml1
//
//  Created by  Gleb on 02.07.2020.
//  Copyright © 2020  Gleb. All rights reserved.
//

/* start
 https://www.youtube.com/watch?v=IghUVIL6okk&list=PLskEAOuS4gQMoAE68-71KRxQegr8rSw54
 https://www.youtube.com/watch?v=mtEiyDbYMxQ
 https://stackoverflow.com/questions/43119058/xcode-c-and-sfml-library-issues
 thick line
 https://www.sfml-dev.org/tutorials/2.0/graphics-shape.php
 https://forum.libcinder.org/topic/smooth-thick-lines-using-geometry-shader#23286000001269127
 http://codeflow.org/entries/2012/aug/05/webgl-rendering-of-solid-trails/
 https://discourse.libcinder.org/t/smooth-efficient-perfect-curves/925
 https://www.sfml-dev.org/tutorials/2.5/graphics-vertex-array.php
 
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <unistd.h>
#include <cmath>


struct point { double x, y, xn, yn, a; };
double dabs(double x){ return x > 0 ? x : - x; }
double dist(point a, point b){ return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)); }
double dist(sf::Vector2i a, sf::Vector2i b){ return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)); }
const int height = 1400, width = 2100;
const int xcenter = width/2, ycenter = height/2;
const bool debug = false;
//const int n = 15;

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

void sin_line(sf::VertexArray *line, int n){
    int scale = 120, thickness = 30, htn = thickness/2;
    double xl = -5.0, xr = 5.0, step = (xr - xl)/n;
    //sf::VertexArray line(sf::TriangleStrip, n*2);
    point zln[n];
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



int main(){
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(width, height), "Thick Line", sf::Style::Default, settings);
    int n = 50, thickness = 24, htn = thickness/2, mindist = 20, iteration = 0;
    point zln[n];
    sf::VertexArray line(sf::TriangleStrip, n*2);
    //sf::VertexArray line(sf::Triangles, (n - 1)*6);
    sf::Vector2i pos = sf::Mouse::getPosition(window), nextpos;
    sf::Vertex axes[] = {
        sf::Vertex(sf::Vector2f(0, ycenter)), sf::Vertex(sf::Vector2f(width, ycenter)),
        sf::Vertex(sf::Vector2f(xcenter, 0)), sf::Vertex(sf::Vector2f(xcenter, height)),
        sf::Vertex(sf::Vector2f(0, height)), sf::Vertex(sf::Vector2f(height, 0))
    };
    
    sf::Text text;
    sf::Font font;
    std::string font_path = "/Users/gleb/Projects/C++/SFML/Inconsolata/inconsolata.ttf";
    if (!font.loadFromFile(font_path)){ return EXIT_FAILURE; }
    text.setFont(font);
    text.setCharacterSize(36);
    text.setFillColor(sf::Color::Green);
    
    
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
        
        nextpos = sf::Mouse::getPosition(window);
        if (dist(pos, nextpos) >= mindist){
            for (int i = n - 1; i > 0; i--){
                zln[i] = zln[i - 1];
            }
            zln[0].x = nextpos.x;
            zln[0].y = nextpos.y;
            pos = nextpos;
            
            /*for (int i = 1; i < n - 1; i++){
                double xi = zln[i].x, yi = zln[i].y;
                double xm = xi - zln[i - 1].x, ym = yi - zln[i - 1].y;
                double xp = xi - zln[i + 1].x, yp = yi - zln[i + 1].y;
                //printf("i = %d xm = %.2f, ym = %.2f, xp = %.2f, yp = %.2f\n", i, xm, ym, xp, yp);
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
                    zln[i].xn = sgn * xn;
                    zln[i].yn = sgn * yn;
                    zln[i].a  = alpha;
                } else {
                    xn = xm - xp;
                    yn = ym - yp;
                    double ln = sqrt(xn*xn+yn*yn);
                    zln[i].xn =  yn / ln;
                    zln[i].yn = -xn / ln;
                    zln[i].a  = M_PI / 2;
                }
            }*/
            
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
            xm = zln[n - 1].x - zln[n - 2].x;
            ym = zln[n - 1].y - zln[n - 2].y;
            lm = sqrt(xm*xm+ym*ym);
            zln[n - 1].xn =  ym / lm;
            zln[n - 1].yn = -xm / lm;
            zln[n - 1].a  = M_PI / 2;
            
            
            for (int i = 0; i < n; i++){
                int wx = int(zln[i].x), wy = int(zln[i].y);
                line[i*2].color = sf::Color::Magenta;
                line[i*2].position = sf::Vector2f(wx - zln[i].xn * htn, wy - zln[i].yn * htn);
                line[i*2 + 1].position = sf::Vector2f(wx + zln[i].xn * htn, wy + zln[i].yn * htn);
            }
            
            /*for (int i = 0; i < n; i++){
                int wx = int(zln[i].x), wy = int(zln[i].y);
                int wxn = int(zln[i].xn * htn), wyn = int(zln[i].yn * htn);
                line[i*6].color = sf::Color::Magenta;
                switch (i) {
                    case 0:
                        line[i*6].position = sf::Vector2f(wx + wxn, wy + wyn);
                        
                        line[i*6 + 1].position = sf::Vector2f(wx - wxn, wy - wyn);
                        line[i*6 + 5].position = sf::Vector2f(wx - wxn, wy - wyn);
                        break;
                    case n - 1:
                        line[i*6 - 3].position = sf::Vector2f(wx + wxn, wy + wyn);
                        line[i*6 - 4].position = sf::Vector2f(wx + wxn, wy + wyn);
                        
                        line[i*6 - 2].position = sf::Vector2f(wx - wxn, wy - wyn);
                        break;
                    default:
                        line[i*6].position = sf::Vector2f(wx + wxn, wy + wyn);
                        line[i*6 - 3].position = sf::Vector2f(wx + wxn, wy + wyn);
                        line[i*6 - 4].position = sf::Vector2f(wx + wxn, wy + wyn);
                        
                        line[i*6 - 2].position = sf::Vector2f(wx - wxn, wy - wyn);
                        line[i*6 + 1].position = sf::Vector2f(wx - wxn, wy - wyn);
                        line[i*6 + 5].position = sf::Vector2f(wx - wxn, wy - wyn);
                        break;
                }
                
            }*/
            
        }
        
        if (debug){
            std::string str = "iteration " + std::to_string(iteration) + "\n";
            char buf[100];
            for (int i = 0; i < n; i++){
                int res = snprintf(buf, sizeof(buf), \
                                   "zln[%02.d] x = %4.d y = %4.d xn = %6.3f yn = %6.3f a = %6.3f\n", \
                                   i, int(zln[i].x), int(zln[i].y), zln[i].xn, zln[i].yn, zln[i].a);
                if (res >= 0 && res < sizeof(buf)) { str += buf; }
                else { str = "format error"; break; }
            }
            text.setString(str);
            window.draw(text);
        }
            
        window.draw(axes, 6, sf::Lines);
        window.draw(line);
        window.display();
    }
    return 0;
}
