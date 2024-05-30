#include <mega32.h>
#include <glcd.h>
#include <font5x7.h>
#include <stdio.h>
#include <graphics.h>
#include <delay.h>
#include <math.h>
// items in e2p
char itemsE2P[5] = {0x10, 0x20, 0x30, 0x40, 0x50};
int stepperXidx = 0;
int stepperYidx = 0;
char KeyPressed;
char stringOut[32];

char mode = 1;
int desk[2] = {128, 64};
int headx = 0;
int heady = 0;
unsigned char speed = 20; // RPM
float stpperDelay = 0;    // ms
int rotatePerStep = 20;   // degree

void Laser(char state)
{
      float scale = 1;
      if (state == 0 || headx > desk[0] || heady > desk[1] || headx < 0 || heady < 0)
      {
            PORTB .6 = 0;
            return;
      }
      PORTB .6 = 1;
      if (desk[1] > 64)
            scale = (64.0 / desk[1]);
      else if (desk[0] > 128)
            scale = (128.0 / desk[0]);
      glcd_setpixel(headx * scale, heady * scale);
}
void goStepX(signed char dir)
{
      char stepperX[4] = {1, 2, 4, 8};
      if ((headx + dir >= 0 && headx + dir <= desk[0]) || (mode == 2))
      {
            stepperXidx += dir;
            if (stepperXidx > 3)
                  stepperXidx = 0;
            if (stepperXidx < 0)
                  stepperXidx = 3;
            PORTD = (PORTD & 0xF0) | stepperX[stepperXidx];
            headx += dir;
            delay_ms(stpperDelay);
      }
}
void goStepY(signed char dir)
{
      char stepperY[4] = {16, 32, 64, 128};
      if ((heady + dir >= 0 && heady + dir <= desk[1]) || (mode == 2))
      {
            stepperYidx += dir;
            if (stepperYidx > 3)
                  stepperYidx = 0;
            if (stepperYidx < 0)
                  stepperYidx = 3;
            PORTD = (PORTD & 0x0F) | stepperY[stepperYidx];
            heady += dir;
            delay_ms(stpperDelay);
      }
}

void E2PW(char data, int adr)
{
      while ((EECR & 0x02) != 0)
            ;
      EEDR = data;
      EEAR = adr;
      EECR |= 0x04;
      EECR |= 0x02;
}

char E2PR(int adr)
{
      while ((EECR & 0x02) != 0)
            ;
      EEAR = adr;
      EECR |= 0x01;
      return EEDR;
}

int getNum(char i)
{
      int num = 0;
      sprintf(stringOut, "parameter %d: ", i + 1);
      glcd_outtextxy(0, 30, stringOut);
      KeyPressed = 'x';
      while (KeyPressed == 'x' || KeyPressed == '=')
            ;
      do
      {
            if (KeyPressed == 'c')
                  return -1;
            num *= 10;
            num += KeyPressed - 48;
            KeyPressed = 'x';
            sprintf(stringOut, "%d", num);
            glcd_outtextxy(0, 40, stringOut);
            while (KeyPressed == 'x')
                  ;
      } while (KeyPressed != '=');
      glcd_outtextxy(0, 40, "        ");
      return num;
}

int ZoneScope(char pointx, char x, char pointy, char y, int TetaSin, int TetaCos)
{
      int teta = 0;
      if (pointx > x && pointy >= y)
            if (TetaSin >= 0 && TetaSin < 90)
                  teta = TetaSin;
            else
                  teta = TetaCos;
      if (pointx <= x && pointy > y)
            if (TetaSin >= 90 && TetaSin < 180)
                  teta = TetaSin;
            else
                  teta = TetaCos;
      if (pointx < x && pointy <= y)
            if (TetaSin >= 180 && TetaSin < 270)
                  teta = TetaSin;
            else
                  teta = TetaCos;
      if (pointx >= x && pointy < y)
            if (TetaSin >= 270 && TetaSin < 360)
                  teta = TetaSin;
            else
                  teta = TetaCos;
      return teta;
}

