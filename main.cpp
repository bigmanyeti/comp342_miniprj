#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <vector>

// --- Config ---
const double G_CONSTANT = 0.005;
const float PHYSICS_SPEED = 0.07f;

float zoomScale = 0.30f;
float offsetX = 0.0f, offsetY = 0.0f;
float aspectRatio = 1.0f;
bool isPaused = false;
bool spacePressedLastFrame = false;
double nextPlanetG = 1.0;

struct Planet {
  double x, y, vx, vy, mass, radius, gMult;
  float r, g, b;
  bool active = true;
};

std::vector<Planet> planets;
double clickStartTime = 0;
bool isMouseDown = false;

// Converts screen pixels to simulation coordinates
void screenToWorld(double mx, double my, int w, int h, double &wx, double &wy) {
  double screenX = (mx / w) * 2.0 - 1.0;
  double screenY = 1.0 - (my / h) * 2.0;
  wx = (screenX * aspectRatio / zoomScale) - offsetX;
  wy = (screenY / zoomScale) - offsetY;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  if (height > 0)
    aspectRatio = (float)width / (float)height;
}

// Scroll callback for Zooming
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  double mx, my;
  int winW, winH;
  glfwGetCursorPos(window, &mx, &my);
  glfwGetWindowSize(window, &winW, &winH);

  // Capture world position before zoom
  double wxBefore, wyBefore;
  screenToWorld(mx, my, winW, winH, wxBefore, wyBefore);

  // Apply zoom
  if (yoffset > 0)
    zoomScale *= 1.1f;
  else
    zoomScale /= 1.1f;

  // Capture world position after zoom
  double wxAfter, wyAfter;
  screenToWorld(mx, my, winW, winH, wxAfter, wyAfter);

  // Adjust offset so the mouse stays over the same world point
  offsetX += (wxAfter - wxBefore);
  offsetY += (wyAfter - wyBefore);
}

void initSolarSystem() {
  planets.clear();
  offsetX = 0;
  offsetY = 0;
  zoomScale = 0.30f;
  planets.push_back({0, 0, 0, 0, 2000.0, 0.22, 1.0, 1.0f, 0.9f, 0.2f}); // Sun

  auto addP = [](double d, double m, double r, float cr, float cg, float cb) {
    double v = std::sqrt((G_CONSTANT * 2000.0) / d);
    planets.push_back({d, 0, 0, v, m, r, 1.0, cr, cg, cb});
  };

  addP(0.4, 0.5, 0.03, 0.6f, 0.6f, 0.6f);  // Mercury
  addP(0.7, 0.9, 0.05, 0.9f, 0.7f, 0.4f);  // Venus
  addP(1.0, 1.0, 0.055, 0.2f, 0.5f, 1.0f); // Earth
  addP(1.5, 0.7, 0.045, 0.9f, 0.3f, 0.2f); // Mars
  addP(2.5, 12.0, 0.12, 0.8f, 0.7f, 0.5f); // Jupiter
  addP(3.5, 10.0, 0.10, 0.9f, 0.8f, 0.6f); // Saturn
  addP(4.5, 7.0, 0.08, 0.5f, 0.8f, 0.9f);  // Uranus
  addP(5.5, 7.0, 0.08, 0.3f, 0.4f, 0.9f);  // Neptune
}

void drawCircle(float cx, float cy, float r, int segs) {
  glBegin(GL_TRIANGLE_FAN);
  for (int i = 0; i <= segs; i++) {
    float a = 2.0f * 3.14159f * i / segs;
    glVertex2f((cx + offsetX) / aspectRatio + (cos(a) * r) / aspectRatio,
               cy + offsetY + sin(a) * r);
  }
  glEnd();
}

void drawGrid() {
  glLineWidth(1.0f);
  glColor4f(0.2f, 0.3f, 0.5f, 0.15f);
  glBegin(GL_LINES);
  float size = 20.0f;
  for (float i = -size; i <= size; i += 0.5f) {
    glVertex2f((i + offsetX) / aspectRatio, -size + offsetY);
    glVertex2f((i + offsetX) / aspectRatio, size + offsetY);
    glVertex2f((-size + offsetX) / aspectRatio, i + offsetY);
    glVertex2f((size + offsetX) / aspectRatio, i + offsetY);
  }
  glEnd();
}

