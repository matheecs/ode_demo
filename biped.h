#pragma once
#include <ode/ode.h>

class bodyStr {
 public:
  dBodyID b;  //�{�f�B��ID
  int e;      //�{�f�B�L��
  dGeomID g;  //�W�I���g����ID
  int ge;     //�W�I���g���ݒ� �L/��
  char k;     //�{�f�B�̎��
  char c;     //�F�̎��
  double l;   //����
  double w;   //��
  double h;   //����
  double r;   //���a
};

class jointStr {
 public:
  dJointID j;  //�W���C���gID
  char k;      //�W���C���g�̎��
  int e;       //�W���C���g�L��
  double x;    // X���W
  double y;    // Y���W
  double z;    // Z���W
  double dn;   //�����p�x
  double up;   //����p�x
  double t;    //�֐ߊp�x
  double t2;   //�֐�2�p�x(���j�o�[�T����)
  double tm;   //�ő�p���x
  double tm2;  //�ő�p���x
  double tk;   //�֐߃g���N
  double tk2;  //�֐߃g���N
  int s;  //���ʐ���t���O(1�̏ꍇ�A���s������ʃW���C���g)
  int c;  //�ėp�J�E���^
  int mode;  //�쓮���[�h
  int pn;    //�����̓J�E���^
  int sv;    //�T�[�{����
};

extern double fbRad;  //���O��p�x		�O��-
extern double lrRad;  //�����E�p�x		�E��-
extern double fbAV;   //���O��p���x	�O��+
extern double lrAV;   //�����E�p���x	�E��+