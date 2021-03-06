// sample11.cpp  by Kosei Demura  2006-2008
#include <drawstuff/drawstuff.h>
#include <ode/ode.h>

static dWorldID world;
static dSpaceID space;
static dGeomID ground;
static dJointID fixed;
static dJointGroupID contactgroup;
dJointFeedback *feedback = new dJointFeedback;

dsFunctions fn;
typedef struct {
  dBodyID body;
  dGeomID geom;
  dReal radius, length, width, height, mass;
} myLink;

myLink box, sensor;

static void nearCallback(void *data, dGeomID o1, dGeomID o2) {
  static int MAX_CONTACTS = 10;
  int i;

  dBodyID b1 = dGeomGetBody(o1);
  dBodyID b2 = dGeomGetBody(o2);

  if (b1 && b2 && dAreConnected(b1, b2)) return;

  dContact contact[MAX_CONTACTS];
  int numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));
  if (numc > 0) {
    for (i = 0; i < numc; i++) {
      contact[i].surface.mode = dContactSoftCFM | dContactSoftERP;
      contact[i].surface.mu = dInfinity;
      contact[i].surface.soft_cfm = 1e-8;
      contact[i].surface.soft_erp = 1.0;
      dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
      dJointAttach(c, dGeomGetBody(contact[i].geom.g1),
                   dGeomGetBody(contact[i].geom.g2));
    }
  }
}

static void simLoop(int pause) {
  static int steps = 0;
  dSpaceCollide(space, 0, &nearCallback);
  dWorldStep(world, 0.01);
  dJointGroupEmpty(contactgroup);
  feedback = dJointGetFeedback(fixed);
  // Get force and torque of the joint
  printf("%5d Force fx=%6.2f ", steps++, feedback->f1[0]);
  // x axis
  printf("fy=%6.2f ", feedback->f1[1]);
  // y axis
  printf("fz=%6.2f \n", feedback->f1[2]);
  // z axis

  // Draw a box
  dsSetColor(1.0, 0.0, 0.0);
  dReal sides1[] = {box.length, box.width, box.height};
  dsDrawBoxD(dBodyGetPosition(box.body), dBodyGetRotation(box.body),
             sides1);  // Draw a sensor
  dsSetColor(0.0, 0.0, 1.0);
  dReal sides2[] = {sensor.length, sensor.width, sensor.height};
  dsDrawBoxD(dBodyGetPosition(sensor.body), dBodyGetRotation(sensor.body),
             sides2);
}

void start() {
  static float xyz[3] = {0.0, -3.0, 1.0};
  static float hpr[3] = {90.0, 0.0, 0.0};
  dsSetViewpoint(xyz, hpr);
}

void setDrawStuff() {
  fn.version = DS_VERSION;
  fn.start = &start;
  fn.step = &simLoop;
  fn.command = NULL;
  fn.stop = NULL;
  fn.path_to_textures =
      "/Users/zhangjixiang/Code/ode-0.16.2/drawstuff/textures";
}

int main(int argc, char **argv) {
  setDrawStuff();
  dInitODE();
  world = dWorldCreate();
  space = dHashSpaceCreate(0);
  contactgroup = dJointGroupCreate(0);
  dWorldSetGravity(world, 0, 0, -9.8);
  ground = dCreatePlane(space, 0, 0, 1, 0);
  dMass m1;
  dReal x0 = 0.0, y0 = 0.0, z0 = 0.0;

  // A sensor (lower box)
  sensor.length = 0.2;
  sensor.width = 0.2;
  sensor.height = 0.2;
  sensor.mass = 1.0;
  sensor.body = dBodyCreate(world);
  dMassSetZero(&m1);
  dMassSetBoxTotal(&m1, sensor.mass, sensor.length, sensor.width,
                   sensor.height);
  dBodySetMass(sensor.body, &m1);
  dBodySetPosition(sensor.body, x0, y0, 0.5 * sensor.height + z0);
  sensor.geom = dCreateBox(space, sensor.length, sensor.width, sensor.height);
  dGeomSetBody(sensor.geom, sensor.body);

  // The upper box
  box.length = 0.2;
  box.width = 0.2;
  box.height = 0.2;
  box.mass = 1.0;
  box.body = dBodyCreate(world);
  dMassSetZero(&m1);
  dMassSetBoxTotal(&m1, box.mass, box.length, box.width, box.height);
  dBodySetMass(box.body, &m1);
  dBodySetPosition(box.body, x0, y0, sensor.height + 0.5 * box.height + z0);
  box.geom = dCreateBox(space, box.length, box.width, box.height);
  dGeomSetBody(box.geom, box.body);

  // A fixed joint
  fixed = dJointCreateFixed(world, 0);
  dJointAttach(fixed, box.body, sensor.body);
  dJointSetFixed(
      fixed);  // Set a joint to get information about force and torque
  dJointSetFeedback(fixed, feedback);

  dsSimulationLoop(argc, argv, 352, 288, &fn);

  dWorldDestroy(world);
  dCloseODE();
  return 0;
}