void takeParaLine(const char itemNumber)
{
      char i;
      char backup = itemNumber;
      char string[60];
      char startAdr = 0x00;
      int parameter[4] = {0, 0, 0, 0};
      sprintf(string, "Mode 1 Enter Line parameters 1-4 (x1, y1, x2, y2) for item %d:", itemNumber);
      glcd_clear();
      glcd_outtextxy(0, 0, string);
      for (i = 0; i < 4; i++)
      {
            parameter[i] = getNum(i);
            if (parameter[i] == -1)
                  return;
      }
      if (parameter[0] > desk[0] || parameter[1] > desk[1] || parameter[2] > desk[0] || parameter[3] > desk[1])
      {
            glcd_outtextxy(0, 30, "invalid!    ");
            return;
      }
      startAdr = itemsE2P[backup - 1];
      E2PW(1, startAdr);
      for (i = 0; i < 4; i++)
            E2PW(parameter[i], startAdr + i + 1);

      glcd_outtextxy(0, 30, "Line saved!    ");
}
void takeParaRect(const char itemNumber)
{
      char i;
      char backup = itemNumber;
      char string[60];
      char startAdr = 0x00;
      int parameter[4] = {0, 0, 0, 0};
      sprintf(string, "Mode 1 Enter Rectangle parameters 1-4 (x1, y1, x2, y2) for item %d:", itemNumber);
      glcd_clear();
      glcd_outtextxy(0, 0, string);
      for (i = 0; i < 4; i++)
      {
            parameter[i] = getNum(i);
            if (parameter[i] == -1)
                  return;
      }
      if (parameter[0] > desk[0] || parameter[1] > desk[1] || parameter[2] > desk[0] || parameter[3] > desk[1])
      {
            glcd_outtextxy(0, 30, "invalid!    ");
            return;
      }
      startAdr = itemsE2P[backup - 1];
      E2PW(2, startAdr);
      for (i = 0; i < 4; i++)
            E2PW(parameter[i], startAdr + i + 1);

      glcd_outtextxy(0, 30, "Rectangle saved!    ");
}
void takeParaCirc(char itemNumber)
{
      char i;
      char string[60];
      char backup = itemNumber;
      char startAdr = 0x00;
      int parameter[3] = {0, 0, 0};
      sprintf(string, "Mode 1 Enter Circle parameters 1-3 (x, y, r) for item %d:", itemNumber);
      glcd_clear();
      glcd_outtextxy(0, 0, string);
      for (i = 0; i < 3; i++)
      {
            parameter[i] = getNum(i);
            if (parameter[i] == -1)
                  return;
      }
      if (parameter[0] > desk[0] || parameter[1] > desk[1] || parameter[0] + parameter[2] > desk[0] || parameter[0] - parameter[2] < 0 || parameter[1] + parameter[2] > desk[1] || parameter[1] - parameter[2] < 0)
      {
            glcd_outtextxy(0, 30, "invalid!    ");
            return;
      }
      startAdr = itemsE2P[backup - 1];
      E2PW(3, startAdr);
      for (i = 0; i < 3; i++)
            E2PW(parameter[i], startAdr + i + 1);

      glcd_outtextxy(0, 30, "Circle saved!    ");
}
void takeParaSect(char itemNumber)
{
      char i;
      char string[60];
      char backup = itemNumber;
      char startAdr = 0x00;
      int parameter[6] = {0, 0, 0, 0, 0, 0};
      float r1, r2;
      sprintf(string, "Mode 1 Enter Circle parameters 1-6 (x, y, x1, y1, x2, y2) for item %d:", itemNumber);
      glcd_clear();
      glcd_outtextxy(0, 0, string);
      for (i = 0; i < 6; i++)
      {
            parameter[i] = getNum(i);
            if (parameter[i] == -1)
                  return;
      }
      r1 = sqrt((parameter[2] - parameter[0]) * (parameter[2] - parameter[0]) + (parameter[3] - parameter[1]) * (parameter[3] - parameter[1]));
      r2 = sqrt((parameter[4] - parameter[0]) * (parameter[4] - parameter[0]) + (parameter[5] - parameter[1]) * (parameter[5] - parameter[1]));
      if (r1 != r2 || parameter[0] > desk[0] || parameter[1] > desk[1] || parameter[2] > desk[0] || parameter[3] > desk[1] || parameter[4] > desk[0] || parameter[5] > desk[1])
      {
            glcd_outtextxy(0, 30, "invalid!    ");
            return;
      }
      startAdr = itemsE2P[backup - 1];
      E2PW(4, startAdr);
      for (i = 0; i < 6; i++)
            E2PW(parameter[i], startAdr + i + 1);

      glcd_outtextxy(0, 30, "Sector saved!    ");
}

