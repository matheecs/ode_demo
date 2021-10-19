#include <drawstuff/drawstuff.h>
#include <ode/ode.h>

#ifdef dDOUBLE
#define dsDrawCapsule dsDrawCapsuleD
#define dsDrawBox dsDrawBoxD
#define dsDrawSphere dsDrawSphereD
#endif

#define NUM 4
#define LENGTH 1.0
#define WIDTH 0.2
#define HEIGHT 0.2

static dWorldID world;
static dSpaceID space;
static dGeomID ground;
static dJointGroupID contactgroup;
dsFunctions fn;

typedef struct {
  dBodyID body;
  dGeomID geom;
  dReal radius;
  dReal length;
  dReal width;
  dReal height;
  dReal mass;
} myLink;

myLink torse;

myLink hip[4];
myLink lower[4];
myLink upper[4];

dJointID hx[4];
dJointID hy[4];
dJointID kn[4];

void start() {
  static float xyz[3] = {0.0, -3.0, 1.0};
  static float hpr[3] = {90.0, 0.0, 0.0};
  dsSetViewpoint(xyz, hpr);
}

static void nearCallback(void* data, dGeomID o1, dGeomID o2) {
  static int MAX_CONTACTS = 10;

  dBodyID b1 = dGeomGetBody(o1);
  dBodyID b2 = dGeomGetBody(o2);

  if (b1 && b2 && dAreConnected(b1, b2)) return;

  dContact contact[MAX_CONTACTS];
  int numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));
  if (numc > 0) {
    for (int i = 0; i < numc; ++i) {
      contact[i].surface.mode = dContactSoftCFM | dContactSoftERP;
      contact[i].surface.mu = 1.0;
      contact[i].surface.soft_cfm = 1e-8;
      contact[i].surface.soft_erp = 1.0;
      dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
      dJointAttach(c, dGeomGetBody(contact[i].geom.g1),
                   dGeomGetBody(contact[i].geom.g2));
    }
  }
}

