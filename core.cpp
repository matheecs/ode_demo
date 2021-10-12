/*
June 1,2021
Corrected setting errors of LEG, autoH, and autoHs.
Corrected (K0�r�U��p�x)
*/

#include "core.h"

// #include<conio.h>
#include <drawstuff/drawstuff.h>
#include <math.h>
#include <ode/ode.h>
#include <stdio.h>
#include <stdlib.h>
// #include <windows.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "biped.h"

#define LEG 180.0  // Update in June 1,2021 :Before revision(#define LEG 190.0)

using namespace std;

core::core(void) {
  adjFR = 2.04;
  autoH = 170;   // Update in June 1,2021 :Before revision(autoH=180)
  autoHs = 180;  // Update in June 1,2021 :Before revision(autoHs=190)
  mode = 0;
  walkF = 0;
  pitch = 0;
  roll = 0;
  frRatI = 0;
  frRatA = 0;
}

core::~core(void) {}

// *****************
// **  �r�ʒu����  **
// *****************
void core::footCont(float x, float y, float h, int s) {
  // x:���_��0�Ƃ��鑫�O����������i�O+�j
  // y:���_��0�Ƃ��鑫���E���������i�E+�j
  // h:���񃍁[��������Ҋ֐߃��[�����܂ł̋���
  // s:�x���r(0)�V�r(1)���w��
  float k;

  k = sqrt(x * x + (y * y + h * h));  // A0����K0�܂ł̋���
  if (k > LEG) k = LEG;               //�v�Z�G���[���

  x = asin(x / k);  // K0�r�U��p�x Update in June 1,2021 :Before revision(
                    // x=asin(x/LEG) )

  k = acos(k / LEG);  // K0�G�Ȃ��p�x

  fbAV = 0;  // UVC�]���ׁ̈A�W���C���͖����ɂ���
  lrAV = 0;
  K0W[s] = k + x + dvi + dvo;
  HW[s] = k * 2;
  A0W[s] = k - x - 0.003 * fbAV;
  k = atan(y / h);  // K1�p�x
  K1W[s] = k;
  if (s == 0)
    A1W[s] = -k - 0.002 * lrAV;
  else
    A1W[s] = -k + 0.002 * lrAV;
}