char moveHeadToPoint(char x, char y)
{
      if (x < desk[0] && y < desk[1] && x >= 0 && y >= 0)
      {
            while (headx != x)
            {
                  if (headx < x)
                        goStepX(1);
                  else if (headx > x)
                        goStepX(-1);
            }
            while (heady != y)
            {
                  if (heady < y)
                        goStepY(1);
                  else if (heady > y)
                        goStepY(-1);
            }
            return 1;
      }
      Laser(0);
      return 0;
}

void exeLineCut(char adr, char mode, char px1, char px2, char py1, char py2)
{
      char x1 = (!mode) ? E2PR(adr) : px1;
      char y1 = (!mode) ? E2PR(adr + 1) : py1;
      char x2 = (!mode) ? E2PR(adr + 2) : px2;
      char y2 = (!mode) ? E2PR(adr + 3) : py2;
      float dl = 0;
      char safe = 1;
      char startPointX;
      char startPointY;
      char EndPointX;
      char EndPointY;
      signed char i;
      if (x1 <= x2)
      {
            startPointX = x1;
            startPointY = y1;
            EndPointX = x2;
            EndPointY = y2;
      }
      else
      {
            startPointX = x2;
            startPointY = y2;
            EndPointX = x1;
            EndPointY = y1;
      }
      safe = moveHeadToPoint(startPointX, startPointY);
      if (!safe)
            return;
      dl = ((EndPointY - startPointY) * 1.0) / (EndPointX - startPointX);
      i = 0;
      if (dl <= 1 && dl >= -1)
            while (headx < EndPointX)
            {
                  safe = moveHeadToPoint(startPointX + i, dl * i + startPointY);
                  if (!safe)
                        return;
                  Laser(1);
                  i += 1;
            }
      else
      {
            if (startPointY < EndPointY)
                  while (heady < EndPointY)
                  {
                        if (1 / dl != 0)
                              safe = moveHeadToPoint((i - startPointY) / (1.0 * dl) + startPointX, i + startPointY);
                        else
                              goStepY(1);
                        if ((!safe) || !(headx < desk[0] && heady < desk[1] && headx >= 0 && heady >= 0))
                        {
                              Laser(0);
                              return;
                        }
                        Laser(1);
                        i += 1;
                  }
            else
                  while (heady > EndPointY)
                  {
                        if (1 / dl != 0)
                              safe = moveHeadToPoint((i - startPointY) / (1.0 * dl) + startPointX, i + startPointY);
                        else
                              goStepY(-1);
                        if ((!safe) || !(headx < desk[0] && heady < desk[1] && headx >= 0 && heady >= 0))
                        {
                              Laser(0);
                              return;
                        }
                        Laser(1);
                        i -= 1;
                  }
      }
      Laser(0);
}
void exeRectCut(char adr)
{
      char x1 = E2PR(adr);
      char y1 = E2PR(adr + 1);
      char x2 = E2PR(adr + 2);
      char y2 = E2PR(adr + 3);
      char startPointX;
      char startPointY;
      char EndPointX;
      char EndPointY;
      if (x1 <= x2)
      {
            startPointX = x1;
            startPointY = y1;
            EndPointX = x2;
            EndPointY = y2;
      }
      else
      {
            startPointX = x2;
            startPointY = y2;
            EndPointX = x1;
            EndPointY = y1;
      }
      exeLineCut(0, 1, startPointX, EndPointX, startPointY, startPointY);
      exeLineCut(0, 1, EndPointX, EndPointX, startPointY, EndPointY);
      exeLineCut(0, 1, EndPointX, startPointX, EndPointY, EndPointY);
      exeLineCut(0, 1, startPointX, startPointX, EndPointY, startPointY);
}
void exeCircCut(char adr)
{
      char x = E2PR(adr);
      char y = E2PR(adr + 1);
      char r = E2PR(adr + 2);
      int teta = 0;
      char safe = 1;
      safe = moveHeadToPoint(x + r, y);
      if (!safe)
            return;
      while (teta < 360)
      {
            safe = moveHeadToPoint(r * cos((teta * 3.14) / 180) + x, r * sin((teta * 3.14) / 180) + y);
            if (!safe)
                  return;
            Laser(1);
            teta += 1;
      }
      Laser(0);
}
void exeSectCut(char adr)
{
      char x = E2PR(adr);
      char y = E2PR(adr + 1);
      char x1 = E2PR(adr + 2);
      char y1 = E2PR(adr + 3);
      char x2 = E2PR(adr + 4);
      char y2 = E2PR(adr + 5);
      float r = 0;
      int teta = 0;
      int startTeta = 0;
      int EndTeta = 0;
      int startTetaSin = 0;
      int EndTetaSin = 0;
      int startTetaCos = 0;
      int EndTetaCos = 0;
      char safe = 1;
      r = sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));

      startTetaSin = asin((y1 - y) / ((1.0) * r)) * (180 / 3.14);
      EndTetaSin = asin((y2 - y) / ((1.0) * r)) * (180 / 3.14);
      if (startTetaSin < 0)
            startTetaSin += 360;
      if (EndTetaSin < 0)
            EndTetaSin += 360;
      startTetaCos = acos((x1 - x) / ((1.0) * r)) * (180 / 3.14);
      EndTetaCos = acos((x2 - x) / ((1.0) * r)) * (180 / 3.14);
      if (startTetaCos < 0)
            startTetaCos += 360;
      if (EndTetaCos < 0)
            EndTetaCos += 360;

      startTeta = ZoneScope(x1, x, y1, y, startTetaSin, startTetaCos);
      EndTeta = ZoneScope(x2, x, y2, y, EndTetaSin, EndTetaCos);
      if (EndTeta == 0 && startTeta > EndTeta)
            EndTeta = 360;
      safe = moveHeadToPoint(x1, y1);
      if (!safe)
            return;
      teta = startTeta;
      if (startTeta > EndTeta)
      {
            while (teta < 360)
            {
                  safe = moveHeadToPoint(r * cos((teta * 3.14) / 180) + x, r * sin((teta * 3.14) / 180) + y);
                  if (!safe)
                        return;
                  Laser(1);
                  teta += 1;
            }
            Laser(0);
            teta = 0;
      }
      while (teta <= EndTeta)
      {
            safe = moveHeadToPoint(r * cos((teta * 3.14) / 180) + x, r * sin((teta * 3.14) / 180) + y);
            if (!safe)
                  return;
            Laser(1);
            teta += 1;
      }
      Laser(0);
      exeLineCut(0, 1, x1, x, y1, y);
      exeLineCut(0, 1, x2, x, y2, y);
}

