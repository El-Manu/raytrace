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
    if(det < 0)
    {
        return false;
    }
    double root = sqrt(det);
    double t1 = -v * ray.direction - root;
    double t2 = t1 + 2* root;
    if(t1 < 0){
        if(t2 < 0) return false;
        t = t2;
        return true;
    }
    if(t2 < 0){
        t = t1;
        return true;
    }
    t = std::min(t1, t2);
    return true;
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
    t = (distance - ray.startpoint * normal) / skalar;
    return skalar && t > 0;
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
    
    //First object in the way
    Shape* shape = NULL;
    for(unsigned i = 0; i < shapes.size(); ++i)
    {
        if(shapes[i]->intersect(ray, t))
            if(t < h)
            {
                h = t;
                shape = shapes[i];
            }
    }
    if(shape == NULL)
    {
        return backGround;
    }
    // ambient
    Color color = shape->color;
    
    Vector intersect = ray.startpoint + h * ray.direction;
    Vector normal = Normalize(shape->getNormal(intersect));
    Ray reflectionRay = Ray(intersect, Vector());
    reflectionRay.direction = Normalize(ray.direction -2 * (ray.direction * normal) * normal);
    reflectionRay.startpoint += 0.5 * reflectionRay.direction;

    // lightsource, shadow
    Ray shadowRay(intersect, lightSource);
    shadowRay.startpoint += 0.5 * shadowRay.direction;
    bool shade = false;

    for (unsigned i = 0; i < shapes.size(); ++i)
    {
        h = 30000;
        if (shapes[i]->intersect(shadowRay, h) && h > 0)
        {
            shade = true;
            break;
        }
    }
    if (!shade)
    {
        // difuse
        color += normal * shadowRay.direction * shape->color * lightColor;
        //specular
        color += std::pow(shadowRay.direction * reflectionRay.direction, 8) * lightColor * shape->color;
    }
    // reflection

    return color;
}

int main()
{
    //create Scene, add shapes
    Vector cam(0, 0, -4096);
    Vector light(-200, 800, 300);
    Scene scene(cam, light, White, Black);

    Vector v(100,350,500);
    Ball ball(v, 150, 0.3 * Red, 4);
    scene.Addshape(&ball);

    Vector m(-200, 14,400);
    Ball ball2(m, 60, 0.5 * Green, 4);
    scene.Addshape(&ball2);

    Vector n(0, 1, 0);
    Plane plane(n, -30, 0.2 * Blue, 4);
    scene.Addshape(&plane);

    Bitmap bitmap (1024, 1024);

	for (int y = 0; y != bitmap.GetHeight(); ++y)
	{
		for (int x = 0; x != bitmap.GetWidth(); ++x)
        {
            Vector v(x - (int)bitmap.GetWidth() / 2, (int)bitmap.GetHeight() / 2 - y, 0);
            Ray ray(scene.cam, v);
            bitmap (x, y) = AdjustColor(scene.TraceRay(ray));
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
