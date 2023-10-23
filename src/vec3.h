#ifndef VEC_H
#define VEC_H

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3{
    public:
        double e[3];

        vec3() : e{0,0,0} {}
        vec3(double e1, double e2, double e3) : e{e1, e2, e3} {}

        double x() const {return e[0];}
        double y() const {return e[1];}
        double z() const {return e[2];}

        double operator[](int i) const {return e[i];} // for read-only access to vector components
        double& operator[](int i){return e[i];} // for write properties 
        vec3 operator-() const {return vec3(-e[0], -e[1], -e[2]);}

        vec3& operator+=(const vec3 other){
            e[0] += other.e[0];
            e[1] += other.e[1];
            e[2] += other.e[2]; 

            return *this;
        }

        vec3& operator*=(double v){
            e[0] *= v;
            e[1] *= v;
            e[2] += v;

            return *this;
        }

        vec3& operator/=(double x){
            return *this *= (1/x);
        }
        
        double length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        double length() const {
            return sqrt(length_squared());
        }

};

// define an alias for vec3 useful for making clear in the code the difference between a point and a color
using point3 = vec3;

// Vector utility functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v){
    return out << v.e[0] << " " << v.e[1] << " " << v.e[2] << " \n";
}

inline vec3 operator+(const vec3& u, const vec3& v){
    return vec3(u.e[0]+v.e[0], u.e[1]+v.e[1], u.e[2]+v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v){
    return vec3(u.e[0]-v.e[0], u.e[1]-v.e[1], u.e[2]-v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v){
    return vec3(u.e[0]*v.e[0], u.e[1]*v.e[1], u.e[2]*v.e[2]);
}

inline vec3 operator*(double t, const vec3& v){
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator/(const vec3& v, double t){
    return (1/t)*v;
}

inline double dot(const vec3& u, const vec3& v){
    return u.e[0]*v.e[0] + u.e[1]*v.e[1] + u.e[2]*v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v){
    return vec3(u.e[1]*v.e[2]-u.e[2]*v.e[1],
                -u.e[0]*v.e[2]+u.e[2]*v.e[0],
                u.e[0]*v.e[1]-u.e[1]*v.e[0]
            );
}

inline vec3 unit_vector(vec3 v){
    return v / v.length();
}

#endif