void mode1Secion(void)
{
      char item = 0;
      char type = 0;
      char i;
      glcd_outtextxy(0, 0, "mode: 1 select item");
      for (i = 0; i < 5; i++)
      {
            sprintf(stringOut, "%d- item %d", i + 1, i + 1);
            glcd_outtextxy(0, 10 * i + 10, stringOut);
      }
      while ((KeyPressed > '5' || KeyPressed < '1') && KeyPressed != 'c')
            ;
      if (KeyPressed == 'c')
            return;

      item = KeyPressed;
      KeyPressed = 'x';
      glcd_clear();
      sprintf(stringOut, "mode: 1 selcet type");
      glcd_outtextxy(0, 0, stringOut);
      sprintf(stringOut, "1- Line");
      glcd_outtextxy(0, 10, stringOut);
      sprintf(stringOut, "2- Rectangle");
      glcd_outtextxy(0, 20, stringOut);
      sprintf(stringOut, "3- Circle");
      glcd_outtextxy(0, 30, stringOut);
      sprintf(stringOut, "4- Sector");
      glcd_outtextxy(0, 40, stringOut);
      while ((KeyPressed > '4' || KeyPressed < '1') && KeyPressed != 'c')
            ;
      if (KeyPressed == 'c')
            return;

      type = KeyPressed;
      KeyPressed = 'x';
      switch (type - 48)
      {
      case 1:
            takeParaLine(item - 48);
            break;
      case 2:
            takeParaRect(item - 48);
            break;
      case 3:
            takeParaCirc(item - 48);
            break;
      case 4:
            takeParaSect(item - 48);
            break;
      }
}
void mode2Secion(void)
{
      char i;
      char string[60];
      int parameter[2] = {0, 0};
      int tempSpeed = 0;
      sprintf(string, "mode 2: Enter desk parameters 1-2 (x, y):");
      glcd_clear();
      glcd_outtextxy(0, 0, string);
      for (i = 0; i < 2; i++)
      {
            parameter[i] = getNum(i);
            if (parameter[i] == -1)
            {
                  glcd_outtextxy(0, 30, "invalid            ");
                  return;
            }
      }
      desk[0] = parameter[0];
      desk[1] = parameter[1];
      E2PW(desk[0], 0x100);
      E2PW(desk[1], 0x101);
      glcd_outtextxy(0, 30, "desk xy set!    ");
      if (headx > desk[0] || heady > desk[1])
      {
            glcd_outtextxy(0, 30, "needs to recovering system!");
            while (headx != 0)
                  goStepX(-1);
            while (heady != 0)
                  goStepY(-1);
      }
      sprintf(string, "mode 2: Enter motor parameter (rpm):");
      glcd_clear();
      glcd_outtextxy(0, 0, string);
      tempSpeed = getNum(0);
      if (tempSpeed == -1 || tempSpeed < 1 || tempSpeed > 255)
      {
            glcd_outtextxy(0, 30, "invalid            ");
            return;
      }
      speed = tempSpeed;
      stpperDelay = (60000.0 / (18 * speed));
      E2PW(speed, 0x102);
      glcd_outtextxy(0, 30, "motors speed set!    ");
}
void mode3Secion(void)
{
      char i = 0;
      char itemType;
      float scale = 1;
      if (desk[1] > 64)
            scale = (64.0 / desk[1]);
      else if (desk[0] > 128)
            scale = (128.0 / desk[0]);
      glcd_outtextxy(0, 0, "mode: 3\nEnter = to start!");
      while (KeyPressed != 'c' && KeyPressed != '=')
            ;
      if (KeyPressed == 'c')
            return;
      glcd_clear();
      glcd_rectround(0, 0, desk[0] * scale, desk[1] * scale, 0);
      sprintf(stringOut, "%dx%d", desk[0], desk[1]);
      glcd_outtextxy(80, 57, stringOut);
      for (i = 0; i < 5; i++)
      {
            itemType = E2PR(itemsE2P[i]);
            switch (itemType)
            {
            case 1:
                  exeLineCut(itemsE2P[i] + 1, 0, 0, 0, 0, 0);
                  break;
            case 2:
                  exeRectCut(itemsE2P[i] + 1);
                  break;
            case 3:
                  exeCircCut(itemsE2P[i] + 1);
                  break;
            case 4:
                  exeSectCut(itemsE2P[i] + 1);
                  break;
            }
      }
}

