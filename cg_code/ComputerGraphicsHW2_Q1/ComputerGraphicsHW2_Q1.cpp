#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <vector>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <cmath>

using namespace glm;

// -------------------------------------------------
// Global Variables
// -------------------------------------------------
int Width = 512;
int Height = 512;
std::vector<float> OutputImage;
// -------------------------------------------------


// Stores the ray's origin and direction; used as the ray object from getRay().
class Ray {
public:
    vec3 origin;
    vec3 direction;
    Ray(const vec3& o, const vec3& d) : origin(o), direction(normalize(d)) {}
};

// Stores camera parameters (eye position, viewing region, image plane distance, resolution)
// and generates a ray for each pixel.
class Camera {
public:
    vec3 eyepoint;
    float l, r, b, t, d;
    int nx, ny;

    Camera(const vec3& eyepoint, float l, float r, float b, float t, float d, int nx, int ny)
        : eyepoint(eyepoint), l(l), r(r), b(b), t(t), d(d), nx(nx), ny(ny) {
    }

    Ray getRay(int ix, int iy) const {
        float u = l + (r - l) * (ix + 0.5f) / nx;
        float v = b + (t - b) * (iy + 0.5f) / ny;
        vec3 pixelPos(u, v, -d);
        return Ray(eyepoint, pixelPos - eyepoint);
    }
};

// Abstract class for scene surfaces.
// All surfaces must implement the intersect function.
struct Material {
    vec3 ka, kd, ks;
    float specularPower;
};

class Surface {
public:
    Material material;
    virtual bool intersect(const Ray& ray, float tMin, float tMax, float& tHit, vec3& normal) const = 0;
};

// Represents a sphere surface with center and radius information.
// Implements ray intersection testing.
class Sphere : public Surface {
public:
    vec3 center;
    float radius;

    Sphere(const vec3& center, float radius) : center(center), radius(radius) {}

    virtual bool intersect(const Ray& ray, float tMin, float tMax, float& tHit, vec3& normal) const override {
        vec3 oc = ray.origin - center;
        float A = dot(ray.direction, ray.direction);
        float B = 2.0f * dot(ray.direction, oc);
        float C = dot(oc, oc) - radius * radius;
        float discriminant = B * B - 4 * A * C;

        if (discriminant < 0) { return false; }
        float sqrtDisc = glm::sqrt(discriminant);
        float t1 = (-B - sqrtDisc) / (2.0f * A);
        float t2 = (-B + sqrtDisc) / (2.0f * A);

        float t = (t1 > tMin && t1 < tMax) ? t1 : (t2 > tMin && t2 < tMax) ? t2 : -1.0f;
        if (t > 0) {
            tHit = t;
            vec3 hitPoint = ray.origin + t * ray.direction;
            normal = normalize(hitPoint - center); // Normal vector of the sphere
            return true;
        }
        return false;
    }
};

// Set Plane
class Plane : public Surface {
public:
    float y; // set y

    Plane(float y) : y(y) {}

    virtual bool intersect(const Ray& ray, float tMin, float tMax, float& tHit, vec3& normal) const override {
        if (ray.direction.y == 0.0f) { return false; }
        float t = (y - ray.origin.y) / ray.direction.y;
        if (t > tMin && t < tMax) {
            tHit = t;
            normal = vec3(0.0f, 1.0f, 0.0f); // Normal vector of the plane
            return true;
        }
        return false;
    }
};

// Manages scene surfaces and finds the nearest ray hit.
class Scene {
public:
    std::vector<Surface*> surfaces;

    void addSurface(Surface* surface) {
        surfaces.push_back(surface);
    }

