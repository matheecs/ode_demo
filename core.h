/*
June 1,2021
Delete variable K2�v[2]
*/

#pragma once

extern double K0W[2];  //�Ҋ֐ߑO����������p
extern double K1W[2];  //�Ҋ֐߉����������p
// extern double K2W[2];
// //�Ҋ֐߉����������p
extern double HW[2];   //�G�֐ߏ����p
extern double A0W[2];  //����㉺���������p
extern double A1W[2];  //���񉡕��������p
extern unsigned char walkF;  //���s�t���O	�ib0:���s  b1:��  b2:���j
extern double asiPress_r;  //�E��������
extern double asiPress_l;  //����������
extern int uvcOff;         // UVC���s�t���O
extern double frRatI;      //��̊p�␳�p�ϕ��W��
extern double frRatA;      //��̊p�I�t�Z�b�g�l
extern double fhRat;       //���グ�����␳�l
extern double fwMax;       //�����ő�l

class core {
 public:
  float dx[2];  //�O������r�ʒu
  unsigned char jikuasi;
  float adjFR;   //�O�㒲���l
  float autoH;   //������������
  float autoHs;  //������������
  float fw;      //���S����̐U��o���ҕ�
  short mode;
  unsigned char sw;   //���U�苗��
  unsigned char swf;  //���U��ő勗��
  float fwr0;         //���S����̖߂�ҕ�(����)
  float fwr1;         //���S����̖߂�ҕ�(�V�r)
  float landRate;  // 0.1����̓��A�����ڒn���Ԃ̊���
  short fwctEnd;  //������ő�J�E���g��
  short fwct;     //������J�E���^
  float fh;       //���グ�����w��
  short fhMax;    //���グ�ő卂��
  float fhOfs;    //���グ�I�t�Z�b�g�l
  float dy;       //���U�苗��
  float dxi;      //�c�����ϕ��l
  float dyi;      //�������ϕ��l
  float dvi;      //��̊p�ϕ��l
  float dvo;      //��̊p�I�t�Z�b�g�l
  float pitch;
  float roll;

  void footCont(float x, float y, float h, int s);
  void walk(void);
  core(void);
  ~core(void);
};