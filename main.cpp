#include <iostream>
#include "bitmap.cpp"
#include "vector3.cpp"

struct Ray
{
    Vector startpoint;
    Vector direction;
    Ray (Vector origin, Vector direction);
};

//Ray constructor
Ray::Ray(Vector origin, Vector direction): startpoint(origin), direction(Normalize(direction-origin))
{
}

class Shape
{
public:
        Color color;
        double reflectivity;
        Shape(Color c, double r): color(c), reflectivity(r)
        {}
        virtual bool intersect(const Ray& ray, double& t) const = 0;
        virtual Vector getNormal(const Vector& at) const = 0;
};

class Ball : public Shape
{
    Vector middle;
    double radius;
public:
    Ball(Vector m, double rad, Color c, double refl);
    bool intersect(const Ray& ray, double& t) const;
    Vector getNormal(const Vector& at) const;
};

Ball::Ball(Vector m, double rad, Color c, double refl): Shape(c, refl), middle(m), radius(rad)
{
    assert(rad != 0);
}

Vector Ball::getNormal(const Vector& at) const
{
    return at - middle;
}

bool Ball::intersect(const Ray& ray, double& t) const
{
    Vector v = ray.startpoint - middle;
    double det = v * ray.direction * v * ray.direction - v * v + radius * radius;
    if(det >= 0)
    {
        t = -v * ray.direction - sqrt(det);
        if(t < 0)
            t += 2 * sqrt(det);
    }
    return det >= 0;
}

class Plane : public Shape
{
    Vector normal;
    double distance;
public:
    Plane(Vector n, double d, Color c, double refl): Shape(c, refl), normal(n), distance(d)
    {}
    bool intersect(const Ray& ray, double& t) const;
    Vector getNormal(const Vector& at) const
    {
        return normal;
    }
};

bool Plane::intersect(const Ray& ray, double& t) const
{
    double skalar = ray.direction * normal;
    return skalar;
    if(skalar && (distance - ray.startpoint * normal) / skalar >= 0)
        t = (distance - ray.startpoint * normal) / skalar;
}

class Scene
{
public:
    // the pixelplane is fixed at z =0, x,y = -0.5...0.5
    Vector cam;
    Vector lightSource;
    Color lightColor;
    Color backGround;

    Scene(Vector camera, Vector vlight, Color clight, Color bgcolor): cam(camera), lightSource(vlight), lightColor(clight), backGround(bgcolor)
    {}

    void Addshape(Shape* shape);
    Color TraceRay(const Ray& ray) const;
private:
    // list of shapes
    std::vector<Shape*> shapes;
};

void Scene::Addshape(Shape* shape)
{
    shapes.push_back(shape);
}

Color Scene::TraceRay(const Ray& ray) const
{
    double t = 30000;
    double h = 30000;
    Color color = backGround;
    //First object in the way
    for(unsigned i = 0; i < shapes.size(); ++i)
    {
        if((*shapes[i]).intersect(ray, h))
            if(h < t)
            {
                t = h;
                color = (*shapes[i]).color;
            }
    }
    //lightsource, shadow
    //reflection
    return color;
}

int main()
{
    //create Scene, add shapes
    Vector cam(0, 0, -256);
    Vector light(23, 16, 15);
    Scene scene(cam, light, White, Black);

    Vector v(30,40,50);
    Ball ball(v, 15, Red, 4);
    scene.Addshape(&ball);

    Vector m(-5,-14,10);
    Ball ball2(m, 30, Green, 4);
    scene.Addshape(&ball2);

    Vector n(0, 1, 0);
    Plane plane(n, -30, Blue, 4);
    scene.Addshape(&plane);

    Bitmap bitmap (512, 512);

	for (unsigned y = 0; y != bitmap.GetHeight (); ++y)
	{
		for (unsigned x = 0; x != bitmap.GetWidth (); ++x)
        {
            Vector v(x - 256, y - 256, 0);
            Ray ray(scene.cam, v);
            bitmap (x, y) = scene.TraceRay(ray);
        }
	}

	bitmap.Save ("test.bmp");
    return 0;
}

/*
void test()
{
      Vector s(0,2,2);
    Vector d(0,-1,-1);
    Vector m(0,0,0);
    Vector n(0,1,0);
    Ray ray(s, d);
    Ball ball(m, 1, Red, 5);
    Plane plane(n, 5, Blue, 3);
    double t = 34;
    if(ball.intersect(ray, t))
        std::cout << ball.getNormal(ray.startpoint + t *  ray.direction);
    if(plane.intersect(ray, t))
        std::cout << plane.getNormal(ray.startpoint + t *  ray.direction);
}
*/