void modeControlUnit(void)
{
      KeyPressed = 'x';
      switch (mode)
      {
      case 1:
            mode = 2;
            glcd_clear();
            mode2Secion();
            break;
      case 2:
            mode = 3;
            glcd_clear();
            mode3Secion();
            break;
      case 3:
            mode = 1;
            glcd_clear();
            mode1Secion();
            break;
      }
}

interrupt[EXT_INT2] void ext_int2_isr(void)
{
      char i;
      char Keycode;
      char table[10] = {0xd7, 0xeb, 0xdb, 0xbb, 0xed, 0xdd, 0xbd, 0xee, 0xde, 0xbe};
      Keycode = PINC | 0xF0;
      if (Keycode != 0xFF)
      {
            DDRC = 0x00;
            DDRC = 0x0F;
            PORTC = 0xF0;
            Keycode = Keycode & (PINC | 0x0F);
            while ((PINC | 0x0F) != 0xFF)
                  ;
      }
      DDRC = 0xF0;
      PORTC = 0x0F;
      if (Keycode == 0xe7)
            KeyPressed = 'c';
      if (Keycode == 0xb7)
            KeyPressed = '=';
      if (KeyPressed != '=' && KeyPressed != 'c')
      {
            for (i = 0; i < 10; i++)
                  if (Keycode == table[i])
                  {
                        KeyPressed = i + 48;
                        break;
                  }
            if (i == 10)
                  KeyPressed = 'x';
      }
}

void main(void)
{
      int tempE2P = 0;
      GLCDINIT_t glcd_init_data;
      glcd_init_data.font = font5x7;
      GICR |= (0 << INT1) | (0 << INT0) | (1 << INT2);
      MCUCSR = (0 << ISC2);
      GIFR = (0 << INTF1) | (0 << INTF0) | (1 << INTF2);
      DDRB .6 = 1;
      PORTB .6 = 0;
      DDRD = 0xFF;
      PORTD = 0x00;
#asm("sei")
      glcd_init(&glcd_init_data);
      DDRC = 0xF0;
      PORTC = 0x0F;
      mode = 1;
      tempE2P = E2PR(0x100);
      if (tempE2P != 0xff)
            desk[0] = tempE2P;
      tempE2P = E2PR(0x101);
      if (tempE2P != 0xff)
            desk[1] = tempE2P;
      tempE2P = E2PR(0x102);
      if (tempE2P != 0xff)
            speed = tempE2P;
      stpperDelay = (60000.0 / ((360.0 / rotatePerStep) * speed));
      mode1Secion();

      while (1)
      {
            if (KeyPressed == 'c')
                  modeControlUnit();
      }
}