#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <slvs.h>

static Slvs_Param* find_param (Slvs_System* sys, Slvs_hParam h) {
  for (int i = 0; i < sys->params; i++) {
    if (h == sys->param[i].h)
      return &sys->param[i];
  }
  return NULL;
}

static void *CheckMalloc(size_t n)
{
    void *r = malloc(n);
    if(!r) {
        printf("out of memory!\n");
        exit(-1);
    }
    return r;
}

extern "C" {
  Slvs_System* Slvs_Make_System (int num) {
    auto sys        = new Slvs_System();
    sys->param      = (Slvs_Param*)CheckMalloc(num*sizeof(sys->param[0]));
    sys->entity     = (Slvs_Entity*)CheckMalloc(num*sizeof(sys->entity[0]));
    sys->constraint = (Slvs_Constraint*)CheckMalloc(num*sizeof(sys->constraint[0]));
    sys->failed     = (Slvs_hConstraint*)CheckMalloc(num*sizeof(sys->failed[0]));
    sys->faileds    = num;
    return sys;
  }

  int Slvs_Add_Param (Slvs_System* sys, Slvs_hParam h, Slvs_hGroup group, double value) {
    // printf("PARAM %d G %d VALUE %f\n", h, group, value);
    auto param = Slvs_MakeParam(h, group, value);
    sys->param[sys->params++] = param;
    return 1;
  }
  double Slvs_Param_Get_Value (Slvs_System* sys, Slvs_hParam h) {
    Slvs_Param* param = find_param(sys, h);
    if (param != NULL) 
      return param->val;
    else
      return 0.0;
  }
  double Slvs_Param_Set_Value (Slvs_System* sys, Slvs_hParam h, double val) {
    Slvs_Param* param = find_param(sys, h);
    if (param != NULL)
      param->val = val;
    return 0;
  }
  int Slvs_System_DOF (Slvs_System* sys) {
    return sys->dof;
  }
  int Slvs_System_Result (Slvs_System* sys) {
    return sys->result;
  }
  int Slvs_System_Set_Calculate_Faileds (Slvs_System* sys, int num) {
    sys->calculateFaileds = num;
    return 1;
  }
  int Slvs_System_Set_Dragged (Slvs_System* sys, int idx, int num) {
    sys->dragged[idx] = num;
    return 1;
  }
  int Slvs_Add_Point2d(Slvs_System* sys, Slvs_hEntity h, Slvs_hGroup group, Slvs_hEntity workplane, Slvs_hParam u, Slvs_hParam v) {
    // printf("POINT3d H %d G %d WP %d u %d v %d\n", h, group, workplane, u, v);
    sys->entity[sys->entities++] = Slvs_MakePoint2d(h, group, workplane, u, v);
    return 1;
  }
  int Slvs_Add_Point3d(Slvs_System* sys, Slvs_hEntity h, Slvs_hGroup group, Slvs_hParam x, Slvs_hParam y, Slvs_hParam z) {
    // printf("POINT3d H %d G %d x %d y %d z %d\n", h, group, x, y, z);
    sys->entity[sys->entities++] = Slvs_MakePoint3d(h, group, x, y, z);
    return 1;
  }
  int Slvs_Add_Normal3d(Slvs_System* sys, Slvs_hEntity h, Slvs_hGroup group, Slvs_hParam qw, Slvs_hParam qx, Slvs_hParam qy, Slvs_hParam qz) {
    // printf("NORMAL3d H %d G %d w %d x %d y %d z %d\n", h, group, qw, qx, qy, qz);
    sys->entity[sys->entities++] = Slvs_MakeNormal3d(h, group, qw, qx, qy, qz);
    return 1;
  }
  int Slvs_Add_Normal2d(Slvs_System* sys, Slvs_hEntity h, Slvs_hGroup group, Slvs_hEntity workplane) {
    sys->entity[sys->entities++] = Slvs_MakeNormal2d(h, group, workplane);
    return 1;
  }
  int Slvs_Add_Distance(Slvs_System* sys, Slvs_hEntity h, Slvs_hGroup group, Slvs_hEntity workplane, Slvs_hParam d) {
    sys->entity[sys->entities++] = Slvs_MakeDistance(h, group, workplane, d);
    return 1;
  }
  int Slvs_Add_LineSegment(Slvs_System* sys, Slvs_hEntity h, Slvs_hGroup group, Slvs_hEntity workplane, Slvs_hEntity pta, Slvs_hEntity ptb) {
    // printf("LINESEGMENT H %d G %d WP %d PA %d PB %d\n", h, group, workplane, pta, ptb);
    sys->entity[sys->entities++] = Slvs_MakeLineSegment(h, group, workplane, pta, ptb);
    return 1;
  }
  int Slvs_Add_Cubic(Slvs_System* sys, Slvs_hEntity h, Slvs_hGroup group, Slvs_hEntity workplane, Slvs_hEntity pt0, Slvs_hEntity pt1, Slvs_hEntity pt2, Slvs_hEntity pt3) {
    sys->entity[sys->entities++] = Slvs_MakeCubic(h, group, workplane, pt0, pt1, pt2, pt3);
    return 1;
  }
  int Slvs_Add_ArcOfCircle(Slvs_System* sys, Slvs_hEntity h, Slvs_hGroup group, Slvs_hEntity workplane, Slvs_hEntity normal, Slvs_hEntity center, Slvs_hEntity start, Slvs_hEntity end) {
    sys->entity[sys->entities++] = Slvs_MakeArcOfCircle(h, group, workplane, normal, center, start, end);
    return 1;
  }
  int Slvs_Add_Circle(Slvs_System* sys, Slvs_hEntity h, Slvs_hGroup group, Slvs_hEntity workplane, Slvs_hEntity center, Slvs_hEntity normal, Slvs_hEntity radius) {
    sys->entity[sys->entities++] = Slvs_MakeCircle(h, group, workplane, center, normal, radius);
    return 1;
  }
  int Slvs_Add_Workplane(Slvs_System* sys, Slvs_hEntity h, Slvs_hGroup group, Slvs_hEntity origin, Slvs_hEntity normal) {
    // printf("WORKPLANE H %d G %d ORIGIN %d NORMAL %d\n", h, group, origin, normal);
    sys->entity[sys->entities++] = Slvs_MakeWorkplane(h, group, origin, normal);
    return 1;
  }
  int Slvs_Add_Constraint(Slvs_System* sys, Slvs_hEntity h, Slvs_hGroup group, int type, Slvs_hEntity workplane, double valA, Slvs_hEntity ptA, Slvs_hEntity ptB, Slvs_hEntity entityA, Slvs_hEntity entityB) {
    printf("CONSTRAINT H %d G %d T %d WP %d VA %f PA %d PB %d EA %d EB %d\n", h, group, type, workplane, valA, ptA, ptB, entityA, entityB);
    sys->constraint[sys->constraints++] = Slvs_MakeConstraint(h, group, type, workplane, valA, ptA, ptB, entityA, entityB);
    return 1;
  }

  int demo (void) {
    Slvs_hGroup g;
    double qw, qx, qy, qz;

    Slvs_System* sys = Slvs_Make_System (50);

    g = 1;
    Slvs_Add_Param(sys, 2, g, 0.0);
    Slvs_Add_Param(sys, 3, g, 0.0);
    Slvs_Add_Param(sys, 4, g, 0.0);
    Slvs_Add_Point3d(sys, 5, g, 2, 3, 4);
    Slvs_MakeQuaternion(1, 0, 0,
                        0, 1, 0, &qw, &qx, &qy, &qz);
    Slvs_Add_Param(sys, 6, g, qw);
    Slvs_Add_Param(sys, 7, g, qx);
    Slvs_Add_Param(sys, 8, g, qy);
    Slvs_Add_Param(sys, 9, g, qz);
    Slvs_Add_Normal3d(sys, 10, g, 6, 7, 8, 9);

    Slvs_Add_Workplane(sys, 11, g, 5, 10);

    g = 12;
    Slvs_Add_Param(sys, 13, g, 10.0);
    Slvs_Add_Param(sys, 14, g, 20.0);
    Slvs_Add_Point2d(sys, 15, g, 11, 13, 14);

    Slvs_Add_Param(sys, 16, g, 20.0);
    Slvs_Add_Param(sys, 17, g, 10.0);
    Slvs_Add_Point2d(sys, 18, g, 11, 16, 17);

    Slvs_Add_LineSegment(sys, 19, g, 11, 15, 18);

    Slvs_Add_Constraint(sys, 20, g, SLVS_C_PT_PT_DISTANCE, 11, 15.0, 15, 5, 0, 0);
    sys->calculateFaileds = 1;
    Slvs_Solve(sys, g);
    return 0;
  }
}

// int main (int argc, char* argv[]) {
//   demo();
//   return 0;
// }