// *********************
// **  ���s���䃁�C��  **
// *********************
void core::walk(void) {
  short i, j;
  float k;

  switch (mode) {
    ////////////////////////
    //// �����p���Ɉڍs ////
    ////////////////////////
    case 0:
      if (autoHs > autoH)
        autoHs -= 1;
      else
        mode = 10;
      footCont(-adjFR, 0, autoHs, 0);
      footCont(-adjFR, 0, autoHs, 1);
      break;

    //////////////////////
    //// �A�C�h����� ////
    //////////////////////
    case 10:
      K0W[0] = 0;
      K0W[1] = 0;

      //// �p�����[�^������ ////
      dx[0] = 0;
      dx[1] = 0;
      fwr0 = 0;
      fwr1 = 0;
      fwct = 0;
      dxi = 0;
      dyi = 0;
      dy = 0;
      dvi = 0;
      dvo = 0;
      fw = 0;
      jikuasi = 0;
      fwctEnd = 48;
      swf = 12;
      fhMax = 20;
      landRate = 0.2;
      fhOfs = 0;
      footCont(-adjFR, 0, autoH, 0);
      footCont(-adjFR, 0, autoH, 1);
      if (walkF & 0x01) mode = 20;
      break;

    /////////////////////////////////////////////////////////////////
    //////////////////////// �@���s����   ///////////////////////////
    /////////////////////////////////////////////////////////////////
    case 20:
    case 25:
    case 30:

      //###########################################################
      //###################  UVC(��̐�������) ####################
      //###########################################################
      if (fw < fwMax - 1)
        dvo = frRatA;  //// �O�X�O�i ////
      else
        dvo = 0;
      dvi += frRatI * (fbRad + dvo);  //// ��̊p�ϕ� ////

      if ((jikuasi == 0 && asiPress_r < -0.1 && asiPress_l > -0.1) ||
          (jikuasi == 1 && asiPress_r > -0.1 && asiPress_l < -0.1)) {
        k = 1.5 * 193 * sin(lrRad);  //// ���E�����ψ� ////
        if (jikuasi == 0)
          dyi += k;
        else
          dyi -= k;
        if (dyi > 0) dyi = 0;
        if (dyi < -30) dyi = -30;
        k = 1.5 * 130 * sin(fbRad);  //// �O������ψ� ////
        dxi += k;
      }
      dyi *= 0.90;  //����
      if (uvcOff == 1) {
        dxi = 0;
        dyi = 0;
      }

      //###########################################################
      //########################  ��{���e  #######################
      //###########################################################

      //// ���U�� ////
      k = swf * sinf(M_PI * (fwct) / fwctEnd);  // sin�J�[�u
      if (jikuasi == 0)
        dy = k;  //�E�U��
      else
        dy = -k;  //���U��

      //// �������O�U�萧�� ////
      if (fwct < fwctEnd / 2)
        dx[jikuasi] = fwr0 * (1 - 2.0 * fwct / fwctEnd);  //���r�����܂�
      else
        dx[jikuasi] =
            -(fw - dxi) * (2.0 * fwct / fwctEnd - 1);  //���r�����ڍsUVC�K�p

      //// �V�r���O�U�萧�� ////
      if (mode == 20 || mode == 25) {  //���r�V�t�g����
        if (fwct < (landRate * fwctEnd)) {
          dx[jikuasi ^ 1] = fwr1 - (fwr0 - dx[jikuasi]);
          if (mode == 20 && fwct > 2) {
            if ((jikuasi == 0 && asiPress_r < -0.1) ||
                (jikuasi == 1 && asiPress_l < -0.1))
              mode = 25;
            else
              fhOfs += fhRat;
          }
        } else {
          fwr1 = dx[jikuasi ^ 1];
          mode = 30;
        }
      }

      if (mode == 30) {  //�O�U�o
        k = (-cosf(M_PI * (fwct - landRate * fwctEnd) /
                   ((1 - landRate) * fwctEnd)  //�O�U�蒸�_�܂ł̎c��N���b�N��
                   ) +
             1) /
            2;  // 0-1�́�I�J�[�u
        dx[jikuasi ^ 1] = fwr1 + k * (fw - dxi - fwr1);
      }
      if (dx[jikuasi] > 100) {  //�U��o�������~�b�g
        dxi -= dx[jikuasi] - 100;
        dx[jikuasi] = 100;
      }
      if (dx[jikuasi] < -100) {
        dxi -= dx[jikuasi] + 100;
        dx[jikuasi] = -100;
      }
      if (dx[jikuasi ^ 1] > 100) dx[jikuasi ^ 1] = 100;  //�U��o�������~�b�g�}��
      if (dx[jikuasi ^ 1] < -100) dx[jikuasi ^ 1] = -100;

      //// ���㐧�� ////
      i = landRate * fwctEnd;
      if (fwct > i) {
        if (fwct < (fwctEnd - i) / 2)
          fh = fhOfs + fhMax * sinf(M_PI * (fwct - i) / (fwctEnd - i));
        else
          fh = (fhMax + fhOfs) * sinf(M_PI * (fwct - i) / (fwctEnd - i));
      } else
        fh = fhOfs;

      //// �r����֐��Ăяo�� ////
      if (jikuasi == 0) {
        footCont(dx[0] - adjFR, -dy - dyi + 1, autoH, 0);
        footCont(dx[1] - adjFR, dy - dyi + 1, autoH - fh, 1);
      } else {
        footCont(dx[0] - adjFR, -dy - dyi + 1, autoH - fh, 0);
        footCont(dx[1] - adjFR, dy - dyi + 1, autoH, 1);
      }

      //###########################################################
      //###########  CPG�i�����������A����͊ȈՎd�l�j ############
      //###########################################################
      if (fwct == fwctEnd) {  //�ȈՎd�l�i�Œ�����j
        jikuasi ^= 1;
        fwct = 1;
        dxi = 0;
        fwr0 = dx[jikuasi];
        fwr1 = dx[jikuasi ^ 1];
        fh = 0;
        fhOfs = 0;
        mode = 20;
        if (fw < fwr0) fw = fwr0;
        if (fw > fwMax) fw = fwMax;  //// ��̑O��p�ϕ� ////
      } else
        ++fwct;
      break;
  }
}