void drawUI() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glColor4f(0.05f, 0.05f, 0.1f, 0.9f);
  glRectf(-0.55f, -0.95f, 0.55f, -0.80f);
  glBegin(GL_QUADS);
  glColor3f(0.2f, 0.8f, 0.3f);
  glVertex2f(-0.5f, -0.90f);
  glColor3f(0.8f, 0.2f, 0.2f);
  glVertex2f(0.5f, -0.90f);
  glColor3f(0.8f, 0.2f, 0.2f);
  glVertex2f(0.5f, -0.85f);
  glColor3f(0.2f, 0.8f, 0.3f);
  glVertex2f(-0.5f, -0.85f);
  glEnd();
  float t = (float)(nextPlanetG - 1.0) / 49.0f;
  float xPos = -0.5f + t;
  glColor3f(1, 1, 1);
  glRectf(xPos - 0.01f, -0.93f, xPos + 0.01f, -0.82f);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void updatePhysics(double dt) {
  if (isPaused)
    return;
  dt *= PHYSICS_SPEED;
  for (size_t i = 0; i < planets.size(); ++i) {
    for (size_t j = i + 1; j < planets.size(); ++j) {
      if (!planets[i].active || !planets[j].active)
        continue;
      double dx = planets[j].x - planets[i].x;
      double dy = planets[j].y - planets[i].y;
      double distSq = dx * dx + dy * dy + 1e-9;
      double dist = std::sqrt(distSq);
      if (dist < (planets[i].radius + planets[j].radius)) {
        double nm = planets[i].mass + planets[j].mass;
        planets[i].vx = (planets[i].vx * planets[i].mass +
                         planets[j].vx * planets[j].mass) /
                        nm;
        planets[i].vy = (planets[i].vy * planets[i].mass +
                         planets[j].vy * planets[j].mass) /
                        nm;
        planets[i].radius = std::sqrt(planets[i].radius * planets[i].radius +
                                      planets[j].radius * planets[j].radius);
        planets[i].mass = nm;
        planets[j].active = false;
        continue;
      }
      double force = (G_CONSTANT * (planets[i].mass * planets[i].gMult) *
                      (planets[j].mass * planets[j].gMult)) /
                     distSq;
      planets[i].vx += (force * dx / dist) / planets[i].mass * dt;
      planets[i].vy += (force * dy / dist) / planets[i].mass * dt;
      planets[j].vx -= (force * dx / dist) / planets[j].mass * dt;
      planets[j].vy -= (force * dy / dist) / planets[j].mass * dt;
    }
  }
  for (auto it = planets.begin(); it != planets.end();) {
    if (!it->active)
      it = planets.erase(it);
    else {
      it->x += it->vx * dt;
      it->y += it->vy * dt;
      ++it;
    }
  }
}

int main() {
  glfwInit();
  GLFWwindow *window =
      glfwCreateWindow(1280, 720, "SpaceTime Zoom Lab", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetScrollCallback(window, scroll_callback); // Register Zoom

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  initSolarSystem();

  while (!glfwWindowShouldClose(window)) {
    double mx, my;
    int winW, winH;
    glfwGetCursorPos(window, &mx, &my);
    glfwGetWindowSize(window, &winW, &winH);
    float sx = (mx / winW) * 2.0 - 1.0, sy = 1.0 - (my / winH) * 2.0;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
      initSolarSystem();
    bool space = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    if (space && !spacePressedLastFrame)
      isPaused = !isPaused;
    spacePressedLastFrame = space;

    // Right Click to Remove
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
      double wx, wy;
      screenToWorld(mx, my, winW, winH, wx, wy);
      for (auto &p : planets) {
        double d = std::sqrt(std::pow(p.x - wx, 2) + std::pow(p.y - wy, 2));
        if (d < p.radius * 1.2)
          p.active = false;
      }
    }

    // Left Click: UI slider or Spawn
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      if (sy < -0.8f && std::abs(sx) < 0.5f) {
        nextPlanetG = 1.0 + ((sx + 0.5) / 1.0) * 49.0;
      } else if (!isMouseDown) {
        clickStartTime = glfwGetTime();
        isMouseDown = true;
      }
    } else if (isMouseDown) {
      double wx, wy;
      screenToWorld(mx, my, winW, winH, wx, wy);
      double rad = (0.02 + (glfwGetTime() - clickStartTime) * 0.05) / zoomScale;
      planets.push_back(
          {wx, wy, 0, 0, rad * 1200, rad, nextPlanetG, 0.4f, 0.6f, 1.0f, true});
      isMouseDown = false;
    }

    updatePhysics(0.016);
    glClearColor(0.01f, 0.01f, 0.02f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glScalef(zoomScale, zoomScale, 1.0f);

    drawGrid();
    for (const auto &p : planets) {
      glColor3f(p.r, p.g, p.b);
      drawCircle(p.x, p.y, p.radius, 40);
    }

    drawUI();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}