    vec3 trace(const Ray& ray, float tMin, float tMax) const {
        float closest_t = tMax;
        const Surface* hitSurface = nullptr;
        vec3 normal, hitNormal;
        float tHit;

        for (auto surface : surfaces) {
            if (surface->intersect(ray, tMin, closest_t, tHit, normal)) {
                closest_t = tHit;
                hitSurface = surface;
                hitNormal = normal;
            }
        }

        if (!hitSurface) { return vec3(0.0f); } // if not hit return Black.

        vec3 hitPoint = ray.origin + closest_t * ray.direction;

        // Lighting
        vec3 lightPos = vec3(-4, 4, -3);
        vec3 lightColor = vec3(1.0f); // White light
        vec3 lightDir = normalize(lightPos - hitPoint);
        vec3 viewDir = normalize(-ray.direction);
        vec3 reflectDir = reflect(-lightDir, hitNormal); // -lightDir - 2 * dot(-lightDir, hitNormal) * hitNormal;

        // Shadow ray
        Ray shadowRay(hitPoint + 0.001f * hitNormal, lightDir);
        for (auto surface : surfaces) {
            float shadowTHit;
            vec3 tempNormal;
            if (surface->intersect(shadowRay, 0.001f, length(lightPos - hitPoint), shadowTHit, tempNormal)) {
                return hitSurface->material.ka; // In shadow ¡æ only ambient
            }
        }

        // Phong illumination
        const Material& mat = hitSurface->material;

        vec3 ambient = mat.ka;
        vec3 diffuse = mat.kd * max(dot(hitNormal, lightDir), 0.0f);
        vec3 specular = mat.ks * pow(max(dot(viewDir, reflectDir), 0.0f), mat.specularPower);

        return ambient + diffuse + specular;
    }
};


// Set Camera, Scene and Create ray and check the intersection and save the result color to the OutputImage
void render() {
    OutputImage.clear();

    // Camera Eye point (0,0,0), Viewing region (l,r,b,t,d), image size
    Camera camera(vec3(0.0f, 0.0f, 0.0f), -0.1f, 0.1f, -0.1f, 0.1f, 0.1f, Width, Height);

    // Scene Setting
    Scene scene;

    auto* plane = new Plane(-2.0f);
    plane->material = { vec3(0.2f), vec3(1.0f), vec3(0.0f), 0.0f };
    scene.addSurface(plane);

    auto* s1 = new Sphere(vec3(-4, 0, -7), 1.0f);
    s1->material = { vec3(0.2f, 0, 0), vec3(1, 0, 0), vec3(0), 0.0f };
    scene.addSurface(s1);

    auto* s2 = new Sphere(vec3(0, 0, -7), 2.0f);
    s2->material = { vec3(0, 0.2f, 0), vec3(0, 0.5f, 0), vec3(0.5f), 32.0f };
    scene.addSurface(s2);

    auto* s3 = new Sphere(vec3(4, 0, -7), 1.0f);
    s3->material = { vec3(0, 0, 0.2f), vec3(0, 0, 1), vec3(0), 0.0f };
    scene.addSurface(s3);


    // Check every pixel
    for (int iy = 0; iy < Height; ++iy) {
        for (int ix = 0; ix < Width; ++ix) {
            // Create ray
            Ray ray = camera.getRay(ix, iy);
            // Check the intersection and get the color
            vec3 color = scene.trace(ray, 0.0f, 1e30f);
            // set the color
            OutputImage.push_back(color.r);
            OutputImage.push_back(color.g);
            OutputImage.push_back(color.b);
        }
    }
}


void resize_callback(GLFWwindow*, int nw, int nh)
{
    //This is called in response to the window resizing.
    //The new width and height are passed in so we make 
    //any necessary changes:
    Width = nw;
    Height = nh;
    //Tell the viewport to use all of our screen estate
    glViewport(0, 0, nw, nh);

    //This is not necessary, we're just working in 2d so
    //why not let our spaces reflect it?
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, static_cast<double>(Width)
        , 0.0, static_cast<double>(Height)
        , 1.0, -1.0);

    //Reserve memory for our render so that we don't do 
    //excessive allocations and render the image
    OutputImage.reserve(Width * Height * 3);
    render();
}


int main(int argc, char* argv[])
{
    // -------------------------------------------------
    // Initialize Window
    // -------------------------------------------------

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(Width, Height, "OpenGL Viewer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //We have an opengl context now. Everything from here on out 
    //is just managing our window or opengl directly.

    //Tell the opengl state machine we don't want it to make 
    //any assumptions about how pixels are aligned in memory 
    //during transfers between host and device (like glDrawPixels(...) )
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    //We call our resize function once to set everything up initially
    //after registering it as a callback with glfw
    glfwSetFramebufferSizeCallback(window, resize_callback);
    resize_callback(NULL, Width, Height);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // -------------------------------------------------------------
        //Rendering begins!
        glDrawPixels(Width, Height, GL_RGB, GL_FLOAT, &OutputImage[0]);
        //and ends.
        // -------------------------------------------------------------

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        //Close when the user hits 'q' or escape
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
            || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