static void simLoop(int pause) {
  dSpaceCollide(space, 0, &nearCallback);
  dWorldStep(world, 0.01);
  dJointGroupEmpty(contactgroup);

  // DrawStuff
  dsSetColor(1.0, 0.0, 0.0);
  dReal size[] = {torse.length - 0.1, torse.width - 0.04, torse.height};
  dsDrawBox(dBodyGetPosition(torse.body), dBodyGetRotation(torse.body), size);

  for (int i = 0; i < NUM; ++i) {
    dsSetColor(0.0, 0.0, 1.0);
    dsDrawCapsule(dBodyGetPosition(hip[i].body), dBodyGetRotation(hip[i].body),
                  hip[i].length, hip[i].radius);
    dsSetColor(1.0, 0.0, 1.0);
    dsDrawCapsule(dBodyGetPosition(upper[i].body),
                  dBodyGetRotation(upper[i].body), upper[i].length,
                  upper[i].radius);
    dsSetColor(0.0, 1.0, 1.0);
    dsDrawCapsule(dBodyGetPosition(lower[i].body),
                  dBodyGetRotation(lower[i].body), lower[i].length,
                  lower[i].radius);
  }
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

void makeRobot() {
  dReal init_z = 1.0;
  {
    // set torse link
    dMass mass;
    torse.length = LENGTH;
    torse.width = WIDTH;
    torse.height = HEIGHT;
    torse.mass = 20;
    torse.body = dBodyCreate(world);
    dMassSetZero(&mass);
    dMassSetBoxTotal(&mass, torse.mass, torse.length, torse.width,
                     torse.height);
    dBodySetMass(torse.body, &mass);
    dBodySetPosition(torse.body, 0, 0, init_z);
    torse.geom =
        dCreateBox(space, torse.length - 0.1, torse.width - 0.04, torse.height);
    dGeomSetBody(torse.geom, torse.body);
  }

  {
    // set leg links and joints
    dMass mass;
    dMatrix3 R;
    dReal offset_x = LENGTH / 2;
    dReal offset_y = WIDTH / 2;
    dReal offset_z = HEIGHT / 2;
    dReal x[NUM] = {offset_x, offset_x, -offset_x, -offset_x};
    dReal y[NUM] = {-offset_y, offset_y, -offset_y, offset_y};
    dReal z = init_z - offset_z;
    for (int i = 0; i < NUM; ++i) {
      hip[i].body = dBodyCreate(world);
      hip[i].length = 0.03;
      hip[i].radius = 0.01;
      hip[i].mass = 0.1;
      dMassSetZero(&mass);
      dMassSetCapsule(&mass, hip[i].mass, 1, hip[i].radius, hip[i].length);
      dBodySetMass(hip[i].body, &mass);
      dBodySetPosition(hip[i].body, x[i], y[i], z);
      dRFromAxisAndAngle(R, 0, 1, 0, M_PI_2);
      dBodySetRotation(hip[i].body, R);
      hip[i].geom = dCreateCapsule(space, hip[i].radius, hip[i].length);
      dGeomSetBody(hip[i].geom, hip[i].body);
    }
    for (int i = 0; i < NUM; ++i) {
      hx[i] = dJointCreateHinge(world, 0);
      dJointAttach(hx[i], torse.body, hip[i].body);
      dJointSetHingeAnchor(hx[i], x[i], y[i], z);
      dJointSetHingeAxis(hx[i], 1, 0, 0);
    }

    // upper
    for (int i = 0; i < NUM; ++i) {
      upper[i].body = dBodyCreate(world);
      upper[i].length = 0.3;
      upper[i].radius = 0.01;
      upper[i].mass = 0.1;
      dMassSetZero(&mass);
      dMassSetCapsule(&mass, upper[i].mass, 1, upper[i].radius,
                      upper[i].length);
      dBodySetMass(upper[i].body, &mass);
      dBodySetPosition(upper[i].body, x[i], y[i], z - 0.5 * upper[i].length);
    }
    for (int i = 0; i < NUM; ++i) {
      hy[i] = dJointCreateHinge(world, 0);
      dJointAttach(hy[i], hip[i].body, upper[i].body);
      dJointSetHingeAnchor(hy[i], x[i], y[i], z);
      dJointSetHingeAxis(hy[i], 0, 1, 0);
    }

    // lower
    for (int i = 0; i < NUM; ++i) {
      lower[i].body = dBodyCreate(world);
      lower[i].length = 0.3;
      lower[i].radius = 0.01;
      lower[i].mass = 0.1;
      dMassSetZero(&mass);
      dMassSetCapsule(&mass, lower[i].mass, 1, lower[i].radius,
                      lower[i].length);
      dBodySetMass(lower[i].body, &mass);
      dBodySetPosition(lower[i].body, x[i], y[i], z - 1.5 * lower[i].length);

      lower[i].geom = dCreateCapsule(space, lower[i].radius, lower[i].length);
      dGeomSetBody(lower[i].geom, lower[i].body);
    }
    for (int i = 0; i < NUM; ++i) {
      hy[i] = dJointCreateHinge(world, 0);
      dJointAttach(hy[i], upper[i].body, lower[i].body);
      dJointSetHingeAnchor(hy[i], x[i], y[i], z - lower[i].length);
      dJointSetHingeAxis(hy[i], 0, 1, 0);
    }
  }
}

int main(int argc, char** argv) {
  dInitODE();
  setDrawStuff();
  world = dWorldCreate();
  space = dHashSpaceCreate(0);
  contactgroup = dJointGroupCreate(0);
  ground = dCreatePlane(space, 0, 0, 1, 0);
  dWorldSetGravity(world, 0, 0, -9.8);
  dWorldSetCFM(world, 1e-3);
  dWorldSetERP(world, 0.9);

  makeRobot();

  dsSimulationLoop(argc, argv, 800, 480, &fn);
  dSpaceDestroy(space);
  dWorldDestroy(world);
  dCloseODE();
  return 0;
}