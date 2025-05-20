#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <deque>

// Constants
const float G = 6.67430e-11; // Gravitational constant
const float dt = 0.01;       // Time step
const int TRAIL_LENGTH = 50; // Number of previous positions to show

// Position structure to store historical positions
struct Position
{
    float x, y;
};

// Sphere structure
struct Sphere
{
    float mass;
    float radius;
    float x, y;
    float vx, vy;
    std::deque<Position> trail; // Store previous positions
};

// Initialize two spheres
Sphere sphere1 = {5000000.0f, 0.05f, -0.5f, 0.0f, 0.0f, 0.01f};
Sphere sphere2 = {5000000.0f, 0.05f, 0.5f, 0.0f, 0.0f, -0.01f};

// Function to update positions and velocities
void update()
{
    // Store current positions in trail
    sphere1.trail.push_front({sphere1.x, sphere1.y});
    sphere2.trail.push_front({sphere2.x, sphere2.y});

    // Keep trail at fixed length
    if (sphere1.trail.size() > TRAIL_LENGTH)
        sphere1.trail.pop_back();
    if (sphere2.trail.size() > TRAIL_LENGTH)
        sphere2.trail.pop_back();

    float dx = sphere2.x - sphere1.x;
    float dy = sphere2.y - sphere1.y;
    float distance = sqrt(dx * dx + dy * dy);
    float force = G * sphere1.mass * sphere2.mass / (distance * distance);
    float ax1 = force * dx / (distance * sphere1.mass);
    float ay1 = force * dy / (distance * sphere1.mass);
    float ax2 = -force * dx / (distance * sphere2.mass);
    float ay2 = -force * dy / (distance * sphere2.mass);

    sphere1.vx += ax1 * dt;
    sphere1.vy += ay1 * dt;
    sphere2.vx += ax2 * dt;
    sphere2.vy += ay2 * dt;

    sphere1.x += sphere1.vx * dt;
    sphere1.y += sphere1.vy * dt;
    sphere2.x += sphere2.vx * dt;
    sphere2.y += sphere2.vy * dt;
}

// Function to draw a sphere with trail
void drawSphere(const Sphere &sphere)
{
    // Draw trails first (so they appear behind the sphere)
    for (size_t i = 0; i < sphere.trail.size(); i++)
    {
        float progress = (float)i / TRAIL_LENGTH;     // Progress from newest to oldest
        float alpha = 1.0f - progress;                // Fade out older positions
        float size_factor = 1.0f - (progress * 0.7f); // Reduce size to 30% of original at the end of trail

        glColor4f(1.0f, 1.0f, 1.0f, alpha * 0.3f); // White color with decreasing alpha

        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(sphere.trail[i].x, sphere.trail[i].y);
        for (int j = 0; j <= 360; j += 10)
        {
            float angle = j * M_PI / 180.0f;
            glVertex2f(
                sphere.trail[i].x + cos(angle) * sphere.radius * size_factor,
                sphere.trail[i].y + sin(angle) * sphere.radius * size_factor);
        }
        glEnd();
    }

    // Draw the current sphere
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Solid white for current position
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(sphere.x, sphere.y);
    for (int i = 0; i <= 360; i += 10)
    {
        float angle = i * M_PI / 180.0f;
        glVertex2f(sphere.x + cos(angle) * sphere.radius,
                   sphere.y + sin(angle) * sphere.radius);
    }
    glEnd();
}

// Main function
int main(int argc, char **argv)
{
    if (!glfwInit())
        return -1;

    GLFWwindow *window = glfwCreateWindow(800, 600, "2D Gravity Simulation", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    while (!glfwWindowShouldClose(window))
    {
        update();
        glClear(GL_COLOR_BUFFER_BIT);
        drawSphere(sphere1);
        drawSphere(sphere2);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}